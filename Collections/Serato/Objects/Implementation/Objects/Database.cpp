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

#include <SeratoCollectionImplementation/Database.hpp>
#include <SeratoCollectionImplementation/Tags/DatabaseV2Tags.hpp>
#include <SeratoCollectionImplementation/Tags/TagFactory.hpp>
#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

// -- Factory Methods

Database Database::databaseOnVolume(const Volume& volumePath)
{
    return Database{ volumePath };
}

// -- Class Methods

Directory Database::seratoFolderPathForVolumePath(const Volume& volumePath)
{
    auto basePath = (volumePath == Volume::musicFolderVolume()) ? Directory::userMusicDirectory().asFilePath() : volumePath.asFilePath();

    Directory seratoFolderPath{ FilePath::filePathByJoiningPaths(basePath, NXA_FILEPATH("_Serato_")) };
    if (!File::existsAt(Database::databaseFilePathForSeratoFolder(seratoFolderPath))) {
        Directory scratchLiveFolderPath{ FilePath::filePathByJoiningPaths(basePath, NXA_FILEPATH("_ScratchLive_")) };
        if (File::existsAt(Database::databaseFilePathForSeratoFolder(scratchLiveFolderPath))) {
            return scratchLiveFolderPath;
        }
    }

    return seratoFolderPath;
}

String Database::versionAsStringForDatabaseInSeratoFolder(const Directory& seratoFolderPath)
{
    auto maybeDatabaseFile = File::maybeContentOfFileAt(databaseFilePathForSeratoFolder(seratoFolderPath));
    if (maybeDatabaseFile.isValid()) {
        auto tagAddress = maybeDatabaseFile->data();
        auto endOfTagsAddress = tagAddress + maybeDatabaseFile->size();

        while (tagAddress < endOfTagsAddress) {
            if ((Tag::dataSizeForTagAt(tagAddress) > 0) &&
                (Tag::identifierForTagAt(tagAddress) == databaseVersionTagIdentifier)) {
                auto tag = TagFactory::tagForTagAt(tagAddress);
                return String(static_cast<VersionTag&>(tag).value());
            }

            tagAddress = Tag::nextTagAfterTagAt(tagAddress);
        }
    }

    return { };
}

FilePath Database::databaseFilePathForSeratoFolder(const Directory& seratoFolderPath)
{
    return FilePath::filePathByJoiningPaths(seratoFolderPath, NXA_FILEPATH("database V2"));
}

boolean Database::volumeContainsAValidSeratoDatabase(const Volume& volumePath)
{
    return File::existsAt(Database::databaseFilePathForSeratoFolder(Database::seratoFolderPathForVolumePath(volumePath)));
}

FilePath Database::pathForCrateOrderFileInSeratoFolder(const Directory& seratoFolderPath)
{
    return FilePath::filePathByJoiningPaths(seratoFolderPath, NXA_FILEPATH("neworder.pref"));
}

// -- Instance Methods

boolean Database::p_tracksHaveChangesToSave() const
{
    if (this->p_databaseTracksWereModified) {
        return true;
    }

    for (auto&& track : this->p_tracks) {
        auto& seratoTrack = track->asSeratoTrack();

        if (seratoTrack.needsToUpdateDatabaseFile() || seratoTrack.needsToUpdateTrackFile()) {
            return true;
        }
    }

    return false;
}

Volume Database::volumePath() const
{
    return this->p_volumePath;
}

Directory Database::seratoFolderPath() const
{
    return Database::seratoFolderPathForVolumePath(this->p_volumePath);
}

Time Database::modificationTime() const
{
    // -- If we don't have a database file yet, we return a very old date.
    auto latestModificationTime = Time::distantPast();

    auto databaseFilePath = Database::databaseFilePathForSeratoFolder(this->p_seratoFolderPath);
    if (File::existsAt(databaseFilePath)) {
        auto newModificationTime = File::modificationTimeForFile(databaseFilePath);
        if (newModificationTime.isLaterThan(latestModificationTime)) {
            latestModificationTime = newModificationTime;
        }
    }

    return latestModificationTime;
}

