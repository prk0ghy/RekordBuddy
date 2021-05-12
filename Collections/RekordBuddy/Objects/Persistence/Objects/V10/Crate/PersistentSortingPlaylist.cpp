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

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentFilterPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/SortingCriteria/PersistentSortingCriteria.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentDecimalProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentIntegerProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TimeProperty/PersistentTimeProperty.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentSortingPlaylist;

template class NxA::Array<std::shared_ptr<PersistentSortingPlaylist>>;
void PersistentSortingPlaylist::bind(SourceBinder& parentBinder)
{
    PersistentFilterPlaylist::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::SortingPlaylist,
                      RBSchema::Type::FilterPlaylist,
                      RELATION(PersistentSortingPlaylist::criteriaId, criteria, SortingCriteria, Unordered, ToOne, Nullify, sortingPlaylists, Unordered, ToMany),
                      RELATION(PersistentSortingPlaylist::maybeDecimalPropertyUsedToSortByOptionalId,
                               maybeDecimalPropertyUsedToSortBy,
                               DecimalProperty,
                               Unordered,
                               ToOptionalOne,
                               Nullify,
                               sortingPlaylists,
                               Unordered,
                               ToMany),
                      RELATION(PersistentSortingPlaylist::maybeIntegerPropertyUsedToSortByOptionalId,
                               maybeIntegerPropertyUsedToSortBy,
                               IntegerProperty,
                               Unordered,
                               ToOptionalOne,
                               Nullify,
                               sortingPlaylists,
                               Unordered,
                               ToMany),
                      RELATION(PersistentSortingPlaylist::maybeStringPropertyUsedToSortByOptionalId,
                               maybeStringPropertyUsedToSortBy,
                               StringProperty,
                               Unordered,
                               ToOptionalOne,
                               Nullify,
                               sortingPlaylists,
                               Unordered,
                               ToMany),
                      RELATION(PersistentSortingPlaylist::maybeTimePropertyUsedToSortByOptionalId,
                               maybeTimePropertyUsedToSortBy,
                               TimeProperty,
                               Unordered,
                               ToOptionalOne,
                               Nullify,
                               sortingPlaylists,
                               Unordered,
                               ToMany));
}
void PersistentSortingPlaylist::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentFilterPlaylist::faultObject();





}
void PersistentSortingPlaylist::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentFilterPlaylist::deleteObject();
    setCriteria({});
    setMaybeDecimalPropertyUsedToSortBy({});
    setMaybeIntegerPropertyUsedToSortBy({});
    setMaybeStringPropertyUsedToSortBy({});
    setMaybeTimePropertyUsedToSortBy({});
}



// -- to-one relationship: criteria
RBSchema::ObjectID PersistentSortingPlaylist::criteriaObjectID() const
{
    return criteriaId;
}
std::shared_ptr<PersistentSortingCriteria> PersistentSortingPlaylist::criteria() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!criteriaId.isValid()) {
        return {};
    }
    auto fetchedObject = lockedContext->fetchObject<PersistentSortingCriteria>(criteriaId);
    if (!fetchedObject) {
        return {};
    }
    return *fetchedObject;
}
void PersistentSortingPlaylist::setCriteria(std::shared_ptr<PersistentSortingCriteria> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentSortingCriteria>(this->criteriaId);
    if (fetchedObject) {
        (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
    }
    if (!inverse) {
        criteriaId = {};
    }
    else {
        criteriaId = inverse->objectId;
        inverse->manySortingPlaylists->append(sharedFromThis<PersistentSortingPlaylist>());
    }
}



// -- to-one relationship: maybeDecimalPropertyUsedToSortBy
Optional<RBSchema::ObjectID> PersistentSortingPlaylist::maybeDecimalPropertyUsedToSortByObjectID() const
{
    return maybeDecimalPropertyUsedToSortByOptionalId;
}
Optional<std::shared_ptr<PersistentDecimalProperty>> PersistentSortingPlaylist::maybeDecimalPropertyUsedToSortBy() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeDecimalPropertyUsedToSortByOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentDecimalProperty>(*maybeDecimalPropertyUsedToSortByOptionalId)};
}
void PersistentSortingPlaylist::setMaybeDecimalPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentDecimalProperty>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeDecimalPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentDecimalProperty>((*this->maybeDecimalPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeDecimalPropertyUsedToSortByOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeDecimalPropertyUsedToSortByOptionalId = derel->objectId;
        derel->manySortingPlaylists->append(sharedFromThis<PersistentSortingPlaylist>());
    }
}



// -- to-one relationship: maybeIntegerPropertyUsedToSortBy
Optional<RBSchema::ObjectID> PersistentSortingPlaylist::maybeIntegerPropertyUsedToSortByObjectID() const
{
    return maybeIntegerPropertyUsedToSortByOptionalId;
}
Optional<std::shared_ptr<PersistentIntegerProperty>> PersistentSortingPlaylist::maybeIntegerPropertyUsedToSortBy() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeIntegerPropertyUsedToSortByOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentIntegerProperty>(*maybeIntegerPropertyUsedToSortByOptionalId)};
}
void PersistentSortingPlaylist::setMaybeIntegerPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentIntegerProperty>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeIntegerPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentIntegerProperty>((*this->maybeIntegerPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeIntegerPropertyUsedToSortByOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeIntegerPropertyUsedToSortByOptionalId = derel->objectId;
        derel->manySortingPlaylists->append(sharedFromThis<PersistentSortingPlaylist>());
    }
}



// -- to-one relationship: maybeStringPropertyUsedToSortBy
Optional<RBSchema::ObjectID> PersistentSortingPlaylist::maybeStringPropertyUsedToSortByObjectID() const
{
    return maybeStringPropertyUsedToSortByOptionalId;
}
Optional<std::shared_ptr<PersistentStringProperty>> PersistentSortingPlaylist::maybeStringPropertyUsedToSortBy() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeStringPropertyUsedToSortByOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentStringProperty>(*maybeStringPropertyUsedToSortByOptionalId)};
}
void PersistentSortingPlaylist::setMaybeStringPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentStringProperty>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeStringPropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentStringProperty>((*this->maybeStringPropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeStringPropertyUsedToSortByOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeStringPropertyUsedToSortByOptionalId = derel->objectId;
        derel->manySortingPlaylists->append(sharedFromThis<PersistentSortingPlaylist>());
    }
}



// -- to-one relationship: maybeTimePropertyUsedToSortBy
Optional<RBSchema::ObjectID> PersistentSortingPlaylist::maybeTimePropertyUsedToSortByObjectID() const
{
    return maybeTimePropertyUsedToSortByOptionalId;
}
Optional<std::shared_ptr<PersistentTimeProperty>> PersistentSortingPlaylist::maybeTimePropertyUsedToSortBy() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeTimePropertyUsedToSortByOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentTimeProperty>(*maybeTimePropertyUsedToSortByOptionalId)};
}
void PersistentSortingPlaylist::setMaybeTimePropertyUsedToSortBy(Optional<std::shared_ptr<PersistentTimeProperty>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeTimePropertyUsedToSortByOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTimeProperty>((*this->maybeTimePropertyUsedToSortByOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySortingPlaylists->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeTimePropertyUsedToSortByOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeTimePropertyUsedToSortByOptionalId = derel->objectId;
        derel->manySortingPlaylists->append(sharedFromThis<PersistentSortingPlaylist>());
    }
}
