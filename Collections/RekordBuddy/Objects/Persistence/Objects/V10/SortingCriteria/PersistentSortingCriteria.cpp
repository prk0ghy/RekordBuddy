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

#include <RekordBuddyCollectionImplementationPersistence/V10/SortingCriteria/PersistentSortingCriteria.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentSortingCriteria;

template class NxA::Array<std::shared_ptr<PersistentSortingCriteria>>;
void PersistentSortingCriteria::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::SortingCriteria,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentSortingCriteria::parentPlaylistId, parentPlaylist, Playlist, Unordered, ToOne, Nullify, sortedBy, Ordered, ToMany),
                      RELATION(PersistentSortingCriteria::manySortingPlaylists, sortingPlaylists, SortingPlaylist, Unordered, ToMany, Nullify, criteria, Unordered, ToOne),
                      ATTRIBUTE(PersistentSortingCriteria::ascendingAttribute, ascending, Unindexed),
                      ATTRIBUTE(PersistentSortingCriteria::shouldOnlySortTagsWithSamePropertyDescriptionAttribute, shouldOnlySortTagsWithSamePropertyDescription, Unindexed),
                      ATTRIBUTE(PersistentSortingCriteria::sortingPropertyDescriptionAttribute, sortingPropertyDescription, Unindexed),
                      ATTRIBUTE(PersistentSortingCriteria::sortsABooleanTypeOfTagAttribute, sortsABooleanTypeOfTag, Unindexed),
                      ATTRIBUTE(PersistentSortingCriteria::tagTypeBeingSortedAttribute, tagTypeBeingSorted, Unindexed));
}
void PersistentSortingCriteria::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();

    manySortingPlaylists->faultRelationship();
}
void PersistentSortingCriteria::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    setParentPlaylist({});
    for (auto && inverse : *manySortingPlaylists) {
        inverse->criteriaId = {};
    }
    manySortingPlaylists->removeAll();
}

// -- ascending
boolean PersistentSortingCriteria::ascending() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return ascendingAttribute;
}
void PersistentSortingCriteria::setAscending(boolean ascending)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, ascendingAttribute, ascending);
}



// -- shouldOnlySortTagsWithSamePropertyDescription
boolean PersistentSortingCriteria::shouldOnlySortTagsWithSamePropertyDescription() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return shouldOnlySortTagsWithSamePropertyDescriptionAttribute;
}
void PersistentSortingCriteria::setShouldOnlySortTagsWithSamePropertyDescription(boolean shouldOnlySortTagsWithSamePropertyDescription)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, shouldOnlySortTagsWithSamePropertyDescriptionAttribute, shouldOnlySortTagsWithSamePropertyDescription);
}



// -- sortingPropertyDescription
String PersistentSortingCriteria::sortingPropertyDescription() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return sortingPropertyDescriptionAttribute;
}
void PersistentSortingCriteria::setSortingPropertyDescription(String sortingPropertyDescription)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, sortingPropertyDescriptionAttribute, sortingPropertyDescription);
}



// -- sortsABooleanTypeOfTag
boolean PersistentSortingCriteria::sortsABooleanTypeOfTag() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return sortsABooleanTypeOfTagAttribute;
}
void PersistentSortingCriteria::setSortsABooleanTypeOfTag(boolean sortsABooleanTypeOfTag)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, sortsABooleanTypeOfTagAttribute, sortsABooleanTypeOfTag);
}



// -- tagTypeBeingSorted
integer32 PersistentSortingCriteria::tagTypeBeingSorted() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return tagTypeBeingSortedAttribute;
}
void PersistentSortingCriteria::setTagTypeBeingSorted(integer32 tagTypeBeingSorted)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, tagTypeBeingSortedAttribute, tagTypeBeingSorted);
}



// -- to-one relationship: parentPlaylist
RBSchema::ObjectID PersistentSortingCriteria::parentPlaylistObjectID() const
{
    return parentPlaylistId;
}
std::shared_ptr<PersistentPlaylist> PersistentSortingCriteria::parentPlaylist() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!parentPlaylistId.isValid()) {
        return {};
    }
    auto fetchedObject = lockedContext->fetchObject<PersistentPlaylist>(parentPlaylistId);
    if (!fetchedObject) {
        return {};
    }
    return *fetchedObject;
}
void PersistentSortingCriteria::setParentPlaylist(std::shared_ptr<PersistentPlaylist> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentPlaylist>(this->parentPlaylistId);
    if (fetchedObject) {
        (*fetchedObject)->manySortedBy->removeObjectWithID(objectId);
    }
    if (!inverse) {
        parentPlaylistId = {};
    }
    else {
        parentPlaylistId = inverse->objectId;
        inverse->manySortedBy->append(sharedFromThis<PersistentSortingCriteria>());
    }
}



// -- to-many relationship: sortingPlaylists
Array<ObjectID> PersistentSortingCriteria::sortingPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->objectIDs();
}
Set<ObjectID> PersistentSortingCriteria::sortingPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentSortingCriteria::manySortingPlaylists, sortingPlaylists, SortingPlaylist, Unordered, ToMany, Nullify, criteria, Unordered, ToOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manySortingPlaylists->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentSortingCriteria::loadSortingPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySortingPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentSortingCriteria::manySortingPlaylists, sortingPlaylists, SortingPlaylist, Unordered, ToMany, Nullify, criteria, Unordered, ToOne));
    }
    if (!manySortingPlaylists->internalVector) {
        return 0;
    }
    return manySortingPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentSortingCriteria::objectIDForSortingPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySortingPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentSortingCriteria::sortingPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentSortingCriteria::sortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySortingPlaylists;
}
void PersistentSortingCriteria::orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objects), to);
}
void PersistentSortingCriteria::orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(objectids), to);
}
void PersistentSortingCriteria::orderSortingPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySortingPlaylists->rearrange(std::move(indicies), to);
}
void PersistentSortingCriteria::addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    auto fetchedObject = lockedContext->fetchObject<PersistentSortingCriteria>(inverse->criteriaId);
    if (fetchedObject) {
        (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
    }
    inverse->criteriaId = objectId;
    manySortingPlaylists->append(inverse);
}
void PersistentSortingCriteria::removeAllSortingPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySortingPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->criteriaId = {};
    }
    manySortingPlaylists->removeAll();
}
void PersistentSortingCriteria::removeSortingPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySortingPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->criteriaId = {};
}
void PersistentSortingCriteria::removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySortingPlaylists->removeObjectWithID(inverse->objectId);
    inverse->criteriaId = {};
}
