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

#include <SeratoCollection/Crates/Playlist.hpp>
#include <SeratoCollection/Crates/Folder.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations.
class MutableCollection;
class MutableTrack;

namespace CollectionImplementation {

// -- Forward Declarations.
class Track;
class TrackEntry;

// -- Public Interface
class Database
{
    // -- Constants
    static constexpr auto p_databaseFileCurrentVersionString = "2.0/Serato Scratch LIVE Database";

    // --Private Class Methods
    static void p_saveContentOfRootFolderAndSmartCrateNamesToSeratoFolder(const Crate& rootFolder,
                                                                          const MutableArray<String>& smartCrateNames,
                                                                          const Directory& seratoFolderPath)
    {
        auto subCratesDirectory = Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath);
        auto cratePathsFound = subCratesDirectory.pathsForFilesInDirectory();
        for (auto&& path : cratePathsFound) {
            File::deleteFileAt(path);
        }

        boolean addedSomething = false;

        MutableString result;
        result.append("[begin record]\n");

        for (count index =0 ; index < rootFolder.numberOfCrates() ; ++index) {
            addedSomething = rootFolder.crateAtIndex(index).apply([&seratoFolderPath, &result](auto& subCrate) {
                auto seratoCrate = subCrate->p_asSeratoCrate();

                seratoCrate->saveAndRecurseToChildren(seratoFolderPath);
                return seratoCrate->addFullCrateNameWithPrefixForCratesAndRecurseToChildren(result, "[crate]");
            });
        }

        for (auto&& crateName : smartCrateNames) {
            addedSomething = true;
            result.append("[crate]");
            result.append(crateName);
            result.append("\n");
        }

        result.append("[end record]\n");

        if (addedSomething && Directory{ seratoFolderPath }.createIfDoesNotExist()) {
            // -- We have to make sure this folder always exists otherwise Serato may grab crates from other locations like 'Crates'.
            if (Crate::createSubCratesFolderIfDoesNotExistsInSeratoFolder(seratoFolderPath)) {
                auto crateOrderFilePath = Database::pathForCrateOrderFileInSeratoFolder(seratoFolderPath);
                File::writeBlobToFileAt(result.asUTF16(), crateOrderFilePath);
            }
        }
    }

    static void p_setDatabaseFilesInSeratoFolderAsModifiedOn(const Directory& folderPath, const Time& modificationTime)
    {
        auto databaseFilePath = Database::databaseFilePathForSeratoFolder(folderPath);
        File::setModificationTimeForFile(modificationTime, databaseFilePath);

        auto crateOrderFilePath = Database::pathForCrateOrderFileInSeratoFolder(folderPath);
        if (File::existsAt(crateOrderFilePath)) {
            File::setModificationTimeForFile(modificationTime, crateOrderFilePath);
        }

        auto subCratesDirectory = Crate::subCratesDirectoryPathInSeratoFolder(folderPath);
        auto cratePathsFound = subCratesDirectory.pathsForFilesInDirectory();
        for (auto&& path : cratePathsFound) {
            File::setModificationTimeForFile(modificationTime, path);
        }
    }

#if defined(NXA_PRINT_SERATO_DEBUG_INFO)
    static void p_debugListCrate(const Crate& crate, const String& spacing)
    {
        for (count index = 0; index < crate.numberOfCrates(); ++index) {
            crate.crateAtIndex(index).apply([&spacing](auto&& subCrate) {
                auto subCrateAsSerato = subCrate->p_asSeratoCrate();

                auto crateName = subCrateAsSerato->name();
                NXA_DLOG_WITH_FORMAT("%sCrate '%s'", spacing.asUTF8(), crateName.asUTF8());

                for (count trackIndex = 0; trackIndex < subCrateAsSerato->numberOfTracks(); ++trackIndex) {
                    NXA_DLOG_WITH_FORMAT("%s   Track '%s'", spacing.asUTF8(), subCrateAsSerato->trackAtIndex(trackIndex)->relativeFilePath().asEncodedString().asUTF8());
                }

                MutableString newSpacing(spacing);
                newSpacing.append("   ");

                Database::p_debugListCrate(*subCrateAsSerato, std::move(newSpacing));
            });
        }
    }
#endif

