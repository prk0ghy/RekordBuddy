//
//  Rekord Buddy - The future proof music collection tool made by DJs for DJs.
//  Copyright (C) 2020-2021 Didier Malenfant (didier@rekordbuddy.org)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <SeratoCollectionImplementation/Crate.hpp>
#include <SeratoCollectionImplementation/TrackEntry.hpp>
#include <SeratoCollectionImplementation/Tags/Tag.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>

#include <CommonCollection/Tracks/Track.hpp>
#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations
class MutableTrack;
class MutablePlaylist;
class MutableFolder;

namespace CollectionImplementation {

// -- Forward Declaration
class Track;
class Database;

// -- Public Interface
class Crate
{
    // -- Friends
    friend class Database;
    friend class Serato::MutablePlaylist;
    friend class Serato::MutableFolder;

#if defined(NXA_BUILD_FOR_TESTING)
    friend class CrateTests;
#endif

    // -- Types
    using ChildrenCrate = Variant<Unique<Serato::MutablePlaylist>, Unique<Serato::MutableFolder>>;
    using MutableSubCrate = Variant<NotNull<Serato::MutablePlaylist*>, NotNull<Serato::MutableFolder*>>;

    // -- Class Methods
    static String p_crateNameIfValidCrateOrEmptyIfNot(const FilePath& name)
    {
        auto result = name.maybeWithPrefixRemoved(NXA_FILEPATH("[crate]"));
        if (result.isValid()) {
            return (*result).asEncodedString().asNormalizedString();
        }
        return { };
    }

    static String p_escapedNameFromCrateName(const String& crateName)
    {
        MutableString result(crateName);
        result.replaceOccurenceOfWith("/", "|");
        result.replaceOccurenceOfWith("\\", "|");
        result.replaceOccurenceOfWith("%%", "_");

        return std::move(result);
    }

    static String p_topParentCrateNameFromFullCrateName(const String& fullCrateName)
    {
        auto maybeFirstSeparatorIndex = fullCrateName.maybeIndexOfFirstOccurenceOf("%%");
        if (maybeFirstSeparatorIndex.isValid()) {
            return fullCrateName.subString(0, *maybeFirstSeparatorIndex);
        }

        return fullCrateName;
    }

    static Directory p_smartCratesDirectoryPathInSeratoFolder(const Directory& seratoFolderPath)
    {
        return Directory{FilePath::filePathByJoiningPaths(seratoFolderPath, NXA_FILEPATH("SmartCrates"))};
    }

    static FilePath p_crateFilePathForFullCrateNameInSeratoFolder(const String& fullCrateName, const Directory& seratoFolderPath)
    {
        NXA_ASSERT_FALSE(fullCrateName.isEmpty());

        auto cratesFolderPath = Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath);
        return FilePath::filePathByJoiningPaths(cratesFolderPath, FilePath{ fullCrateName.stringByAppending(".crate") });
    }

    static FilePath p_crateFilePathForFullSmartCrateNameInSeratoFolder(const String& fullCrateName, const Directory& seratoFolderPath)
    {
        NXA_ASSERT_FALSE(fullCrateName.isEmpty());

        auto cratesFolderPath = Crate::p_smartCratesDirectoryPathInSeratoFolder(seratoFolderPath);
        return FilePath::filePathByJoiningPaths(cratesFolderPath, FilePath{ fullCrateName.stringByAppending(".scrate") });
    }

    static String p_fullCrateNameFromFilename(const FilePath& fileName)
    {
        auto filenameAsString = fileName.asPlatformNativeString();
        return String{ filenameAsString.substr(0, filenameAsString.size() - 6) }.asNormalizedString();
    }

    // -- Instance Variables
    String p_name;

    mutable boolean p_crateWasModified;

    Optional<NotNull<Crate*>> p_parentCrate;
    mutable Optional<MutableArray<ChildrenCrate>> p_maybeChildrenCrates;

    MutableArray<NotNull<Serato::MutableTrack*>> p_tracks;
    MutableArray<Tag> p_otherTags;

    // -- Instance Methods
    void p_markCrateAsModified()
    {
        this->p_crateWasModified = true;

        if (this->p_parentCrate.isValid()) {
            (*this->p_parentCrate)->p_markCrateAsModified();
        }
    }

    void p_saveDataToCrateFileInSeratoFolder(const Blob& data, const Directory& seratoFolderPath, const String& fullCrateName) const
    {
        if (Directory{ seratoFolderPath }.createIfDoesNotExist()) {
            // -- We have to make sure this folder always exists otherwise Serato may grab crates from other locations like 'Crates'.
            if (Crate::createSubCratesFolderIfDoesNotExistsInSeratoFolder(seratoFolderPath)) {
                auto crateFilePath = Crate::p_crateFilePathForFullCrateNameInSeratoFolder(fullCrateName, seratoFolderPath);

                File::writeBlobToFileAt(data, crateFilePath);

                this->p_crateWasModified = false;
            }
        }
    }

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constants
    static constexpr const character* crateFileCurrentVersionString = "1.0/Serato ScratchLive Crate";

    // -- Class Methods
    static Directory subCratesDirectoryPathInSeratoFolder(const Directory&);
    static Array<String> readFullCratesNamesInCrateOrderFile(const FilePath&);
    static boolean filenameIsAValidCratePath(const FilePath&);
    static Array<String> cratesInSubCratesDirectory(const Directory&);
    static boolean isAnExistingFullCrateName(const String&, const Directory&);
    static boolean isAnExistingFullSmartCrateName(const String&, const Directory&);
    static boolean createSubCratesFolderIfDoesNotExistsInSeratoFolder(const Directory&);

    // -- Constructors & Destructors
    Crate(const String&, const Protected&);
    Crate(const Crate&) = delete;
    Crate(Crate&) = delete;
    Crate(Crate&&) = default;
    virtual ~Crate() = default;

    // -- Instance Methods
    String name() const;
    void setName(const String&);

    String fullCrateName() const;

    boolean addFullCrateNameWithPrefixForCratesAndRecurseToChildren(MutableString&, NotNull<const char*>) const;

    count numberOfTracks() const;
    NotNull<const Common::Track*> trackAtIndex(count) const;
    NotNull<Common::MutableTrack*> trackAtIndex(count);
    count addTrackAtIndex(NotNull<Serato::MutableTrack*>, count);
    void moveTracksAtIndicesToIndex(Array<count>, count);
    void moveTrackAtIndexTo(count, count);
    void removeTrackAtIndex(count);

    count numberOfCrates() const;
    MutableSubCrate crateAtIndex(count) const;
    MutableSubCrate crateAtIndex(count);

    bool crateOrChildrenCrateContainsTracks() const;

    boolean hasParentCrate() const;
    Pointer<const Crate> parentCrate() const;
    Pointer<Crate> parentCrate();

    boolean isEmpty() const;

    Optional<Time> maybeLastModificationTimeInSeratoFolder(const Directory&) const;

    void saveAndRecurseToChildren(const Directory&) const;
    boolean childrenCratesWereModified() const;
};

} } }
