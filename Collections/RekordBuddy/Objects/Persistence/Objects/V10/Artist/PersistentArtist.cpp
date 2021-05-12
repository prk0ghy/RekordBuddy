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

#include <RekordBuddyCollectionImplementationPersistence/V10/Artist/PersistentArtist.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentArtist;

template class NxA::Array<std::shared_ptr<PersistentArtist>>;

void PersistentArtist::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::Artist,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentArtist::manyTracksCreditedAsArtist, tracksCreditedAsArtist, Track, Unordered, ToMany, Nullify, artists, Ordered, ToMany),
                      RELATION(PersistentArtist::manyTracksCreditedAsProducer, tracksCreditedAsProducer, Track, Unordered, ToMany, Nullify, producers, Ordered, ToMany),
                      RELATION(PersistentArtist::manyTracksCreditedAsRemixer, tracksCreditedAsRemixer, Track, Unordered, ToMany, Nullify, remixers, Ordered, ToMany),
                      ATTRIBUTE(PersistentArtist::nameAttribute, name, Unindexed));
}
void PersistentArtist::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyTracksCreditedAsArtist->faultRelationship();
    manyTracksCreditedAsProducer->faultRelationship();
    manyTracksCreditedAsRemixer->faultRelationship();
}
void PersistentArtist::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyTracksCreditedAsArtist) {
        inverse->manyArtists->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsArtist->removeAll();
    for (auto && inverse : *manyTracksCreditedAsProducer) {
        inverse->manyProducers->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsProducer->removeAll();
    for (auto && inverse : *manyTracksCreditedAsRemixer) {
        inverse->manyRemixers->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsRemixer->removeAll();
}

// -- name
String PersistentArtist::name() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return nameAttribute;
}
void PersistentArtist::setName(String name)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, nameAttribute, name);
}



// -- to-many relationship: tracksCreditedAsArtist
Array<ObjectID> PersistentArtist::tracksCreditedAsArtistObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsArtist->objectIDs();
}
Set<ObjectID> PersistentArtist::tracksCreditedAsArtistObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyTracksCreditedAsArtist->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentArtist::manyTracksCreditedAsArtist, tracksCreditedAsArtist, Track, Unordered, ToMany, Nullify, artists, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyTracksCreditedAsArtist->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentArtist::loadTracksCreditedAsArtist(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyTracksCreditedAsArtist->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentArtist::manyTracksCreditedAsArtist, tracksCreditedAsArtist, Track, Unordered, ToMany, Nullify, artists, Ordered, ToMany));
    }
    if (!manyTracksCreditedAsArtist->internalVector) {
        return 0;
    }
    return manyTracksCreditedAsArtist->internalVector->size();
}
typename RBSchema::ObjectID PersistentArtist::objectIDForTracksCreditedAsArtistItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsArtist->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentArtist::tracksCreditedAsArtistConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsArtist;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentArtist::tracksCreditedAsArtist()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsArtist;
}
void PersistentArtist::orderTracksCreditedAsArtistItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsArtist->rearrange(std::move(objects), to);
}
void PersistentArtist::orderTracksCreditedAsArtistItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsArtist->rearrange(std::move(objectids), to);
}
void PersistentArtist::orderTracksCreditedAsArtistItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsArtist->rearrange(std::move(indicies), to);
}
void PersistentArtist::addTracksCreditedAsArtistItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyArtists->append(*lockedContext->fetchObject<PersistentArtist>(objectId));
    manyTracksCreditedAsArtist->append(inverse);
}
void PersistentArtist::removeAllTracksCreditedAsArtist()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyTracksCreditedAsArtist) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyArtists->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsArtist->removeAll();
}
void PersistentArtist::removeTracksCreditedAsArtistItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyTracksCreditedAsArtist)[index];
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsArtist->removeObjectWithID(inverse->objectId);
    inverse->manyArtists->removeObjectWithID(objectId);
}
void PersistentArtist::removeTracksCreditedAsArtistItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsArtist->removeObjectWithID(inverse->objectId);
    inverse->manyArtists->removeObjectWithID(objectId);
}



