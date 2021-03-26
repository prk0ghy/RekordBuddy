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

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrateClone.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentCrate;

template class std::shared_ptr<PersistentCrate>;

template class NxA::Array<std::shared_ptr<PersistentCrate>>;
void PersistentCrate::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::Crate,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentCrate::manyClones, clones, CrateClone, Unordered, ToMany, Nullify, cloneOf, Unordered, ToOne),
                      RELATION(PersistentCrate::maybeParentCrateOptionalId, maybeParentCrate, Crate, Unordered, ToOptionalOne, Nullify, subCrates, Ordered, ToMany),
                      RELATION(PersistentCrate::manySubCrates, subCrates, Crate, Ordered, ToMany, Nullify, maybeParentCrate, Unordered, ToOptionalOne),
                      ATTRIBUTE(PersistentCrate::lastModifiedOnAttribute, lastModifiedOn, Unindexed),
                      ATTRIBUTE(PersistentCrate::nameAttribute, name, Unindexed));
}
void PersistentCrate::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyClones->faultRelationship();
    
    manySubCrates->faultRelationship();
}
void PersistentCrate::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyClones) {
        inverse->cloneOfId = {};
    }
    manyClones->removeAll();
    setMaybeParentCrate({});
    for (auto && inverse : *manySubCrates) {
        inverse->maybeParentCrateOptionalId = {};
    }
    manySubCrates->removeAll();
}

// -- lastModifiedOn
Time PersistentCrate::lastModifiedOn() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return lastModifiedOnAttribute;
}
void PersistentCrate::setLastModifiedOn(Time lastModifiedOn)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, lastModifiedOnAttribute, lastModifiedOn);
}



// -- name
String PersistentCrate::name() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return nameAttribute;
}
void PersistentCrate::setName(String name)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, nameAttribute, name);
}



// -- to-many relationship: clones
Array<ObjectID> PersistentCrate::clonesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyClones->objectIDs();
}
Set<ObjectID> PersistentCrate::clonesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyClones->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type, RELATION(PersistentCrate::manyClones, clones, CrateClone, Unordered, ToMany, Nullify, cloneOf, Unordered, ToOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyClones->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentCrate::loadClones(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyClones->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentCrate::manyClones, clones, CrateClone, Unordered, ToMany, Nullify, cloneOf, Unordered, ToOne));
    }
    if (!manyClones->internalVector) {
        return 0;
    }
    return manyClones->internalVector->size();
}
typename RBSchema::ObjectID PersistentCrate::objectIDForClonesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyClones->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentCrateClone>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentCrate::clonesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyClones;
}
PersistentRelationship<std::shared_ptr<PersistentCrateClone>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentCrate::clones()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyClones;
}
void PersistentCrate::orderClonesItems(const Array<std::shared_ptr<PersistentCrateClone>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyClones->rearrange(std::move(objects), to);
}
void PersistentCrate::orderClonesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyClones->rearrange(std::move(objectids), to);
}
void PersistentCrate::orderClonesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyClones->rearrange(std::move(indicies), to);
}
void PersistentCrate::addClonesItem(const std::shared_ptr<PersistentCrateClone>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    auto fetchedObject = lockedContext->fetchObject<PersistentCrate>(inverse->cloneOfId);
    if (fetchedObject) {
        (*fetchedObject)->manyClones->removeObjectWithID(objectId);
    }
    inverse->cloneOfId = objectId;
    manyClones->append(inverse);
}
void PersistentCrate::removeAllClones()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyClones) {
        lockedContext->updateObject(inverse->objectId);
        inverse->cloneOfId = {};
    }
    manyClones->removeAll();
}
void PersistentCrate::removeClonesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyClones)[index];
    lockedContext->updateObject(inverse->objectId);
    manyClones->removeObjectWithID(inverse->objectId);
    inverse->cloneOfId = {};
}
void PersistentCrate::removeClonesItem(const std::shared_ptr<PersistentCrateClone>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyClones->removeObjectWithID(inverse->objectId);
    inverse->cloneOfId = {};
}



// -- to-one relationship: maybeParentCrate
Optional<RBSchema::ObjectID> PersistentCrate::maybeParentCrateObjectID() const
{
    return maybeParentCrateOptionalId;
}
Optional<std::shared_ptr<PersistentCrate>> PersistentCrate::maybeParentCrate() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeParentCrateOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentCrate>(*maybeParentCrateOptionalId)};
}
void PersistentCrate::setMaybeParentCrate(Optional<std::shared_ptr<PersistentCrate>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeParentCrateOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentCrate>((*this->maybeParentCrateOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySubCrates->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeParentCrateOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeParentCrateOptionalId = derel->objectId;
        derel->manySubCrates->append(sharedFromThis<PersistentCrate>());
    }
}



// -- to-many relationship: subCrates
Array<ObjectID> PersistentCrate::subCratesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySubCrates->objectIDs();
}
Set<ObjectID> PersistentCrate::subCratesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manySubCrates->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentCrate::manySubCrates, subCrates, Crate, Ordered, ToMany, Nullify, maybeParentCrate, Unordered, ToOptionalOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manySubCrates->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentCrate::loadSubCrates(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manySubCrates->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentCrate::manySubCrates, subCrates, Crate, Ordered, ToMany, Nullify, maybeParentCrate, Unordered, ToOptionalOne));
    }
    if (!manySubCrates->internalVector) {
        return 0;
    }
    return manySubCrates->internalVector->size();
}
typename RBSchema::ObjectID PersistentCrate::objectIDForSubCratesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manySubCrates->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentCrate>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentCrate::subCratesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySubCrates;
}
PersistentRelationship<std::shared_ptr<PersistentCrate>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentCrate::subCrates()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manySubCrates;
}
void PersistentCrate::orderSubCratesItems(const Array<std::shared_ptr<PersistentCrate>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySubCrates->rearrange(std::move(objects), to);
}
void PersistentCrate::orderSubCratesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySubCrates->rearrange(std::move(objectids), to);
}
void PersistentCrate::orderSubCratesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manySubCrates->rearrange(std::move(indicies), to);
}
void PersistentCrate::addSubCratesItem(const std::shared_ptr<PersistentCrate>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeParentCrateOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentCrate>((*inverse->maybeParentCrateOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manySubCrates->removeObjectWithID(objectId);
        }
    }
    inverse->maybeParentCrateOptionalId = objectId;
    manySubCrates->append(inverse);
}
void PersistentCrate::removeAllSubCrates()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manySubCrates) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeParentCrateOptionalId = {};
    }
    manySubCrates->removeAll();
}
void PersistentCrate::removeSubCratesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manySubCrates)[index];
    lockedContext->updateObject(inverse->objectId);
    manySubCrates->removeObjectWithID(inverse->objectId);
    inverse->maybeParentCrateOptionalId = {};
}
void PersistentCrate::removeSubCratesItem(const std::shared_ptr<PersistentCrate>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manySubCrates->removeObjectWithID(inverse->objectId);
    inverse->maybeParentCrateOptionalId = {};
}


