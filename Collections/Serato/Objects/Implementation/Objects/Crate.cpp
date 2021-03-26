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

#include <SeratoCollection/Tracks/Track.hpp>
#include <SeratoCollection/Crates/Folder.hpp>
#include <SeratoCollection/Crates/Playlist.hpp>

#include <SeratoCollectionImplementation/Crate.hpp>
#include <SeratoCollectionImplementation/Database.hpp>
#include <SeratoCollectionImplementation/Track.hpp>
#include <SeratoCollectionImplementation/Tags/CrateV1Tags.hpp>
#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>
#include <SeratoCollectionImplementation/Tags/TagFactory.hpp>

#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Pointers.hpp>

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

// -- Class Methods

Directory Crate::subCratesDirectoryPathInSeratoFolder(const Directory& seratoFolderPath)
{
    return Directory{FilePath::filePathByJoiningPaths(seratoFolderPath, NXA_FILEPATH("Subcrates"))};
}

Array<String> Crate::readFullCratesNamesInCrateOrderFile(const FilePath& crateOrderFilePath)
{
    MutableArray<String> cratesInOrder;

    auto maybeCrateOrderFile = File::maybeContentOfFileAt(crateOrderFilePath);
    if (maybeCrateOrderFile.isValid()) {
        auto textAsString = String::stringWithUTF16(*maybeCrateOrderFile);
        auto lines = textAsString.splitBySeparator('\n');
        for (auto&& crateLine : lines) {
            if (crateLine.length()) {
                auto crateName = Crate::p_crateNameIfValidCrateOrEmptyIfNot(FilePath{ crateLine });
                if (!crateName.isEmpty()) {
                    if (!cratesInOrder.contains(crateName)) {
                        cratesInOrder.append(crateName);
                    }
                }
            }
        }
    }

    return std::move(cratesInOrder);
}

boolean Crate::filenameIsAValidCratePath(const FilePath& filename)
{
    return !filename.asEncodedString().hasPrefix(".") && filename.hasExtension(NXA_FILEPATH("crate"));
}

Array<String> Crate::cratesInSubCratesDirectory(const Directory& directory)
{
    MutableArray<String> crateNamesFound;

    for (auto&& path : directory.pathsForFilesInDirectory()) {
        auto maybeFileName = path.maybeFileName();
        if (maybeFileName.isValid() && Crate::filenameIsAValidCratePath(*maybeFileName)) {
            crateNamesFound.append(Crate::p_fullCrateNameFromFilename(*maybeFileName));
        }
    }

    return std::move(crateNamesFound);
}

boolean Crate::isAnExistingFullCrateName(const String& fullCrateName, const Directory& seratoFolderPath)
{
    return File::existsAt(Crate::p_crateFilePathForFullCrateNameInSeratoFolder(fullCrateName, seratoFolderPath));
}

boolean Crate::isAnExistingFullSmartCrateName(const String& fullCrateName, const Directory& seratoFolderPath)
{
    return File::existsAt(Crate::p_crateFilePathForFullSmartCrateNameInSeratoFolder(fullCrateName, seratoFolderPath));
}

boolean Crate::createSubCratesFolderIfDoesNotExistsInSeratoFolder(const Directory& seratoFolderPath)
{
    return Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath).createIfDoesNotExist();
}

// -- Constructor & Destructors

Crate::Crate(const String& crateName, const Protected&) : p_name{ Crate::p_escapedNameFromCrateName(crateName).asNormalizedString() },
                                                          p_maybeChildrenCrates{ MutableArray<CollectionImplementation::Crate::ChildrenCrate>{ } } { }

// -- Instance Methods

String Crate::name() const
{
    return this->p_name;
}

void Crate::setName(const String& newName)
{
    this->p_name = newName;
    this->p_markCrateAsModified();
}

String Crate::fullCrateName() const
{
    auto name = this->p_name;
    if (this->p_parentCrate.isValid()) {
        auto parentFullCrateName = (*this->p_parentCrate)->fullCrateName();
        if (parentFullCrateName.length()) {
            return NxA::String::stringWithFormat("%s%%%%%s", parentFullCrateName.asUTF8(), name.asUTF8());
        }
    }

    return NxA::String(name);
}

boolean Crate::addFullCrateNameWithPrefixForCratesAndRecurseToChildren(MutableString& destination, NotNull<const char*> prefix) const
{
    auto fullCrateName = this->fullCrateName();
    NXA_ASSERT_FALSE(fullCrateName.isEmpty());

    boolean somethingWasAdded = false;

    if (this->p_tracks.length()) {
        destination.append(String{prefix.get()});
        destination.append(fullCrateName);
        destination.append("\n"_String);

        somethingWasAdded = true;
    }

    for (auto&& crate : (*this->p_maybeChildrenCrates)) {
        somethingWasAdded = crate.apply([&destination, &prefix](auto& subCrate) {
            return subCrate->p_asSeratoCrate()->addFullCrateNameWithPrefixForCratesAndRecurseToChildren(destination, prefix);
        });
    }

    return somethingWasAdded;
}

count Crate::numberOfTracks() const
{
    return this->p_tracks.length();
}

NotNull<const Common::Track*> Crate::trackAtIndex(count index) const
{
    return this->p_tracks[index];
}

NotNull<Common::MutableTrack*> Crate::trackAtIndex(count index)
{
    return this->p_tracks[index];
}