    // -- Instance Variables
    Optional<Unique<Serato::MutableFolder>> p_maybeRootFolder;
    MutableArray<Shared<Serato::MutableTrack>> p_tracks;
    MutableMap<FilePath, NotNull<Serato::MutableTrack*>> p_trackForFilePath;

    Volume p_volumePath;
    Directory p_seratoFolderPath;

    MutableArray<Tag> p_otherTags;
    MutableArray<String> p_smartCrateNames;

    mutable boolean p_databaseTracksWereModified{ false };
    boolean p_isOpened{ false };

    // -- Private Constructors & Destructors
    Database(const Volume& newVolumePath) : p_volumePath{ newVolumePath },
                                            p_seratoFolderPath{ Database::seratoFolderPathForVolumePath(newVolumePath) } { }

    // -- Private Instance Methods
    boolean p_tracksHaveChangesToSave() const;

public:
    // -- Factory Methods
    static Database databaseOnVolume(const Volume&);

    // -- Class Methods
    static Directory seratoFolderPathForVolumePath(const Volume&);
    static String versionAsStringForDatabaseInSeratoFolder(const Directory&);
    static FilePath databaseFilePathForSeratoFolder(const Directory&);
    static boolean volumeContainsAValidSeratoDatabase(const Volume&);
    static FilePath pathForCrateOrderFileInSeratoFolder(const Directory&);
    static void addCratesNamesAtTheEndOfUnlessAlreadyThere(MutableArray<String>& cratesToAddTo, Array<String>& cratesToAdd)
    {
        MutableArray<String> orderedCrateNamesToAdd(cratesToAdd);
        orderedCrateNamesToAdd.sort();

        for (auto&& crateName : orderedCrateNamesToAdd) {
            if (cratesToAddTo.contains(crateName)) {
                continue;
            }

            cratesToAddTo.append(crateName);
        }
    }

    // -- Constructors & Destructors
    Database() = delete;
    Database(const Database&) = delete;
    Database(Database&) = delete;
    Database(Database&&) = default;

    // -- Instance Methods
    Volume volumePath() const;
    Directory seratoFolderPath() const;

    Time modificationTime() const;
    Time rootFolderModificationTime() const;

    NotNull<Serato::MutableFolder*> rootFolder(boolean checkIfOpened = true);

    Optional<NotNull<const Serato::MutableTrack*>> maybeExistingTrackForRelativeFilePath(const FilePath&) const;
    Optional<NotNull<Serato::MutableTrack*>> maybeExistingTrackForRelativeFilePath(const FilePath&);
    NotNull<Serato::MutableTrack*> newTrackForRelativeFilePathAtIndexInCollection(const FilePath&, count, Pointer<Serato::MutableCollection>);

    count numberOfTracks() const;
    NotNull<const Serato::MutableTrack*> trackAtIndex(count) const;
    NotNull<Serato::MutableTrack*> trackAtIndex(count);
    void moveTracksAtIndicesToIndex(Array<count>, count);
    void moveTrackAtIndexTo(count, count);
    Optional<count> maybeIndexForTrack(NotNull<Serato::MutableTrack*>);
    count moveTrackToIndex(NotNull<Serato::MutableTrack*>, count);
    void removeTrack(NotNull<Serato::MutableTrack*>);
    void removeTrackAtIndex(count);
    void removeAllTracks();

    boolean openInCollection(Pointer<Serato::MutableCollection>);
    boolean isOpened() const;
    void save() const {
        this->saveWithProgress([](double p) {});
    }
    void saveWithProgress(std::function<void(double)>&& progress) const;
};

} } }
