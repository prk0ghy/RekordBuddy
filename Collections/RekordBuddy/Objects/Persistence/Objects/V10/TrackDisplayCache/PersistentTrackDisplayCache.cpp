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

#include <RekordBuddyCollectionImplementationPersistence/V10/TrackDisplayCache/PersistentTrackDisplayCache.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTrackDisplayCache;

template class std::shared_ptr<PersistentTrackDisplayCache>;

template class NxA::Array<std::shared_ptr<PersistentTrackDisplayCache>>;
void PersistentTrackDisplayCache::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::TrackDisplayCache,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentTrackDisplayCache::parentTrackId, parentTrack, Track, Unordered, ToOne, Nullify, displayCache, Unordered, ToOne),
                      ATTRIBUTE(PersistentTrackDisplayCache::cacheDataAttribute, cacheData, Unindexed));
}
void PersistentTrackDisplayCache::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    
}
void PersistentTrackDisplayCache::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    setParentTrack({});
}

// -- cacheData
Blob PersistentTrackDisplayCache::cacheData() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return cacheDataAttribute;
}
void PersistentTrackDisplayCache::setCacheData(Blob cacheData)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, cacheDataAttribute, cacheData);
}



// -- to-one relationship: parentTrack
RBSchema::ObjectID PersistentTrackDisplayCache::parentTrackObjectID() const
{
    return parentTrackId;
}
std::shared_ptr<PersistentTrack> PersistentTrackDisplayCache::parentTrack() const
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
void PersistentTrackDisplayCache::setParentTrack(std::shared_ptr<PersistentTrack> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(this->parentTrackId);
    if (fetchedObject) {
        (*fetchedObject)->displayCacheId = {};
    }
    if (!inverse) {
        parentTrackId = {};
    }
    else {
        parentTrackId = inverse->objectId;
        inverse->displayCacheId = objectId;
    }
}


