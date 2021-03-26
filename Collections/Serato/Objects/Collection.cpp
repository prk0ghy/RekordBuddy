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

#include <SeratoCollection/Collection.hpp>

#include <SeratoCollection/Tracks/Track.hpp>
#include <SeratoCollection/Crates/Folder.hpp>
#include <SeratoCollection/Crates/AllTracksPlaylist.hpp>

#include <SeratoCollectionImplementation/Track.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <TrackFiles/TrackFileFactory.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Instance Methods

NotNull<MutableFolder*> MutableCollection::p_rootFolder() const
{
    return this->p_seratoDatabase.rootFolder();
}

NotNull<MutableAllTracksPlaylist*> MutableCollection::p_tracks() const
{
    if (!this->p_maybeAllTracksPlaylist.isValid()) {
        NXA_ASSERT_TRUE(this->isOpened());

        this->p_maybeAllTracksPlaylist = Unique<MutableAllTracksPlaylist>::with(const_cast<MutableCollection*>(this), MutableAllTracksPlaylist::p_isProtected);
    }

    return this->p_maybeAllTracksPlaylist->asRawPointer();
}

void MutableCollection::p_addCratesFoundInSeratoFolderToRootFolderAndSaveSmartCrateNamesIn(const Directory& seratoFolderPath,
                                                                                           MutableArray<String>& smartCrateNames)
{
    auto crateOrderFilePath = CollectionImplementation::Database::pathForCrateOrderFileInSeratoFolder(seratoFolderPath);
    MutableArray<String> fullCratesNamesInOrder(CollectionImplementation::Crate::readFullCratesNamesInCrateOrderFile(crateOrderFilePath));

    auto subCratesDirectory = CollectionImplementation::Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath);
    auto subCratesFound = CollectionImplementation::Crate::cratesInSubCratesDirectory(subCratesDirectory);
    CollectionImplementation::Database::addCratesNamesAtTheEndOfUnlessAlreadyThere(fullCratesNamesInOrder, subCratesFound);

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("Found full crate names  in '%s':", seratoFolderPath.asFilePath().asEncodedString().asUTF8());
    for (auto&& fullCrateName : fullCratesNamesInOrder) {
        NXA_DLOG_WITH_FORMAT("  %s\n", fullCrateName.asUTF8());
    }
#endif

    for (auto&& fullCrateName : fullCratesNamesInOrder) {
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
        NXA_DLOG_WITH_FORMAT("Parsing crate file '%s' in '%s'.", fullCrateName.asUTF8(), seratoFolderPath.asFilePath().asEncodedString().asUTF8());
#endif

        if (!CollectionImplementation::Crate::isAnExistingFullCrateName(fullCrateName, seratoFolderPath)) {
            if (CollectionImplementation::Crate::isAnExistingFullSmartCrateName(fullCrateName, seratoFolderPath)) {
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
                NXA_DLOG("  It's a smart crate.");
#endif
                smartCrateNames.append(fullCrateName);
            }

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
            NXA_DLOG("  It can't be read.");
#endif
            continue;
        }

        this->p_seratoDatabase.rootFolder(false)->p_findOrAddCrateWithRelativeNameInSeratoFolder(fullCrateName,
                                                                                                 fullCrateName,
                                                                                                 seratoFolderPath,
                                                                                                 this->p_ignorePlaylistsThatAreAlsoFoldersOnImport);
    }
}

// -- Common::MutableCollection Overridden Instance Methods

void MutableCollection::notifyUserPreferencesHaveChanged()
{
    auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());

    this->p_artistsSeparator = *userPreferences->maybeStringForKey(String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    this->p_genresSeparator = *userPreferences->maybeStringForKey(String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
    this->p_musicalKeysSeparator = *userPreferences->maybeStringForKey(String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
    this->p_convertMemoryCuesToHotCuesOnExport = *userPreferences->maybeBooleanForKey(String{ NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
    this->p_ignorePlaylistsThatAreAlsoFoldersOnImport = *userPreferences->maybeBooleanForKey(String{ NXA_IGNORE_PLAYLISTS_THAT_ARE_ALSO_FOLDERS_ON_IMPORT_PREFERENCES_KEY });
}

NotNull<const Common::Folder*> MutableCollection::rootFolder() const
{
    return this->p_rootFolder();
}

NotNull<Common::MutableFolder*> MutableCollection::rootFolder()
{
    return this->p_rootFolder();
}

NotNull<const Common::Playlist*> MutableCollection::tracks() const
{
    return this->p_tracks();
}

NotNull<Common::MutablePlaylist*> MutableCollection::tracks()
{
    return this->p_tracks();
}

NotNull<Common::MutableTrack*> MutableCollection::trackWithRelativeFilePath(const FilePath& path)
{
    NXA_ASSERT_TRUE(this->isOpened());

    auto maybeTrack = this->maybeExistingTrackWithRelativeFilePath(path);
    if (maybeTrack.isValid()) {
        return std::move(*maybeTrack);
    }

    auto& seratoDatabase = this->p_seratoDatabase;
    auto newTrack = seratoDatabase.newTrackForRelativeFilePathAtIndexInCollection(path, seratoDatabase.numberOfTracks(), this);

    this->markAsModifiedNow();

    return newTrack.as<Common::MutableTrack*>();
}

Optional<NotNull<const Common::Track*>> MutableCollection::maybeExistingTrackWithRelativeFilePath(const FilePath& path) const
{
    NXA_ASSERT_TRUE(this->isOpened());

    return this->p_seratoDatabase.maybeExistingTrackForRelativeFilePath(path).maybe([](auto track) -> NotNull<const Common::Track*> {
        return track;
    });
}

Optional<NotNull<Common::MutableTrack*>> MutableCollection::maybeExistingTrackWithRelativeFilePath(const FilePath& path)
{
    NXA_ASSERT_TRUE(this->isOpened());

    return this->p_seratoDatabase.maybeExistingTrackForRelativeFilePath(path).maybe([](auto track) -> NotNull<Common::MutableTrack*> {
        return track;
    });
}

void MutableCollection::removeTrackAtIndex(count index)
{
    NXA_ASSERT_TRUE(this->isOpened());

    this->p_seratoDatabase.removeTrackAtIndex(index);

    this->markAsModifiedNow();
}

void MutableCollection::removeAllTracks()
{
    NXA_ASSERT_TRUE(this->isOpened());

    this->p_seratoDatabase.removeAllTracks();

    this->markAsModifiedNow();
}
