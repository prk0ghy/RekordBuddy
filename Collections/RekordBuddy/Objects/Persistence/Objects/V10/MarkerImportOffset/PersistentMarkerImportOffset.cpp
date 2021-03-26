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

#include <RekordBuddyCollectionImplementationPersistence/V10/MarkerImportOffset/PersistentMarkerImportOffset.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentMarkerImportOffset;

template class std::shared_ptr<PersistentMarkerImportOffset>;

template class NxA::Array<std::shared_ptr<PersistentMarkerImportOffset>>;
void PersistentMarkerImportOffset::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::MarkerImportOffset,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentMarkerImportOffset::parentTrackId, parentTrack, Track, Unordered, ToOne, Nullify, markerImportOffsets, Unordered, ToMany),
                      ATTRIBUTE(PersistentMarkerImportOffset::collectionTypeIDAttribute, collectionTypeID, Unindexed),
                      ATTRIBUTE(PersistentMarkerImportOffset::offsetToAddInSecondsAttribute, offsetToAddInSeconds, Unindexed));
}
void PersistentMarkerImportOffset::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    
}
void PersistentMarkerImportOffset::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    setParentTrack({});
}

// -- collectionTypeID
integer32 PersistentMarkerImportOffset::collectionTypeID() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return collectionTypeIDAttribute;
}
void PersistentMarkerImportOffset::setCollectionTypeID(integer32 collectionTypeID)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, collectionTypeIDAttribute, collectionTypeID);
}



// -- offsetToAddInSeconds
DecimalNumber PersistentMarkerImportOffset::offsetToAddInSeconds() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return offsetToAddInSecondsAttribute;
}
void PersistentMarkerImportOffset::setOffsetToAddInSeconds(DecimalNumber offsetToAddInSeconds)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, offsetToAddInSecondsAttribute, offsetToAddInSeconds);
}



// -- to-one relationship: parentTrack
RBSchema::ObjectID PersistentMarkerImportOffset::parentTrackObjectID() const
{
    return parentTrackId;
}
std::shared_ptr<PersistentTrack> PersistentMarkerImportOffset::parentTrack() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!parentTrackId.isValid()) {
        return {};
    }
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(parentTrackId);
    if (!fetchedObject) {
        return {};
    }
    return *fetchedObject;
}
void PersistentMarkerImportOffset::setParentTrack(std::shared_ptr<PersistentTrack> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(this->parentTrackId);
    if (fetchedObject) {
        (*fetchedObject)->manyMarkerImportOffsets->removeObjectWithID(objectId);
    }
    if (!inverse) {
        parentTrackId = {};
    }
    else {
        parentTrackId = inverse->objectId;
        inverse->manyMarkerImportOffsets->append(sharedFromThis<PersistentMarkerImportOffset>());
    }
}


