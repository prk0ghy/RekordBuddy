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

#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentGridMarker.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentGridMarker;

template class std::shared_ptr<PersistentGridMarker>;

template class NxA::Array<std::shared_ptr<PersistentGridMarker>>;
void PersistentGridMarker::bind(SourceBinder& parentBinder)
{
    PersistentMarker::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::GridMarker,
                      RBSchema::Type::Marker,
                      ATTRIBUTE(PersistentGridMarker::beatNumberAttribute, beatNumber, Unindexed),
                      ATTRIBUTE(PersistentGridMarker::beatsPerMinuteAttribute, beatsPerMinute, Unindexed));
}
void PersistentGridMarker::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentMarker::faultObject();
}
void PersistentGridMarker::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentMarker::deleteObject();
}

// -- beatNumber
integer16 PersistentGridMarker::beatNumber() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return beatNumberAttribute;
}
void PersistentGridMarker::setBeatNumber(integer16 beatNumber)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, beatNumberAttribute, beatNumber);
}



// -- beatsPerMinute
DecimalNumber PersistentGridMarker::beatsPerMinute() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return beatsPerMinuteAttribute;
}
void PersistentGridMarker::setBeatsPerMinute(DecimalNumber beatsPerMinute)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, beatsPerMinuteAttribute, beatsPerMinute);
}


