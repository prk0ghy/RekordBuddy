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

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentStringProperty;

template class std::shared_ptr<PersistentStringProperty>;

template class NxA::Array<std::shared_ptr<PersistentStringProperty>>;
void PersistentStringProperty::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::StringProperty,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentStringProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, stringProperties, Ordered, ToMany),
                      RELATION(PersistentStringProperty::manySortingPlaylists,
                               sortingPlaylists,
                               SortingPlaylist,
                               Unordered,
                               ToMany,
                               Nullify,
                               maybeStringPropertyUsedToSortBy,
                               Unordered,
                               ToOptionalOne),
                      ATTRIBUTE(PersistentStringProperty::stringValueAttribute, stringValue, Unindexed));
}
void PersistentStringProperty::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyParentTracks->faultRelationship();
    manySortingPlaylists->faultRelationship();
}
void PersistentStringProperty::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyParentTracks) {
        inverse->manyStringProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
    for (auto && inverse : *manySortingPlaylists) {
        inverse->maybeStringPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}

// -- stringValue
String PersistentStringProperty::stringValue() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return stringValueAttribute;
}
void PersistentStringProperty::setStringValue(String stringValue)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, stringValueAttribute, stringValue);
}



// -- to-many relationship: parentTracks
Array<ObjectID> PersistentStringProperty::parentTracksObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->objectIDs();
}
Set<ObjectID> PersistentStringProperty::parentTracksObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentStringProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, stringProperties, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyParentTracks->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentStringProperty::loadParentTracks(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyParentTracks->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentStringProperty::manyParentTracks, parentTracks, Track, Unordered, ToMany, Nullify, stringProperties, Ordered, ToMany));
    }
    if (!manyParentTracks->internalVector) {
        return 0;
    }
    return manyParentTracks->internalVector->size();
}
typename RBSchema::ObjectID PersistentStringProperty::objectIDForParentTracksItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentTracks->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentStringProperty::parentTracksConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentStringProperty::parentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentTracks;
}
void PersistentStringProperty::orderParentTracksItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objects), to);
}
void PersistentStringProperty::orderParentTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(objectids), to);
}
void PersistentStringProperty::orderParentTracksItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentTracks->rearrange(std::move(indicies), to);
}
void PersistentStringProperty::addParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    
    inverse->manyStringProperties->append(*lockedContext->fetchObject<PersistentStringProperty>(objectId));
    manyParentTracks->append(inverse);
}
void PersistentStringProperty::removeAllParentTracks()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyParentTracks) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyStringProperties->removeObjectWithID(objectId);
    }
    manyParentTracks->removeAll();
}
void PersistentStringProperty::removeParentTracksItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyParentTracks)[index];
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyStringProperties->removeObjectWithID(objectId);
}
void PersistentStringProperty::removeParentTracksItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyParentTracks->removeObjectWithID(inverse->objectId);
    inverse->manyStringProperties->removeObjectWithID(objectId);
}



// -- to-many relationship: sortingPlaylists
Array<ObjectID> PersistentStringProperty::sortingPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->objectIDs();
}
Set<ObjectID> PersistentStringProperty::sortingPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentStringProperty::manySortingPlaylists,
                                                       sortingPlaylists,
                                                       SortingPlaylist,
                                                       Unordered,
                                                       ToMany,
                                                       Nullify,
                                                       maybeStringPropertyUsedToSortBy,
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
count PersistentStringProperty::loadSortingPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentStringProperty::manySortingPlaylists,
                                          sortingPlaylists,
                                          SortingPlaylist,
                                          Unordered,
                                          ToMany,
                                          Nullify,
                                          maybeStringPropertyUsedToSortBy,
                                          Unordered,
                                          ToOptionalOne));
    }
    if (!manySortingPlaylists->internalVector) {
        return 0;
    }
    return manySortingPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentStringProperty::objectIDForSortingPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentStringProperty::sortingPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentStringProperty::sortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
void PersistentStringProperty::orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objects), to);
}
void PersistentStringProperty::orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objectids), to);
}
void PersistentStringProperty::orderSortingPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(indicies), to);
}
void PersistentStringProperty::addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeStringPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentStringProperty>((*inverse->maybeStringPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    inverse->maybeStringPropertyUsedToSortByOptionalId = objectId;
    manySortingPlaylists->append(inverse);
}
void PersistentStringProperty::removeAllSortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortingPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeStringPropertyUsedToSortByOptionalId = {};
    }
    manySortingPlaylists->removeAll();
}
void PersistentStringProperty::removeSortingPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortingPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeStringPropertyUsedToSortByOptionalId = {};
}
void PersistentStringProperty::removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->maybeStringPropertyUsedToSortByOptionalId = {};
}


