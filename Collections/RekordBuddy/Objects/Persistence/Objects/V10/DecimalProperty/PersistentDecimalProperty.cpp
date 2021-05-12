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

#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentDecimalProperty.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentDecimalProperty;

template class NxA::Array<std::shared_ptr<PersistentDecimalProperty>>;
void PersistentDecimalProperty::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::DecimalProperty,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentDecimalProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, decimalProperties, Ordered, ToMany),
                      RELATION(PersistentDecimalProperty::manySortingPlaylists,
                               sortingPlaylists,
                               SortingPlaylist,
                               Unordered,
                               ToMany,
                               Nullify,
                               maybeDecimalPropertyUsedToSortBy,
                               Unordered,
                               ToOptionalOne),
                      ATTRIBUTE(PersistentDecimalProperty::decimalValueAttribute, decimalValue, Unindexed));
}
void PersistentDecimalProperty::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyParentTracks->faultRelationship();
    manySortingPlaylists->faultRelationship();
}
void PersistentDecimalProperty::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyParentTracks) {
        inverse->manyDecimalProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
    for (auto && inverse : *manySortingPlaylists) {
        inverse->maybeDecimalPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}

// -- decimalValue
DecimalNumber PersistentDecimalProperty::decimalValue() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return decimalValueAttribute;
}
void PersistentDecimalProperty::setDecimalValue(DecimalNumber decimalValue)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, decimalValueAttribute, decimalValue);
}



// -- to-many relationship: parentTracks
Array<ObjectID> PersistentDecimalProperty::parentTracksObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->objectIDs();
}
Set<ObjectID> PersistentDecimalProperty::parentTracksObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentDecimalProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, decimalProperties, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyParentTracks->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentDecimalProperty::loadParentTracks(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentDecimalProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, decimalProperties, Ordered, ToMany));
    }
    if (!manyParentTracks->internalVector) {
        return 0;
    }
    return manyParentTracks->internalVector->size();
}
typename RBSchema::ObjectID PersistentDecimalProperty::objectIDForParentTracksItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentDecimalProperty::parentTracksConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentDecimalProperty::parentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
void PersistentDecimalProperty::orderParentTracksItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objects), to);
}
void PersistentDecimalProperty::orderParentTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objectids), to);
}
void PersistentDecimalProperty::orderParentTracksItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(indicies), to);
}
void PersistentDecimalProperty::addParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyDecimalProperties->append(*lockedContext->fetchObject<PersistentDecimalProperty>(objectId));
    manyParentTracks->append(inverse);
}
void PersistentDecimalProperty::removeAllParentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyParentTracks) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyDecimalProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
}
void PersistentDecimalProperty::removeParentTracksItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyParentTracks)[index];
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyDecimalProperties->removeObjectWithID(objectId);
}
void PersistentDecimalProperty::removeParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyDecimalProperties->removeObjectWithID(objectId);
}



// -- to-many relationship: sortingPlaylists
Array<ObjectID> PersistentDecimalProperty::sortingPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->objectIDs();
}
Set<ObjectID> PersistentDecimalProperty::sortingPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentDecimalProperty::manySortingPlaylists,
                                                       sortingPlaylists,
                                                       SortingPlaylist,
                                                       Unordered,
                                                       ToMany,
                                                       Nullify,
                                                       maybeDecimalPropertyUsedToSortBy,
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
count PersistentDecimalProperty::loadSortingPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentDecimalProperty::manySortingPlaylists,
                                          sortingPlaylists,
                                          SortingPlaylist,
                                          Unordered,
                                          ToMany,
                                          Nullify,
                                          maybeDecimalPropertyUsedToSortBy,
                                          Unordered,
                                          ToOptionalOne));
    }
    if (!manySortingPlaylists->internalVector) {
        return 0;
    }
    return manySortingPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentDecimalProperty::objectIDForSortingPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentDecimalProperty::sortingPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentDecimalProperty::sortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
void PersistentDecimalProperty::orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objects), to);
}
void PersistentDecimalProperty::orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objectids), to);
}
void PersistentDecimalProperty::orderSortingPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(indicies), to);
}
void PersistentDecimalProperty::addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeDecimalPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentDecimalProperty>((*inverse->maybeDecimalPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    inverse->maybeDecimalPropertyUsedToSortByOptionalId = objectId;
    manySortingPlaylists->append(inverse);
}
void PersistentDecimalProperty::removeAllSortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortingPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeDecimalPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}
void PersistentDecimalProperty::removeSortingPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortingPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeDecimalPropertyUsedToSortByOptionalId = {};
}
void PersistentDecimalProperty::removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeDecimalPropertyUsedToSortByOptionalId = {};
}