// -- to-many relationship: tracksCreditedAsProducer
Array<ObjectID> PersistentArtist::tracksCreditedAsProducerObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsProducer->objectIDs();
}
Set<ObjectID> PersistentArtist::tracksCreditedAsProducerObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyTracksCreditedAsProducer->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentArtist::manyTracksCreditedAsProducer, tracksCreditedAsProducer, Track, Unordered, ToMany, Nullify, producers, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyTracksCreditedAsProducer->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentArtist::loadTracksCreditedAsProducer(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyTracksCreditedAsProducer->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentArtist::manyTracksCreditedAsProducer, tracksCreditedAsProducer, Track, Unordered, ToMany, Nullify, producers, Ordered, ToMany));
    }
    if (!manyTracksCreditedAsProducer->internalVector) {
        return 0;
    }
    return manyTracksCreditedAsProducer->internalVector->size();
}
typename RBSchema::ObjectID PersistentArtist::objectIDForTracksCreditedAsProducerItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsProducer->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentArtist::tracksCreditedAsProducerConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsProducer;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentArtist::tracksCreditedAsProducer()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsProducer;
}
void PersistentArtist::orderTracksCreditedAsProducerItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsProducer->rearrange(std::move(objects), to);
}
void PersistentArtist::orderTracksCreditedAsProducerItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsProducer->rearrange(std::move(objectids), to);
}
void PersistentArtist::orderTracksCreditedAsProducerItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsProducer->rearrange(std::move(indicies), to);
}
void PersistentArtist::addTracksCreditedAsProducerItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyProducers->append(*lockedContext->fetchObject<PersistentArtist>(objectId));
    manyTracksCreditedAsProducer->append(inverse);
}
void PersistentArtist::removeAllTracksCreditedAsProducer()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyTracksCreditedAsProducer) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyProducers->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsProducer->removeAll();
}
void PersistentArtist::removeTracksCreditedAsProducerItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyTracksCreditedAsProducer)[index];
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsProducer->removeObjectWithID(inverse->objectId);
    inverse->manyProducers->removeObjectWithID(objectId);
}
void PersistentArtist::removeTracksCreditedAsProducerItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsProducer->removeObjectWithID(inverse->objectId);
    inverse->manyProducers->removeObjectWithID(objectId);
}



// -- to-many relationship: tracksCreditedAsRemixer
Array<ObjectID> PersistentArtist::tracksCreditedAsRemixerObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsRemixer->objectIDs();
}
Set<ObjectID> PersistentArtist::tracksCreditedAsRemixerObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyTracksCreditedAsRemixer->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentArtist::manyTracksCreditedAsRemixer, tracksCreditedAsRemixer, Track, Unordered, ToMany, Nullify, remixers, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyTracksCreditedAsRemixer->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentArtist::loadTracksCreditedAsRemixer(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyTracksCreditedAsRemixer->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentArtist::manyTracksCreditedAsRemixer, tracksCreditedAsRemixer, Track, Unordered, ToMany, Nullify, remixers, Ordered, ToMany));
    }
    if (!manyTracksCreditedAsRemixer->internalVector) {
        return 0;
    }
    return manyTracksCreditedAsRemixer->internalVector->size();
}
typename RBSchema::ObjectID PersistentArtist::objectIDForTracksCreditedAsRemixerItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTracksCreditedAsRemixer->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentArtist::tracksCreditedAsRemixerConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsRemixer;
}
PersistentRelationship<std::shared_ptr<PersistentTrack>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentArtist::tracksCreditedAsRemixer()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTracksCreditedAsRemixer;
}
void PersistentArtist::orderTracksCreditedAsRemixerItems(const Array<std::shared_ptr<PersistentTrack>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsRemixer->rearrange(std::move(objects), to);
}
void PersistentArtist::orderTracksCreditedAsRemixerItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsRemixer->rearrange(std::move(objectids), to);
}
void PersistentArtist::orderTracksCreditedAsRemixerItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTracksCreditedAsRemixer->rearrange(std::move(indicies), to);
}
void PersistentArtist::addTracksCreditedAsRemixerItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyRemixers->append(*lockedContext->fetchObject<PersistentArtist>(objectId));
    manyTracksCreditedAsRemixer->append(inverse);
}
void PersistentArtist::removeAllTracksCreditedAsRemixer()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyTracksCreditedAsRemixer) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyRemixers->removeObjectWithID(objectId);
    }
    manyTracksCreditedAsRemixer->removeAll();
}
void PersistentArtist::removeTracksCreditedAsRemixerItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyTracksCreditedAsRemixer)[index];
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsRemixer->removeObjectWithID(inverse->objectId);
    inverse->manyRemixers->removeObjectWithID(objectId);
}
void PersistentArtist::removeTracksCreditedAsRemixerItem(const std::shared_ptr<PersistentTrack>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyTracksCreditedAsRemixer->removeObjectWithID(inverse->objectId);
    inverse->manyRemixers->removeObjectWithID(objectId);
}
