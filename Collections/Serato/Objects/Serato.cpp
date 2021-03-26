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

#include <Base/Base.hpp>

namespace NxA { namespace Serato {

// -- Functions

count approximateNumberOfAvailableSeratoCollections()
{
    count numberOfCollectionsFound = 0;

    for (auto&& databaseVolume : Volume::availableVolumes()) {
        if (CollectionImplementation::Database::volumeContainsAValidSeratoDatabase(databaseVolume)) {
            ++numberOfCollectionsFound;
        }
    }

    return numberOfCollectionsFound;
}

Array<Common::CollectionOfSomeSort> availableSeratoCollectionsWithPerCollectionProgressCallBack(const std::function<void(void)>& callback)
{
    static MutableArray<Common::CollectionOfSomeSort> collections;
    static MutableArray<Volume> collectionVolumes;
    MutableArray<Common::CollectionOfSomeSort> results;

    for (auto&& databaseVolume : Volume::availableVolumes()) {
        auto maybeExisting = collectionVolumes.find(databaseVolume);
        if (maybeExisting != collectionVolumes.end()) {
            results.append(collections[maybeExisting - collectionVolumes.begin()]);
        }
        else if (CollectionImplementation::Database::volumeContainsAValidSeratoDatabase(databaseVolume)) {
            auto newCollection = Shared<Common::MutableCollection>{ Shared<MutableCollection>::with(CollectionImplementation::Database::databaseOnVolume(databaseVolume)) };
            results.append(newCollection);

            collections.emplaceAppend(std::move(newCollection));
            collectionVolumes.append(databaseVolume);
        }

        callback();
    }

    return std::move(results);
}

} }