Time Database::rootFolderModificationTime() const
{
    auto latestModificationTime = Time::distantPast();

    auto& seratoFolderPath = this->p_seratoFolderPath;
    auto crateOrderFilePath = Database::pathForCrateOrderFileInSeratoFolder(seratoFolderPath);
    if (File::existsAt(crateOrderFilePath)) {
        auto newModificationTime = File::modificationTimeForFile(crateOrderFilePath);
        if (newModificationTime.isLaterThan(latestModificationTime)) {
            latestModificationTime = newModificationTime;
        }
    }

    auto subCratesDirectory = Crate::subCratesDirectoryPathInSeratoFolder(seratoFolderPath);
    auto cratePathsFound = subCratesDirectory.pathsForFilesInDirectory();
    for (auto&& cratePath : cratePathsFound) {
        auto newModificationTime = File::modificationTimeForFile(cratePath);
        if (newModificationTime.isLaterThan(latestModificationTime)) {
            latestModificationTime = newModificationTime;
        }
    }

    if (latestModificationTime == Time::distantPast()) {
        // -- If we couldn't find a modification date for any crate order files, we return
        // -- the database's modification date.
        latestModificationTime = Database::modificationTime();
    }

    return latestModificationTime;
}

NotNull<Serato::MutableFolder*> Database::rootFolder(boolean checkIfOpened)
{
    if (checkIfOpened) {
        NXA_ASSERT_TRUE(this->p_isOpened);
    }

    NXA_ASSERT_TRUE(this->p_maybeRootFolder.isValid());

    return { this->p_maybeRootFolder->asRawPointer() };
}

Optional<NotNull<const Serato::MutableTrack*>> Database::maybeExistingTrackForRelativeFilePath(const FilePath& path) const
{
    return this->p_trackForFilePath.maybeValueForKey(path).maybe([](auto track) {
        return track.template as<const Serato::MutableTrack*>();
    });
}

Optional<NotNull<Serato::MutableTrack*>> Database::maybeExistingTrackForRelativeFilePath(const FilePath& path)
{
    return this->p_trackForFilePath.maybeValueForKey(path);
}

NotNull<Serato::MutableTrack*> Database::newTrackForRelativeFilePathAtIndexInCollection(const FilePath& relativeFilePath,
                                                                                        count index,
                                                                                        Pointer<Serato::MutableCollection> collection)
{
    auto& tracks = this->p_tracks;

    NXA_ASSERT_TRUE(index <= tracks.length());

    this->p_tracks.insertObjectAtIndex(Shared<Serato::MutableTrack>::with(Track{ relativeFilePath,
                                                                                 collection,
                                                                                 Track::p_isProtected },
                                                                          Serato::MutableTrack::p_isProtected),
                                       index);

    auto* newTrackPointer = this->p_tracks[index].asRawPointer();

    // -- We assert to make sure the track is new and not already in our database.
    NXA_ASSERT_TRUE(this->p_trackForFilePath.setValueForKey(newTrackPointer, relativeFilePath));

    this->p_databaseTracksWereModified = true;

    return newTrackPointer;
}

count Database::numberOfTracks() const
{
    return this->p_tracks.length();
}

NotNull<const Serato::MutableTrack*> Database::trackAtIndex(count index) const
{
    return this->p_tracks[index].asRawPointer();
}

NotNull<Serato::MutableTrack*> Database::trackAtIndex(count index)
{
    return this->p_tracks[index].asRawPointer();
}

void Database::moveTracksAtIndicesToIndex(Array<count> indices, count toIndex)
{
    this->p_tracks.moveObjectsAtIndicesTo(indices, toIndex);

    this->p_databaseTracksWereModified = true;
}

void Database::moveTrackAtIndexTo(count index, count toIndex)
{
    this->p_tracks.moveObjectAtIndexTo(index, toIndex);

    this->p_databaseTracksWereModified = true;
}

