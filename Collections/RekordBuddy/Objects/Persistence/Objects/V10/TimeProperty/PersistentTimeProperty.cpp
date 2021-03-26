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

#include <RekordBuddyCollectionImplementationPersistence/V10/TimeProperty/PersistentTimeProperty.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTimeProperty;

template class std::shared_ptr<PersistentTimeProperty>;

template class NxA::Array<std::shared_ptr<PersistentTimeProperty>>;
void PersistentTimeProperty::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::TimeProperty,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentTimeProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, timeProperties, Ordered, ToMany),
                      RELATION(PersistentTimeProperty::manySortingPlaylists,
                               sortingPlaylists,
                               SortingPlaylist,
                               Unordered,
                               ToMany,
                               Nullify,
                               maybeTimePropertyUsedToSortBy,
                               Unordered,
                               ToOptionalOne),
                      ATTRIBUTE(PersistentTimeProperty::sortingValueAttribute, sortingValue, Unindexed),
                      ATTRIBUTE(PersistentTimeProperty::timeValueAttribute, timeValue, Unindexed));
}
void PersistentTimeProperty::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyParentTracks->faultRelationship();
    manySortingPlaylists->faultRelationship();
}
void PersistentTimeProperty::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyParentTracks) {
        inverse->manyTimeProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
    for (auto && inverse : *manySortingPlaylists) {
        inverse->maybeTimePropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}

// -- sortingValue
String PersistentTimeProperty::sortingValue() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return sortingValueAttribute;
}
void PersistentTimeProperty::setSortingValue(String sortingValue)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, sortingValueAttribute, sortingValue);
}



// -- timeValue
Time PersistentTimeProperty::timeValue() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return timeValueAttribute;
}
void PersistentTimeProperty::setTimeValue(Time timeValue)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, timeValueAttribute, timeValue);
}



// -- to-many relationship: parentTracks
Array<ObjectID> PersistentTimeProperty::parentTracksObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->objectIDs();
}
Set<ObjectID> PersistentTimeProperty::parentTracksObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTimeProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, timeProperties, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyParentTracks->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTimeProperty::loadParentTracks(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTimeProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, timeProperties, Ordered, ToMany));
    }
    if (!manyParentTracks->internalVector) {
        return 0;
    }
    return manyParentTracks->internalVector->size();
}
typename RBSchema::ObjectID PersistentTimeProperty::objectIDForParentTracksItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTimeProperty::parentTracksConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTimeProperty::parentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
void PersistentTimeProperty::orderParentTracksItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objects), to);
}
void PersistentTimeProperty::orderParentTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objectids), to);
}
void PersistentTimeProperty::orderParentTracksItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(indicies), to);
}
void PersistentTimeProperty::addParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    
    inverse->manyTimeProperties->append(*lockedContext->fetchObject<PersistentTimeProperty>(objectId));
    manyParentTracks->append(inverse);
}
void PersistentTimeProperty::removeAllParentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyParentTracks) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyTimeProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
}
void PersistentTimeProperty::removeParentTracksItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyParentTracks)[index];
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyTimeProperties->removeObjectWithID(objectId);
}
void PersistentTimeProperty::removeParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyTimeProperties->removeObjectWithID(objectId);
}



// -- to-many relationship: sortingPlaylists
Array<ObjectID> PersistentTimeProperty::sortingPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->objectIDs();
}
Set<ObjectID> PersistentTimeProperty::sortingPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTimeProperty::manySortingPlaylists,
                                                       sortingPlaylists,
                                                       SortingPlaylist,
                                                       Unordered,
                                                       ToMany,
                                                       Nullify,
                                                       maybeTimePropertyUsedToSortBy,
                                                       Unordered,
                                                       ToOptionalOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manySortingPlaylists->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTimeProperty::loadSortingPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTimeProperty::manySortingPlaylists,
                                          sortingPlaylists,
                                          SortingPlaylist,
                                          Unordered,
                                          ToMany,
                                          Nullify,
                                          maybeTimePropertyUsedToSortBy,
                                          Unordered,
                                          ToOptionalOne));
    }
    if (!manySortingPlaylists->internalVector) {
        return 0;
    }
    return manySortingPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentTimeProperty::objectIDForSortingPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTimeProperty::sortingPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentTimeProperty::sortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
void PersistentTimeProperty::orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objects), to);
}
void PersistentTimeProperty::orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objectids), to);
}
void PersistentTimeProperty::orderSortingPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(indicies), to);
}
void PersistentTimeProperty::addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeTimePropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTimeProperty>((*inverse->maybeTimePropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    inverse->maybeTimePropertyUsedToSortByOptionalId = objectId;
    manySortingPlaylists->append(inverse);
}
void PersistentTimeProperty::removeAllSortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortingPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeTimePropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}
void PersistentTimeProperty::removeSortingPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortingPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeTimePropertyUsedToSortByOptionalId = {};
}
void PersistentTimeProperty::removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeTimePropertyUsedToSortByOptionalId = {};
}


