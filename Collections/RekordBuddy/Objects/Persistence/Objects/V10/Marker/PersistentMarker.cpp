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

#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentMarker;

template class std::shared_ptr<PersistentMarker>;

template class NxA::Array<std::shared_ptr<PersistentMarker>>;
void PersistentMarker::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::Marker,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentMarker::parentTrackId, parentTrack, Track, Unordered, ToOne, Nullify, markers, Ordered, ToMany),
                      ATTRIBUTE(PersistentMarker::flagsAttribute, flags, Unindexed),
                      ATTRIBUTE(PersistentMarker::positionInSecondsAttribute, positionInSeconds, Unindexed));
}
void PersistentMarker::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    
}
void PersistentMarker::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    setParentTrack({});
}

// -- flags
integer64 PersistentMarker::flags() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return flagsAttribute;
}
void PersistentMarker::setFlags(integer64 flags)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, flagsAttribute, flags);
}



// -- positionInSeconds
DecimalNumber PersistentMarker::positionInSeconds() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return positionInSecondsAttribute;
}
void PersistentMarker::setPositionInSeconds(DecimalNumber positionInSeconds)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, positionInSecondsAttribute, positionInSeconds);
}



// -- to-one relationship: parentTrack
RBSchema::ObjectID PersistentMarker::parentTrackObjectID() const
{
    return parentTrackId;
}
std::shared_ptr<PersistentTrack> PersistentMarker::parentTrack() const
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
void PersistentMarker::setParentTrack(std::shared_ptr<PersistentTrack> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(this->parentTrackId);
    if (fetchedObject) {
        (*fetchedObject)->manyMarkers->removeObjectWithID(objectId);
    }
    if (!inverse) {
        parentTrackId = {};
    }
    else {
        parentTrackId = inverse->objectId;
        inverse->manyMarkers->append(sharedFromThis<PersistentMarker>());
    }
}