Optional<count> Database::maybeIndexForTrack(NotNull<Serato::MutableTrack*> track)
{
    auto& tracks = this->p_tracks;
    for (auto trackIterator = tracks.begin(); trackIterator < tracks.end(); ++trackIterator) {
        if (trackIterator->asRawPointer() != track.get()) {
            continue;
        }

        auto maybeIndex = maybeNarrowCast<count>(trackIterator - tracks.begin());
        NXA_ASSERT_TRUE(maybeIndex.isValid());

        return maybeIndex;
    }

    return nothing;
}

count Database::moveTrackToIndex(NotNull<Serato::MutableTrack*> track, count toIndex)
{
    auto& tracks = this->p_tracks;

    for (auto trackIterator = tracks.begin(); trackIterator < tracks.end(); ++trackIterator) {
        if (trackIterator->asRawPointer() != track.get()) {
            continue;
        }

        auto maybeIndexToReArrange = maybeNarrowCast<count>(trackIterator - tracks.begin());
        NXA_ASSERT_TRUE(maybeIndexToReArrange.isValid());

        auto& indexToReArrange = *maybeIndexToReArrange;
        if (indexToReArrange == toIndex) {
            // -- Nothing needs to be changed.
            return toIndex;
        }

        count resultingIndex = indexToReArrange < toIndex ? (toIndex - 1) : toIndex;
        tracks.moveObjectAtIndexTo(indexToReArrange, toIndex);

        this->p_databaseTracksWereModified = true;

        return resultingIndex;
    }

    NXA_ALOG("Can't find track to move.");
}

void Database::removeTrack(NotNull<Serato::MutableTrack*> track)
{
    auto& tracks = this->p_tracks;

    this->p_trackForFilePath.removeValueForKey(track->relativeFilePath());

    for (auto trackIterator = tracks.begin(); trackIterator < tracks.end(); ++trackIterator) {
        if (trackIterator->asRawPointer() != track.get()) {
            continue;
        }

        track->collection()->markAsModifiedNow();

        tracks.removeObjectAt(trackIterator);

        this->p_databaseTracksWereModified = true;

        return;
    }

    NXA_ALOG("Can't find track to remove.");
}

void Database::removeTrackAtIndex(count index)
{
    auto& tracks = this->p_tracks;
    NXA_ASSERT_TRUE(index < tracks.length());

    auto track = tracks[index];
    track->collection()->markAsModifiedNow();

    this->p_trackForFilePath.removeValueForKey(track->relativeFilePath());
    tracks.removeObjectAtIndex(index);

    this->p_databaseTracksWereModified = true;
}

void Database::removeAllTracks()
{
    auto& tracks = this->p_tracks;
    if (tracks.length() == 0) {
        return;
    }

    auto firstTrack = tracks.firstObject();
    firstTrack->collection()->markAsModifiedNow();

    this->p_trackForFilePath.removeAll();
    tracks.removeAll();

    this->p_databaseTracksWereModified = true;
}

