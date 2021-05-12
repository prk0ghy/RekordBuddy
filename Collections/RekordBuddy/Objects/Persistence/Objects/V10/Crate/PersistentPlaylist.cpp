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

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentPlaylist.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/SortingCriteria/PersistentSortingCriteria.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentPlaylist;

template class NxA::Array<std::shared_ptr<PersistentPlaylist>>;
void PersistentPlaylist::bind(SourceBinder& parentBinder)
{
    PersistentCrate::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::Playlist,
                      RBSchema::Type::Crate,
                      RELATION(PersistentPlaylist::manySortedBy, sortedBy, SortingCriteria, Ordered, ToMany, Nullify, parentPlaylist, Unordered, ToOne),
                      RELATION(PersistentPlaylist::manyTracks, tracks, Track, Ordered, ToMany, Nullify, parentPlaylists, Unordered, ToMany));
}
void PersistentPlaylist::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentCrate::faultObject();
    manySortedBy->faultRelationship();
    manyTracks->faultRelationship();
}
void PersistentPlaylist::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentCrate::deleteObject();
    for (auto && inverse : *manySortedBy) {
        inverse->parentPlaylistId = {};
    }
    manySortedBy->removeAll();
    for (auto && inverse : *manyTracks) {
        inverse->manyParentPlaylists->removeObjectWithID(objectId);
    }
    manyTracks->removeAll();
}



// -- to-many relationship: sortedBy
Array<ObjectID> PersistentPlaylist::sortedByObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortedBy->objectIDs();
}
Set<ObjectID> PersistentPlaylist::sortedByObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortedBy->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentPlaylist::manySortedBy, sortedBy, SortingCriteria, Ordered, ToMany, Nullify, parentPlaylist, Unordered, ToOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manySortedBy->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentPlaylist::loadSortedBy(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortedBy->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentPlaylist::manySortedBy, sortedBy, SortingCriteria, Ordered, ToMany, Nullify, parentPlaylist, Unordered, ToOne));
    }
    if (!manySortedBy->internalVector) {
        return 0;
    }
    return manySortedBy->internalVector->size();
}
typename RBSchema::ObjectID PersistentPlaylist::objectIDForSortedByItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortedBy->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingCriteria>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentPlaylist::sortedByConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortedBy;
}
PersistentRelationship<std::shared_ptr<PersistentSortingCriteria>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentPlaylist::sortedBy()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortedBy;
}
void PersistentPlaylist::orderSortedByItems(const Array<std::shared_ptr<PersistentSortingCriteria>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortedBy->rearrange(std::move(objects), to);
}
void PersistentPlaylist::orderSortedByItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortedBy->rearrange(std::move(objectids), to);
}
void PersistentPlaylist::orderSortedByItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortedBy->rearrange(std::move(indicies), to);
}
void PersistentPlaylist::addSortedByItem(const std::shared_ptr<PersistentSortingCriteria>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    auto fetchedObject = lockedContext->fetchObject<PersistentPlaylist>(inverse->parentPlaylistId);
    if (fetchedObject) {
        (*fetchedObject)->manySortedBy->removeObjectWithID(objectId);
    }
    inverse->parentPlaylistId = objectId;
    manySortedBy->append(inverse);
}
void PersistentPlaylist::removeAllSortedBy()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortedBy) {
        lockedContext->updateObject(inverse->objectId);
        inverse->parentPlaylistId = {};
    }
    manySortedBy->removeAll();
}
void PersistentPlaylist::removeSortedByItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortedBy)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortedBy->removeObjectWithID(inverse->objectId);
    inverse->parentPlaylistId = {};
}
void PersistentPlaylist::removeSortedByItem(const std::shared_ptr<PersistentSortingCriteria>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortedBy->removeObjectWithID(inverse->objectId);
    inverse->parentPlaylistId = {};
}



// -- to-many relationship: tracks
Array<ObjectID> PersistentPlaylist::tracksObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracks->objectIDs();
}
Set<ObjectID> PersistentPlaylist::tracksObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentPlaylist::manyTracks, tracks, Track, Ordered, ToMany, Nullify, parentPlaylists, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyTracks->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentPlaylist::loadTracks(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentPlaylist::manyTracks, tracks, Track, Ordered, ToMany, Nullify, parentPlaylists, Unordered, ToMany));
    }
    if (!manyTracks->internalVector) {
        return 0;
    }
    return manyTracks->internalVector->size();
}
typename RBSchema::ObjectID PersistentPlaylist::objectIDForTracksItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracks->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentPlaylist::tracksConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracks;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentPlaylist::tracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracks;
}
void PersistentPlaylist::orderTracksItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracks->rearrange(std::move(objects), to);
}
void PersistentPlaylist::orderTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracks->rearrange(std::move(objectids), to);
}
void PersistentPlaylist::orderTracksItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracks->rearrange(std::move(indicies), to);
}
void PersistentPlaylist::addTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyParentPlaylists->append(*lockedContext->fetchObject<PersistentPlaylist>(objectId));
    manyTracks->append(inverse);
}
void PersistentPlaylist::removeAllTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyTracks) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyParentPlaylists->removeObjectWithID(objectId);
    }
    manyTracks->removeAll();
}
void PersistentPlaylist::removeTracksItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyTracks)[index];
    lockedContext->updateObject(inverse->objectId);
    manyTracks->removeObjectWithID(inverse->objectId);
    inverse->manyParentPlaylists->removeObjectWithID(objectId);
}
void PersistentPlaylist::removeTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyTracks->removeObjectWithID(inverse->objectId);
    inverse->manyParentPlaylists->removeObjectWithID(objectId);
}
