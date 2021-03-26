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

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackTag.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackTag.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTrackTag;

template class std::shared_ptr<PersistentTrackTag>;

template class NxA::Array<std::shared_ptr<PersistentTrackTag>>;
void PersistentTrackTag::bind(SourceBinder& parentBinder)
{
    PersistentStringProperty::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::TrackTag,
                      RBSchema::Type::StringProperty,
                      RELATION(PersistentTrackTag::manyChildrenTags, childrenTags, TrackTag, Unordered, ToMany, Nullify, maybeParentTag, Unordered, ToOptionalOne),
                      RELATION(PersistentTrackTag::maybeParentTagOptionalId, maybeParentTag, TrackTag, Unordered, ToOptionalOne, Nullify, childrenTags, Unordered, ToMany));
}
void PersistentTrackTag::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentStringProperty::faultObject();
    manyChildrenTags->faultRelationship();
    
}
void PersistentTrackTag::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentStringProperty::deleteObject();
    for (auto && inverse : *manyChildrenTags) {
        inverse->maybeParentTagOptionalId = {};
    }
    manyChildrenTags->removeAll();
    setMaybeParentTag({});
}



// -- to-many relationship: childrenTags
Array<ObjectID> PersistentTrackTag::childrenTagsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyChildrenTags->objectIDs();
}
Set<ObjectID> PersistentTrackTag::childrenTagsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyChildrenTags->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrackTag::manyChildrenTags, childrenTags, TrackTag, Unordered, ToMany, Nullify, maybeParentTag, Unordered, ToOptionalOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyChildrenTags->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrackTag::loadChildrenTags(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyChildrenTags->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrackTag::manyChildrenTags, childrenTags, TrackTag, Unordered, ToMany, Nullify, maybeParentTag, Unordered, ToOptionalOne));
    }
    if (!manyChildrenTags->internalVector) {
        return 0;
    }
    return manyChildrenTags->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrackTag::objectIDForChildrenTagsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyChildrenTags->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrackTag>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrackTag::childrenTagsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyChildrenTags;
}
PersistentRelationship<std::shared_ptr<PersistentTrackTag>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrackTag::childrenTags()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyChildrenTags;
}
void PersistentTrackTag::orderChildrenTagsItems(const Array<std::shared_ptr<PersistentTrackTag>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyChildrenTags->rearrange(std::move(objects), to);
}
void PersistentTrackTag::orderChildrenTagsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyChildrenTags->rearrange(std::move(objectids), to);
}
void PersistentTrackTag::orderChildrenTagsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyChildrenTags->rearrange(std::move(indicies), to);
}
void PersistentTrackTag::addChildrenTagsItem(const std::shared_ptr<PersistentTrackTag>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeParentTagOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTrackTag>((*inverse->maybeParentTagOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manyChildrenTags->removeObjectWithID(objectId);
        }
    }
    inverse->maybeParentTagOptionalId = objectId;
    manyChildrenTags->append(inverse);
}
void PersistentTrackTag::removeAllChildrenTags()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyChildrenTags) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeParentTagOptionalId = {};
    }
    manyChildrenTags->removeAll();
}
void PersistentTrackTag::removeChildrenTagsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyChildrenTags)[index];
    lockedContext->updateObject(inverse->objectId);
    manyChildrenTags->removeObjectWithID(inverse->objectId);
    inverse->maybeParentTagOptionalId = {};
}
void PersistentTrackTag::removeChildrenTagsItem(const std::shared_ptr<PersistentTrackTag>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyChildrenTags->removeObjectWithID(inverse->objectId);
    inverse->maybeParentTagOptionalId = {};
}



// -- to-one relationship: maybeParentTag
Optional<RBSchema::ObjectID> PersistentTrackTag::maybeParentTagObjectID() const
{
    return maybeParentTagOptionalId;
}
Optional<std::shared_ptr<PersistentTrackTag>> PersistentTrackTag::maybeParentTag() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeParentTagOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentTrackTag>(*maybeParentTagOptionalId)};
}
void PersistentTrackTag::setMaybeParentTag(Optional<std::shared_ptr<PersistentTrackTag>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeParentTagOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTrackTag>((*this->maybeParentTagOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manyChildrenTags->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeParentTagOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeParentTagOptionalId = derel->objectId;
        derel->manyChildrenTags->append(sharedFromThis<PersistentTrackTag>());
    }
}