count Crate::addTrackAtIndex(NotNull<Serato::MutableTrack*> track, count toIndex)
{
    NXA_ASSERT_TRUE(toIndex <= this->p_tracks.length());

    this->p_tracks.insertObjectAtIndex(track, toIndex);

    this->p_markCrateAsModified();

    return toIndex;
}

void Crate::moveTracksAtIndicesToIndex(Array<count> indices, count toIndex)
{
    this->p_tracks.moveObjectsAtIndicesTo(indices, toIndex);

    this->p_markCrateAsModified();
}

void Crate::moveTrackAtIndexTo(count index, count toIndex)
{
    this->p_tracks.moveObjectAtIndexTo(index, toIndex);

    this->p_markCrateAsModified();
}

void Crate::removeTrackAtIndex(count index)
{
    NXA_ASSERT_TRUE(index < this->p_tracks.length());
    this->p_tracks.removeObjectAtIndex(index);

    this->p_markCrateAsModified();
}

count Crate::numberOfCrates() const
{
    return this->p_maybeChildrenCrates->length();
}

Crate::MutableSubCrate Crate::crateAtIndex(count index) const
{
    return (*this->p_maybeChildrenCrates)[index].apply([](auto& crate) {
        return crate->p_asSeratoSubCrate();
    });
}

Crate::MutableSubCrate Crate::crateAtIndex(count index)
{
    return (*this->p_maybeChildrenCrates)[index].apply([](auto& crate) {
        return crate->p_asSeratoSubCrate();
    });
}

bool Crate::crateOrChildrenCrateContainsTracks() const
{
    if (this->numberOfTracks()) {
        return true;
    }

    for (auto&& crate : *this->p_maybeChildrenCrates) {
        auto crateOrChildrenCrateContainsTracks = crate.apply([](auto& crate) {
            return crate->p_asSeratoCrate()->crateOrChildrenCrateContainsTracks();
        });

        if (crateOrChildrenCrateContainsTracks) {
            return true;
        }
    }

    return false;
}

boolean Crate::hasParentCrate() const
{
    return this->p_parentCrate.isValid();
}

Pointer<const Crate> Crate::parentCrate() const
{
    NXA_ASSERT_TRUE(this->hasParentCrate());
    return this->p_parentCrate->as<const Crate*>();
}

Pointer<Crate> Crate::parentCrate()
{
    NXA_ASSERT_TRUE(this->hasParentCrate());
    return this->p_parentCrate->as<Crate*>();
}

boolean Crate::isEmpty() const
{
    if (this->p_maybeChildrenCrates->length()) {
        return false;
    }

    return this->p_tracks.length() == 0;
}

Optional<Time> Crate::maybeLastModificationTimeInSeratoFolder(const Directory& seratoFolderPath) const
{
    if (!this->fullCrateName().length()) {
        // -- For the root folder, we use the later modification time for any crates the collection contains.
        auto mostRecentCrateModification = Time::distantPast();

        auto cratesFolderPath = Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath);
        for (auto&& crateName : CollectionImplementation::Crate::cratesInSubCratesDirectory(cratesFolderPath)) {
            auto crateFilePath = Crate::p_crateFilePathForFullCrateNameInSeratoFolder(crateName, seratoFolderPath);
            if (File::existsAt(crateFilePath, File::IfFileEmptyThenDoesNotExist::No)) {
                auto crateModificationTime = File::modificationTimeForFile(crateFilePath);
                if (crateModificationTime > mostRecentCrateModification) {
                    mostRecentCrateModification = crateModificationTime;
                }
            }
        }

        return mostRecentCrateModification;
    }

    auto crateFilePath = Crate::p_crateFilePathForFullCrateNameInSeratoFolder(this->fullCrateName(), seratoFolderPath);
    if (!File::existsAt(crateFilePath, File::IfFileEmptyThenDoesNotExist::No)) {
        return nothing;
    }

    return File::modificationTimeForFile(crateFilePath);
}

boolean Crate::childrenCratesWereModified() const
{
    return this->p_crateWasModified;
}

void Crate::saveAndRecurseToChildren(const Directory& seratoFolderPath) const
{
    auto& childrenCrates = (*this->p_maybeChildrenCrates);
    if (childrenCrates.length()) {
        for (auto&& crate : *this->p_maybeChildrenCrates) {
            crate.apply([seratoFolderPath](auto& crate) {
                crate->p_asSeratoCrate()->saveAndRecurseToChildren(seratoFolderPath);
            });
        }

        return;
    }

    auto& tracks = this->p_tracks;
    if (!tracks.length()) {
        return;
    }

    // -- We have to make sure this folder always exists otherwise Serato may grab crates from other locations lise 'Crates'.
    if (!Crate::createSubCratesFolderIfDoesNotExistsInSeratoFolder(seratoFolderPath)) {
        return;
    }

    MutableBlob outputData;

    auto versionTag = VersionTag::tagWithIdentifierAndValue(crateVersionTagIdentifier,
                                                            String(crateFileCurrentVersionString));
    versionTag.addTo(outputData);

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("Writing to crate '%s':", this->fullCrateName().asUTF8());
#endif

    for (auto&& tag : this->p_otherTags) {
        tag.addTo(outputData);
    }

    for (auto&& track : tracks) {
        TrackEntry::tagForRelativeFilePath(track->relativeFilePath()).addTo(outputData);
    }

    this->p_saveDataToCrateFileInSeratoFolder(std::move(outputData), seratoFolderPath, this->fullCrateName());
}
