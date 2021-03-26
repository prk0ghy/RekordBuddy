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

#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentIntegerProperty.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentIntegerProperty;

template class std::shared_ptr<PersistentIntegerProperty>;

template class NxA::Array<std::shared_ptr<PersistentIntegerProperty>>;
void PersistentIntegerProperty::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::IntegerProperty,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentIntegerProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, integerProperties, Ordered, ToMany),
                      RELATION(PersistentIntegerProperty::manySortingPlaylists,
                               sortingPlaylists,
                               SortingPlaylist,
                               Unordered,
                               ToMany,
                               Nullify,
                               maybeIntegerPropertyUsedToSortBy,
                               Unordered,
                               ToOptionalOne),
                      ATTRIBUTE(PersistentIntegerProperty::integerValueAttribute, integerValue, Unindexed));
}
void PersistentIntegerProperty::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyParentTracks->faultRelationship();
    manySortingPlaylists->faultRelationship();
}
void PersistentIntegerProperty::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyParentTracks) {
        inverse->manyIntegerProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
    for (auto && inverse : *manySortingPlaylists) {
        inverse->maybeIntegerPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}

// -- integerValue
integer32 PersistentIntegerProperty::integerValue() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return integerValueAttribute;
}
void PersistentIntegerProperty::setIntegerValue(integer32 integerValue)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, integerValueAttribute, integerValue);
}



// -- to-many relationship: parentTracks
Array<ObjectID> PersistentIntegerProperty::parentTracksObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->objectIDs();
}
Set<ObjectID> PersistentIntegerProperty::parentTracksObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentIntegerProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, integerProperties, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyParentTracks->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentIntegerProperty::loadParentTracks(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentIntegerProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, integerProperties, Ordered, ToMany));
    }
    if (!manyParentTracks->internalVector) {
        return 0;
    }
    return manyParentTracks->internalVector->size();
}
typename RBSchema::ObjectID PersistentIntegerProperty::objectIDForParentTracksItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentIntegerProperty::parentTracksConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentIntegerProperty::parentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
void PersistentIntegerProperty::orderParentTracksItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objects), to);
}
void PersistentIntegerProperty::orderParentTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objectids), to);
}
void PersistentIntegerProperty::orderParentTracksItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(indicies), to);
}
void PersistentIntegerProperty::addParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    
    inverse->manyIntegerProperties->append(*lockedContext->fetchObject<PersistentIntegerProperty>(objectId));
    manyParentTracks->append(inverse);
}
void PersistentIntegerProperty::removeAllParentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyParentTracks) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyIntegerProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
}
void PersistentIntegerProperty::removeParentTracksItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyParentTracks)[index];
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyIntegerProperties->removeObjectWithID(objectId);
}
void PersistentIntegerProperty::removeParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyIntegerProperties->removeObjectWithID(objectId);
}



// -- to-many relationship: sortingPlaylists
Array<ObjectID> PersistentIntegerProperty::sortingPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->objectIDs();
}
Set<ObjectID> PersistentIntegerProperty::sortingPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentIntegerProperty::manySortingPlaylists,
                                                       sortingPlaylists,
                                                       SortingPlaylist,
                                                       Unordered,
                                                       ToMany,
                                                       Nullify,
                                                       maybeIntegerPropertyUsedToSortBy,
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
count PersistentIntegerProperty::loadSortingPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentIntegerProperty::manySortingPlaylists,
                                          sortingPlaylists,
                                          SortingPlaylist,
                                          Unordered,
                                          ToMany,
                                          Nullify,
                                          maybeIntegerPropertyUsedToSortBy,
                                          Unordered,
                                          ToOptionalOne));
    }
    if (!manySortingPlaylists->internalVector) {
        return 0;
    }
    return manySortingPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentIntegerProperty::objectIDForSortingPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentIntegerProperty::sortingPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentIntegerProperty::sortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
void PersistentIntegerProperty::orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objects), to);
}
void PersistentIntegerProperty::orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objectids), to);
}
void PersistentIntegerProperty::orderSortingPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(indicies), to);
}
void PersistentIntegerProperty::addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeIntegerPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentIntegerProperty>((*inverse->maybeIntegerPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    inverse->maybeIntegerPropertyUsedToSortByOptionalId = objectId;
    manySortingPlaylists->append(inverse);
}
void PersistentIntegerProperty::removeAllSortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortingPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeIntegerPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}
void PersistentIntegerProperty::removeSortingPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortingPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeIntegerPropertyUsedToSortByOptionalId = {};
}
void PersistentIntegerProperty::removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeIntegerPropertyUsedToSortByOptionalId = {};
}


