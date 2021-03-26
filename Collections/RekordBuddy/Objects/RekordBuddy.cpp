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

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>
#include <RekordBuddyCollection/RekordBuddy.hpp>
#include <RekordBuddyCollection/UserPreferences.hpp>

namespace NxA { namespace RekordBuddy {

// -- Functions

Directory rekordBuddyFolderPathOnVolume(const Volume& volumePath)
{
    FilePath basePath = (volumePath == Volume::musicFolderVolume()) ?
                        Directory::userMusicDirectory().asFilePath() : volumePath.asFilePath();
    return Directory{ FilePath::filePathByJoiningPaths(basePath, NXA_FILEPATH("Rekord Buddy")) };
}

FilePath rekordBuddyDatabasePathOnVolume(const Volume& volumePath)
{
    return FilePath::filePathByJoiningPaths(rekordBuddyFolderPathOnVolume(volumePath),
                                            NXA_FILEPATH("collection.db"));
}

Array<Volume> otherVolumesWithNoRekordBuddyCollection()
{
    auto& musicFolderVolume = Volume::musicFolderVolume();

    MutableArray<Volume> results;

    for (auto&& volume : Volume::availableVolumes()) {
        if (volume == musicFolderVolume) {
            continue;
        }

        auto databasePath = rekordBuddyDatabasePathOnVolume(volume);
        if (!File::existsAt(databasePath)) {
            results.append(volume);
        }
    }

    return { std::move (results) };
}

static count numberOfOtherVolumesWithAnyRekordBuddyCollection()
{
    auto& musicFolderVolume = Volume::musicFolderVolume();

    count numberOfCollectionsFound = 0;

    for (auto&& volume : Volume::availableVolumes()) {
        if (volume == musicFolderVolume) {
            continue;
        }

        auto databasePath = rekordBuddyDatabasePathOnVolume(volume);
        if (File::existsAt(databasePath)) {
            ++numberOfCollectionsFound;
        }
    }

    return numberOfCollectionsFound;
}

Array<Volume> volumesWithRekordBuddyCollection()
{
    auto& musicFolderVolume = Volume::musicFolderVolume();

    MutableArray<Volume> results;

    for (auto&& volume : Volume::availableVolumes()) {
        // -- We always want to have a collection on the music folder volume.
        if (volume != musicFolderVolume ) {
            auto databasePath = rekordBuddyDatabasePathOnVolume(volume);
            if (!File::existsAt(databasePath)) {
                continue;
            }

            CrashLog::addBreadCrumb(String::stringWithFormat("Found a RB Db on external volume '%s' (%s) at '%s'.",
                                                             volume.name().asUTF8(),
                                                             volume.asFilePath().asEncodedString().asUTF8(),
                                                             databasePath.asEncodedString().asUTF8()));
        }

        results.append(volume);
    }

    return { std::move (results) };
}

boolean makeSureRekorBuddyCollectionExistsOnVolume(const Volume& volume)
{
    auto folderPath = rekordBuddyFolderPathOnVolume(volume);
    if (!folderPath.createIfDoesNotExist()) {
        NXA_BETA_LOG_WITH_FORMAT("Can't create RKB folder at '%s'.", folderPath.asFilePath().asEncodedString().asUTF8());
        return false;
    }

    auto newOrExistingCollection = Shared<MutableCollection>::with(volume);
    if (newOrExistingCollection->open().isValid()) {
        NXA_BETA_LOG_WITH_FORMAT("Can't create RKB database on volume '%s'.", volume.asFilePath().asEncodedString().asUTF8());
        return false;
    }

    newOrExistingCollection->save();

    return true;
}

count approximateNumberOfAvailableRekordBuddyCollections()
{
    // -- We always have at least the home volume one.
    return 1 + numberOfOtherVolumesWithAnyRekordBuddyCollection();
}

Array<Common::CollectionOfSomeSort> availableRekordBuddyCollectionsWithPerCollectionProgressCallBack(const std::function<void(void)>& callback)
{
    static MutableMap<Volume, Common::CollectionOfSomeSort> collectionsAlreadyOpened;

    CrashLog::addBreadCrumb(String::stringWithFormat("Main Rekord Buddy DB is on volume '%s'.", Volume::musicFolderVolume().asFilePath().asEncodedString().asUTF8()));

    MutableArray<Common::CollectionOfSomeSort> results;
    for (auto&& volume : volumesWithRekordBuddyCollection()) {
        auto maybeExisting = collectionsAlreadyOpened.maybeValueForKey(volume);
        if (maybeExisting.isValid()) {
            results.append(*maybeExisting);
        }
        else {
            auto newRekordBuddyCollection = Shared<MutableCollection>::with(volume);
            newRekordBuddyCollection->setShouldBeOpenedLazily(volume != Volume::musicFolderVolume());

            Common::CollectionOfSomeSort newCollection{ Shared<Common::Collection>{ newRekordBuddyCollection } };

            newCollection = Common::CollectionOfSomeSort{ Shared<Common::MutableCollection>{ newRekordBuddyCollection } };

            collectionsAlreadyOpened.setValueForKey(newCollection, volume);
            results.append(newCollection);
        }

        callback();
    }

    return std::move(results);
}

void deleteRekordBuddyCollectionOnVolume(const Volume& volume)
{
    File::deleteFileAt(rekordBuddyDatabasePathOnVolume(volume));
}

} }