boolean Database::openInCollection(Pointer<Serato::MutableCollection> collection)
{
#if defined(NXA_PRINT_SERATO_DEBUG_INFO)
    count previousTrackCount = 0;
    count previousOtherTagsCount = 0;
#endif

    if (Database::volumeContainsAValidSeratoDatabase(this->p_volumePath)) {
        auto databasePath = Database::databaseFilePathForSeratoFolder(this->p_seratoFolderPath);

        auto maybeDatabaseFile = File::maybeContentOfFileAt(databasePath);
        if (!maybeDatabaseFile.isValid()) {
            // -- This should log something.
            return false;
        }

        auto separator = FilePath::nativePathSeparator();

        auto tags = TagFactory::parseTagsAt(maybeDatabaseFile->data(), maybeDatabaseFile->size());
        for (auto&& tag : tags) {
            switch (tag.identifier()) {
                case trackObjectTagIdentifier: {
                    auto& trackTag = static_cast<ObjectTag&>(tag);

                    auto maybeFilePathTag = trackTag.maybeSubTagForIdentifier(trackFilePathTagIdentifier);
                    if (maybeFilePathTag.isValid()) {
                        auto relativeFilePath = static_cast<PathTag&>(*maybeFilePathTag).value();
                        if (relativeFilePath != separator) {
                            this->p_tracks.append(Shared<Serato::MutableTrack>::with(Track{ trackTag,
                                                                                            collection,
                                                                                            Track::p_isProtected },
                                                                                     Serato::MutableTrack::p_isProtected));
                            this->p_trackForFilePath.setValueForKey(this->p_tracks.lastObject().asRawPointer(),
                                                                    relativeFilePath);
                        }
                    }
                    else {
                        this->p_otherTags.append(tag);
                    }
                    break;
                }
                case databaseVersionTagIdentifier: {
                    auto versionText = static_cast<VersionTag&>(tag).value();
                    if (versionText != String{ Database::p_databaseFileCurrentVersionString }) {
                        NXA_ALOG_WITH_FORMAT("Illegal database version for file at '%s'.", databasePath.asEncodedString().asUTF8());
                    }
                    break;
                }
                default: {
                    this->p_otherTags.append(tag);
                    break;
                }
            }
        }
    }

#if defined(NXA_PRINT_SERATO_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("  found %lld tracks and %lld other tags.",
        this->p_tracks.length() - previousTrackCount,
        this->p_otherTags.length() - previousOtherTagsCount);
        previousTrackCount = this->p_tracks.length();
        previousOtherTagsCount = this->p_otherTags.length();
#endif

    this->p_maybeRootFolder = Unique<Serato::MutableFolder>::with(Crate{ String{ }, Crate::p_isProtected },
                                                                  collection,
                                                                  nothing,
                                                                  Serato::MutableFolder::p_isProtected);
    collection.asReference().p_addCratesFoundInSeratoFolderToRootFolderAndSaveSmartCrateNamesIn(this->p_seratoFolderPath, this->p_smartCrateNames);

#if defined(NXA_PRINT_SERATO_DEBUG_INFO)
    Database::p_debugListCrate(*(*this->p_maybeRootFolder)->p_asSeratoCrate(), String());
#endif

    this->p_isOpened = true;

    return true;
}

boolean Database::isOpened() const
{
    return this->p_isOpened;
}

void Database::saveWithProgress(std::function<void(double)>&& progress) const
{
    Directory seratoFolderPath = this->p_seratoFolderPath;
    if (this->p_maybeRootFolder.isValid() && (*this->p_maybeRootFolder)->p_asSeratoCrate()->childrenCratesWereModified()) {
        Database::p_saveContentOfRootFolderAndSmartCrateNamesToSeratoFolder(*(*this->p_maybeRootFolder)->p_asSeratoCrate(),
                                                                            this->p_smartCrateNames,
                                                                            seratoFolderPath);
    }

    if (this->p_tracksHaveChangesToSave()) {
        MutableBlob outputData;

        auto versionTag = VersionTag::tagWithIdentifierAndValue(databaseVersionTagIdentifier,
                                                                String(Database::p_databaseFileCurrentVersionString));
        versionTag.addTo(outputData);

        boolean addedSomething = false;

        double currentCount = 0.0;
        double totalCount = this->p_tracks.length() + this->p_otherTags.length() + 1;
        for (auto&& track : this->p_tracks) {
            addedSomething = true;

            auto& seratoTrack = track->asSeratoTrack();
            seratoTrack.addTo(outputData);

            if (seratoTrack.needsToUpdateTrackFile()) {
                seratoTrack.updateTrackFile();
            }

            currentCount += 1;
            progress(currentCount / totalCount);
        }

        for (auto&& tag : this->p_otherTags) {
            addedSomething = true;

            tag.addTo(outputData);
            currentCount += 1;
            progress(currentCount / totalCount);
        }

        if (addedSomething) {
            seratoFolderPath.createIfDoesNotExist();

            auto databaseFilePath = Database::databaseFilePathForSeratoFolder(seratoFolderPath);
            File::writeBlobToFileAt(std::move(outputData), databaseFilePath);
        }

        this->p_databaseTracksWereModified = false;
    }

    progress(1.0);
}
