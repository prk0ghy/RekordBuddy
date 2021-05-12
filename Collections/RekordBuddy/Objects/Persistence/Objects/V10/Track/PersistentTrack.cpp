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

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Artist/PersistentArtist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentDecimalProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TrackDisplayCache/PersistentTrackDisplayCache.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentIntegerProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/LogEntry/PersistentLogEntry.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/MarkerImportOffset/PersistentMarkerImportOffset.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TimeProperty/PersistentTimeProperty.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTrack;

template class NxA::Array<std::shared_ptr<PersistentTrack>>;
void PersistentTrack::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::Track,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentTrack::manyArtists, artists, Artist, Ordered, ToMany, Nullify, tracksCreditedAsArtist, Unordered, ToMany),
                      RELATION(PersistentTrack::manyDecimalProperties, decimalProperties, DecimalProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany),
                      RELATION(PersistentTrack::displayCacheId, displayCache, TrackDisplayCache, Unordered, ToOne, Nullify, parentTrack, Unordered, ToOne),
                      RELATION(PersistentTrack::manyIntegerProperties, integerProperties, IntegerProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany),
                      RELATION(PersistentTrack::manyLogEntries, logEntries, LogEntry, Unordered, ToMany, Nullify, maybeRelatedToTrack, Unordered, ToOptionalOne),
                      RELATION(PersistentTrack::manyMarkerImportOffsets, markerImportOffsets, MarkerImportOffset, Unordered, ToMany, Nullify, parentTrack, Unordered, ToOne),
                      RELATION(PersistentTrack::manyMarkers, markers, Marker, Ordered, ToMany, Nullify, parentTrack, Unordered, ToOne),
                      RELATION(PersistentTrack::manyParentPlaylists, parentPlaylists, Playlist, Unordered, ToMany, Nullify, tracks, Ordered, ToMany),
                      RELATION(PersistentTrack::manyProducers, producers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsProducer, Unordered, ToMany),
                      RELATION(PersistentTrack::manyRemixers, remixers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsRemixer, Unordered, ToMany),
                      RELATION(PersistentTrack::manyStringProperties, stringProperties, StringProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany),
                      RELATION(PersistentTrack::manyTimeProperties, timeProperties, TimeProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany),
                      ATTRIBUTE(PersistentTrack::filePathRelativeToCollectionVolumeAttribute, filePathRelativeToCollectionVolume, Unindexed),
                      ATTRIBUTE(PersistentTrack::maybeMacOSBookmarkAttribute, maybeMacOSBookmark, Unindexed));
}
void PersistentTrack::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    manyArtists->faultRelationship();
    manyDecimalProperties->faultRelationship();

    manyIntegerProperties->faultRelationship();
    manyLogEntries->faultRelationship();
    manyMarkerImportOffsets->faultRelationship();
    manyMarkers->faultRelationship();
    manyParentPlaylists->faultRelationship();
    manyProducers->faultRelationship();
    manyRemixers->faultRelationship();
    manyStringProperties->faultRelationship();
    manyTimeProperties->faultRelationship();
}
void PersistentTrack::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    for (auto && inverse : *manyArtists) {
        inverse->manyTracksCreditedAsArtist->removeObjectWithID(objectId);
    }
    manyArtists->removeAll();
    for (auto && inverse : *manyDecimalProperties) {
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyDecimalProperties->removeAll();
    setDisplayCache({});
    for (auto && inverse : *manyIntegerProperties) {
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyIntegerProperties->removeAll();
    for (auto && inverse : *manyLogEntries) {
        inverse->maybeRelatedToTrackOptionalId = {};
    }
    manyLogEntries->removeAll();
    for (auto && inverse : *manyMarkerImportOffsets) {
        inverse->parentTrackId = {};
    }
    manyMarkerImportOffsets->removeAll();
    for (auto && inverse : *manyMarkers) {
        inverse->parentTrackId = {};
    }
    manyMarkers->removeAll();
    for (auto && inverse : *manyParentPlaylists) {
        inverse->manyTracks->removeObjectWithID(objectId);
    }
    manyParentPlaylists->removeAll();
    for (auto && inverse : *manyProducers) {
        inverse->manyTracksCreditedAsProducer->removeObjectWithID(objectId);
    }
    manyProducers->removeAll();
    for (auto && inverse : *manyRemixers) {
        inverse->manyTracksCreditedAsRemixer->removeObjectWithID(objectId);
    }
    manyRemixers->removeAll();
    for (auto && inverse : *manyStringProperties) {
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyStringProperties->removeAll();
    for (auto && inverse : *manyTimeProperties) {
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyTimeProperties->removeAll();
}

// -- filePathRelativeToCollectionVolume
String PersistentTrack::filePathRelativeToCollectionVolume() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return filePathRelativeToCollectionVolumeAttribute;
}
void PersistentTrack::setFilePathRelativeToCollectionVolume(String filePathRelativeToCollectionVolume)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, filePathRelativeToCollectionVolumeAttribute, filePathRelativeToCollectionVolume);
}



// -- maybeMacOSBookmark
Optional<Blob> PersistentTrack::maybeMacOSBookmark() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeMacOSBookmarkAttribute;
}
void PersistentTrack::setMaybeMacOSBookmark(Optional<Blob> maybeMacOSBookmark)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeMacOSBookmarkAttribute, maybeMacOSBookmark);
}



// -- to-many relationship: artists
Array<ObjectID> PersistentTrack::artistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyArtists->objectIDs();
}
Set<ObjectID> PersistentTrack::artistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyArtists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyArtists, artists, Artist, Ordered, ToMany, Nullify, tracksCreditedAsArtist, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyArtists->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadArtists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyArtists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentTrack::manyArtists, artists, Artist, Ordered, ToMany, Nullify, tracksCreditedAsArtist, Unordered, ToMany));
    }
    if (!manyArtists->internalVector) {
        return 0;
    }
    return manyArtists->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForArtistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyArtists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentArtist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::artistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyArtists;
}
PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::artists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyArtists;
}
void PersistentTrack::orderArtistsItems(const Array<std::shared_ptr<PersistentArtist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyArtists->rearrange(std::move(objects), to);
}
void PersistentTrack::orderArtistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyArtists->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderArtistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyArtists->rearrange(std::move(indicies), to);
}
void PersistentTrack::addArtistsItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyTracksCreditedAsArtist->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyArtists->append(inverse);
}
void PersistentTrack::removeAllArtists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyArtists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyTracksCreditedAsArtist->removeObjectWithID(objectId);
    }
    manyArtists->removeAll();
}
void PersistentTrack::removeArtistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyArtists)[index];
    lockedContext->updateObject(inverse->objectId);
    manyArtists->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsArtist->removeObjectWithID(objectId);
}
void PersistentTrack::removeArtistsItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyArtists->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsArtist->removeObjectWithID(objectId);
}



// -- to-many relationship: decimalProperties
Array<ObjectID> PersistentTrack::decimalPropertiesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyDecimalProperties->objectIDs();
}
Set<ObjectID> PersistentTrack::decimalPropertiesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyDecimalProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyDecimalProperties, decimalProperties, DecimalProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyDecimalProperties->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadDecimalProperties(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyDecimalProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyDecimalProperties, decimalProperties, DecimalProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    if (!manyDecimalProperties->internalVector) {
        return 0;
    }
    return manyDecimalProperties->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForDecimalPropertiesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyDecimalProperties->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentDecimalProperty>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::decimalPropertiesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyDecimalProperties;
}
PersistentRelationship<std::shared_ptr<PersistentDecimalProperty>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentTrack::decimalProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyDecimalProperties;
}
void PersistentTrack::orderDecimalPropertiesItems(const Array<std::shared_ptr<PersistentDecimalProperty>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyDecimalProperties->rearrange(std::move(objects), to);
}
void PersistentTrack::orderDecimalPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyDecimalProperties->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderDecimalPropertiesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyDecimalProperties->rearrange(std::move(indicies), to);
}
void PersistentTrack::addDecimalPropertiesItem(const std::shared_ptr<PersistentDecimalProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyParentTracks->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyDecimalProperties->append(inverse);
}
void PersistentTrack::removeAllDecimalProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyDecimalProperties) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyDecimalProperties->removeAll();
}
void PersistentTrack::removeDecimalPropertiesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyDecimalProperties)[index];
    lockedContext->updateObject(inverse->objectId);
    manyDecimalProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}
void PersistentTrack::removeDecimalPropertiesItem(const std::shared_ptr<PersistentDecimalProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyDecimalProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}



// -- to-one relationship: displayCache
RBSchema::ObjectID PersistentTrack::displayCacheObjectID() const
{
    return displayCacheId;
}
std::shared_ptr<PersistentTrackDisplayCache> PersistentTrack::displayCache() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!displayCacheId.isValid()) {
        return {};
    }
    auto fetchedObject = lockedContext->fetchObject<PersistentTrackDisplayCache>(displayCacheId);
    if (!fetchedObject) {
        return {};
    }
    return *fetchedObject;
}
void PersistentTrack::setDisplayCache(std::shared_ptr<PersistentTrackDisplayCache> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentTrackDisplayCache>(this->displayCacheId);
    if (fetchedObject) {
        (*fetchedObject)->parentTrackId = {};
    }
    if (!inverse) {
        displayCacheId = {};
    }
    else {
        displayCacheId = inverse->objectId;
        inverse->parentTrackId = objectId;
    }
}



// -- to-many relationship: integerProperties
Array<ObjectID> PersistentTrack::integerPropertiesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyIntegerProperties->objectIDs();
}
Set<ObjectID> PersistentTrack::integerPropertiesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyIntegerProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyIntegerProperties, integerProperties, IntegerProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyIntegerProperties->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadIntegerProperties(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyIntegerProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyIntegerProperties, integerProperties, IntegerProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    if (!manyIntegerProperties->internalVector) {
        return 0;
    }
    return manyIntegerProperties->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForIntegerPropertiesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyIntegerProperties->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentIntegerProperty>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::integerPropertiesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyIntegerProperties;
}
PersistentRelationship<std::shared_ptr<PersistentIntegerProperty>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentTrack::integerProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyIntegerProperties;
}
void PersistentTrack::orderIntegerPropertiesItems(const Array<std::shared_ptr<PersistentIntegerProperty>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyIntegerProperties->rearrange(std::move(objects), to);
}
void PersistentTrack::orderIntegerPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyIntegerProperties->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderIntegerPropertiesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyIntegerProperties->rearrange(std::move(indicies), to);
}
void PersistentTrack::addIntegerPropertiesItem(const std::shared_ptr<PersistentIntegerProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyParentTracks->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyIntegerProperties->append(inverse);
}
void PersistentTrack::removeAllIntegerProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyIntegerProperties) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyIntegerProperties->removeAll();
}
void PersistentTrack::removeIntegerPropertiesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyIntegerProperties)[index];
    lockedContext->updateObject(inverse->objectId);
    manyIntegerProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}
void PersistentTrack::removeIntegerPropertiesItem(const std::shared_ptr<PersistentIntegerProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyIntegerProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}



// -- to-many relationship: logEntries
Array<ObjectID> PersistentTrack::logEntriesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyLogEntries->objectIDs();
}
Set<ObjectID> PersistentTrack::logEntriesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyLogEntries->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyLogEntries, logEntries, LogEntry, Unordered, ToMany, Nullify, maybeRelatedToTrack, Unordered, ToOptionalOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyLogEntries->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadLogEntries(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyLogEntries->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyLogEntries, logEntries, LogEntry, Unordered, ToMany, Nullify, maybeRelatedToTrack, Unordered, ToOptionalOne));
    }
    if (!manyLogEntries->internalVector) {
        return 0;
    }
    return manyLogEntries->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForLogEntriesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyLogEntries->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentLogEntry>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::logEntriesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyLogEntries;
}
PersistentRelationship<std::shared_ptr<PersistentLogEntry>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::logEntries()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyLogEntries;
}
void PersistentTrack::orderLogEntriesItems(const Array<std::shared_ptr<PersistentLogEntry>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyLogEntries->rearrange(std::move(objects), to);
}
void PersistentTrack::orderLogEntriesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyLogEntries->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderLogEntriesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyLogEntries->rearrange(std::move(indicies), to);
}
void PersistentTrack::addLogEntriesItem(const std::shared_ptr<PersistentLogEntry>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    if (inverse->maybeRelatedToTrackOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTrack>((*inverse->maybeRelatedToTrackOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manyLogEntries->removeObjectWithID(objectId);
        }
    }
    inverse->maybeRelatedToTrackOptionalId = objectId;
    manyLogEntries->append(inverse);
}
void PersistentTrack::removeAllLogEntries()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyLogEntries) {
        lockedContext->updateObject(inverse->objectId);
        inverse->maybeRelatedToTrackOptionalId = {};
    }
    manyLogEntries->removeAll();
}
void PersistentTrack::removeLogEntriesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyLogEntries)[index];
    lockedContext->updateObject(inverse->objectId);
    manyLogEntries->removeObjectWithID(inverse->objectId);
    inverse->maybeRelatedToTrackOptionalId = {};
}
void PersistentTrack::removeLogEntriesItem(const std::shared_ptr<PersistentLogEntry>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyLogEntries->removeObjectWithID(inverse->objectId);
    inverse->maybeRelatedToTrackOptionalId = {};
}



// -- to-many relationship: markerImportOffsets
Array<ObjectID> PersistentTrack::markerImportOffsetsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyMarkerImportOffsets->objectIDs();
}
Set<ObjectID> PersistentTrack::markerImportOffsetsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyMarkerImportOffsets->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyMarkerImportOffsets, markerImportOffsets, MarkerImportOffset, Unordered, ToMany, Nullify, parentTrack, Unordered, ToOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyMarkerImportOffsets->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadMarkerImportOffsets(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyMarkerImportOffsets->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyMarkerImportOffsets, markerImportOffsets, MarkerImportOffset, Unordered, ToMany, Nullify, parentTrack, Unordered, ToOne));
    }
    if (!manyMarkerImportOffsets->internalVector) {
        return 0;
    }
    return manyMarkerImportOffsets->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForMarkerImportOffsetsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyMarkerImportOffsets->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentMarkerImportOffset>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::markerImportOffsetsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyMarkerImportOffsets;
}
PersistentRelationship<std::shared_ptr<PersistentMarkerImportOffset>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentTrack::markerImportOffsets()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyMarkerImportOffsets;
}
void PersistentTrack::orderMarkerImportOffsetsItems(const Array<std::shared_ptr<PersistentMarkerImportOffset>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkerImportOffsets->rearrange(std::move(objects), to);
}
void PersistentTrack::orderMarkerImportOffsetsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkerImportOffsets->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderMarkerImportOffsetsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkerImportOffsets->rearrange(std::move(indicies), to);
}
void PersistentTrack::addMarkerImportOffsetsItem(const std::shared_ptr<PersistentMarkerImportOffset>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(inverse->parentTrackId);
    if (fetchedObject) {
        (*fetchedObject)->manyMarkerImportOffsets->removeObjectWithID(objectId);
    }
    inverse->parentTrackId = objectId;
    manyMarkerImportOffsets->append(inverse);
}
void PersistentTrack::removeAllMarkerImportOffsets()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyMarkerImportOffsets) {
        lockedContext->updateObject(inverse->objectId);
        inverse->parentTrackId = {};
    }
    manyMarkerImportOffsets->removeAll();
}
void PersistentTrack::removeMarkerImportOffsetsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyMarkerImportOffsets)[index];
    lockedContext->updateObject(inverse->objectId);
    manyMarkerImportOffsets->removeObjectWithID(inverse->objectId);
    inverse->parentTrackId = {};
}
void PersistentTrack::removeMarkerImportOffsetsItem(const std::shared_ptr<PersistentMarkerImportOffset>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyMarkerImportOffsets->removeObjectWithID(inverse->objectId);
    inverse->parentTrackId = {};
}



// -- to-many relationship: markers
Array<ObjectID> PersistentTrack::markersObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyMarkers->objectIDs();
}
Set<ObjectID> PersistentTrack::markersObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyMarkers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type, RELATION(PersistentTrack::manyMarkers, markers, Marker, Ordered, ToMany, Nullify, parentTrack, Unordered, ToOne));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyMarkers->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadMarkers(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyMarkers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentTrack::manyMarkers, markers, Marker, Ordered, ToMany, Nullify, parentTrack, Unordered, ToOne));
    }
    if (!manyMarkers->internalVector) {
        return 0;
    }
    return manyMarkers->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForMarkersItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyMarkers->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentMarker>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::markersConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyMarkers;
}
PersistentRelationship<std::shared_ptr<PersistentMarker>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::markers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyMarkers;
}
void PersistentTrack::orderMarkersItems(const Array<std::shared_ptr<PersistentMarker>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkers->rearrange(std::move(objects), to);
}
void PersistentTrack::orderMarkersItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkers->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderMarkersItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyMarkers->rearrange(std::move(indicies), to);
}
void PersistentTrack::addMarkersItem(const std::shared_ptr<PersistentMarker>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());
    auto fetchedObject = lockedContext->fetchObject<PersistentTrack>(inverse->parentTrackId);
    if (fetchedObject) {
        (*fetchedObject)->manyMarkers->removeObjectWithID(objectId);
    }
    inverse->parentTrackId = objectId;
    manyMarkers->append(inverse);
}
void PersistentTrack::removeAllMarkers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyMarkers) {
        lockedContext->updateObject(inverse->objectId);
        inverse->parentTrackId = {};
    }
    manyMarkers->removeAll();
}
void PersistentTrack::removeMarkersItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyMarkers)[index];
    lockedContext->updateObject(inverse->objectId);
    manyMarkers->removeObjectWithID(inverse->objectId);
    inverse->parentTrackId = {};
}
void PersistentTrack::removeMarkersItem(const std::shared_ptr<PersistentMarker>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyMarkers->removeObjectWithID(inverse->objectId);
    inverse->parentTrackId = {};
}



// -- to-many relationship: parentPlaylists
Array<ObjectID> PersistentTrack::parentPlaylistsObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentPlaylists->objectIDs();
}
Set<ObjectID> PersistentTrack::parentPlaylistsObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyParentPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyParentPlaylists, parentPlaylists, Playlist, Unordered, ToMany, Nullify, tracks, Ordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyParentPlaylists->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadParentPlaylists(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyParentPlaylists->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentTrack::manyParentPlaylists, parentPlaylists, Playlist, Unordered, ToMany, Nullify, tracks, Ordered, ToMany));
    }
    if (!manyParentPlaylists->internalVector) {
        return 0;
    }
    return manyParentPlaylists->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForParentPlaylistsItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyParentPlaylists->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentPlaylist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::parentPlaylistsConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentPlaylists;
}
PersistentRelationship<std::shared_ptr<PersistentPlaylist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::parentPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyParentPlaylists;
}
void PersistentTrack::orderParentPlaylistsItems(const Array<std::shared_ptr<PersistentPlaylist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentPlaylists->rearrange(std::move(objects), to);
}
void PersistentTrack::orderParentPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentPlaylists->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderParentPlaylistsItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyParentPlaylists->rearrange(std::move(indicies), to);
}
void PersistentTrack::addParentPlaylistsItem(const std::shared_ptr<PersistentPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyTracks->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyParentPlaylists->append(inverse);
}
void PersistentTrack::removeAllParentPlaylists()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyParentPlaylists) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyTracks->removeObjectWithID(objectId);
    }
    manyParentPlaylists->removeAll();
}
void PersistentTrack::removeParentPlaylistsItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyParentPlaylists)[index];
    lockedContext->updateObject(inverse->objectId);
    manyParentPlaylists->removeObjectWithID(inverse->objectId);
    inverse->manyTracks->removeObjectWithID(objectId);
}
void PersistentTrack::removeParentPlaylistsItem(const std::shared_ptr<PersistentPlaylist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyParentPlaylists->removeObjectWithID(inverse->objectId);
    inverse->manyTracks->removeObjectWithID(objectId);
}



// -- to-many relationship: producers
Array<ObjectID> PersistentTrack::producersObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyProducers->objectIDs();
}
Set<ObjectID> PersistentTrack::producersObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyProducers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyProducers, producers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsProducer, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyProducers->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadProducers(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyProducers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyProducers, producers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsProducer, Unordered, ToMany));
    }
    if (!manyProducers->internalVector) {
        return 0;
    }
    return manyProducers->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForProducersItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyProducers->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentArtist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::producersConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyProducers;
}
PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::producers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyProducers;
}
void PersistentTrack::orderProducersItems(const Array<std::shared_ptr<PersistentArtist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyProducers->rearrange(std::move(objects), to);
}
void PersistentTrack::orderProducersItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyProducers->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderProducersItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyProducers->rearrange(std::move(indicies), to);
}
void PersistentTrack::addProducersItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyTracksCreditedAsProducer->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyProducers->append(inverse);
}
void PersistentTrack::removeAllProducers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyProducers) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyTracksCreditedAsProducer->removeObjectWithID(objectId);
    }
    manyProducers->removeAll();
}
void PersistentTrack::removeProducersItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyProducers)[index];
    lockedContext->updateObject(inverse->objectId);
    manyProducers->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsProducer->removeObjectWithID(objectId);
}
void PersistentTrack::removeProducersItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyProducers->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsProducer->removeObjectWithID(objectId);
}



// -- to-many relationship: remixers
Array<ObjectID> PersistentTrack::remixersObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyRemixers->objectIDs();
}
Set<ObjectID> PersistentTrack::remixersObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyRemixers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyRemixers, remixers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsRemixer, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyRemixers->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadRemixers(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyRemixers->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load, RELATION(PersistentTrack::manyRemixers, remixers, Artist, Ordered, ToMany, Nullify, tracksCreditedAsRemixer, Unordered, ToMany));
    }
    if (!manyRemixers->internalVector) {
        return 0;
    }
    return manyRemixers->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForRemixersItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyRemixers->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentArtist>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::remixersConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyRemixers;
}
PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::remixers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyRemixers;
}
void PersistentTrack::orderRemixersItems(const Array<std::shared_ptr<PersistentArtist>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyRemixers->rearrange(std::move(objects), to);
}
void PersistentTrack::orderRemixersItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyRemixers->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderRemixersItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyRemixers->rearrange(std::move(indicies), to);
}
void PersistentTrack::addRemixersItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyTracksCreditedAsRemixer->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyRemixers->append(inverse);
}
void PersistentTrack::removeAllRemixers()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyRemixers) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyTracksCreditedAsRemixer->removeObjectWithID(objectId);
    }
    manyRemixers->removeAll();
}
void PersistentTrack::removeRemixersItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyRemixers)[index];
    lockedContext->updateObject(inverse->objectId);
    manyRemixers->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsRemixer->removeObjectWithID(objectId);
}
void PersistentTrack::removeRemixersItem(const std::shared_ptr<PersistentArtist>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyRemixers->removeObjectWithID(inverse->objectId);
    inverse->manyTracksCreditedAsRemixer->removeObjectWithID(objectId);
}



// -- to-many relationship: stringProperties
Array<ObjectID> PersistentTrack::stringPropertiesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyStringProperties->objectIDs();
}
Set<ObjectID> PersistentTrack::stringPropertiesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyStringProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyStringProperties, stringProperties, StringProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyStringProperties->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadStringProperties(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyStringProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyStringProperties, stringProperties, StringProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    if (!manyStringProperties->internalVector) {
        return 0;
    }
    return manyStringProperties->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForStringPropertiesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyStringProperties->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentStringProperty>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::stringPropertiesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyStringProperties;
}
PersistentRelationship<std::shared_ptr<PersistentStringProperty>,
                       RekordBuddy::CollectionImplementation::RBSchema,
                       LocalType>& PersistentTrack::stringProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyStringProperties;
}
void PersistentTrack::orderStringPropertiesItems(const Array<std::shared_ptr<PersistentStringProperty>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyStringProperties->rearrange(std::move(objects), to);
}
void PersistentTrack::orderStringPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyStringProperties->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderStringPropertiesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyStringProperties->rearrange(std::move(indicies), to);
}
void PersistentTrack::addStringPropertiesItem(const std::shared_ptr<PersistentStringProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyParentTracks->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyStringProperties->append(inverse);
}
void PersistentTrack::removeAllStringProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyStringProperties) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyStringProperties->removeAll();
}
void PersistentTrack::removeStringPropertiesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyStringProperties)[index];
    lockedContext->updateObject(inverse->objectId);
    manyStringProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}
void PersistentTrack::removeStringPropertiesItem(const std::shared_ptr<PersistentStringProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyStringProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}



// -- to-many relationship: timeProperties
Array<ObjectID> PersistentTrack::timePropertiesObjectIDs() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTimeProperties->objectIDs();
}
Set<ObjectID> PersistentTrack::timePropertiesObjectIDsOfType(typename RBSchema::Type type) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->borrowLock();
    if (manyTimeProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadAllOfTypeFromToMany(type,
                                              RELATION(PersistentTrack::manyTimeProperties, timeProperties, TimeProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    MutableSet<ObjectID> result;
    for (auto && oid : manyTimeProperties->objectIDs()) {
        if (RBSchema::typeIs(oid, type)) {
            result.add(oid);
        }
    }
    return result;
}
count PersistentTrack::loadTimeProperties(bool load) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (manyTimeProperties->isRelationshipFaulted()) {
        SomeSourceBinder<RBSchema> binder{*lockedContext->source, Mode::Load, objectId};
        return binder.loadToMany(load,
                                 RELATION(PersistentTrack::manyTimeProperties, timeProperties, TimeProperty, Ordered, ToMany, Nullify, parentTracks, Unordered, ToMany));
    }
    if (!manyTimeProperties->internalVector) {
        return 0;
    }
    return manyTimeProperties->internalVector->size();
}
typename RBSchema::ObjectID PersistentTrack::objectIDForTimePropertiesItemAtIndex(count index) const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return manyTimeProperties->getObjectIDAtIndex(index);
}
const PersistentRelationship<std::shared_ptr<PersistentTimeProperty>,
                             RekordBuddy::CollectionImplementation::RBSchema,
                             LocalType>& PersistentTrack::timePropertiesConst() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTimeProperties;
}
PersistentRelationship<std::shared_ptr<PersistentTimeProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& PersistentTrack::timeProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    return *manyTimeProperties;
}
void PersistentTrack::orderTimePropertiesItems(const Array<std::shared_ptr<PersistentTimeProperty>>& objects, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTimeProperties->rearrange(std::move(objects), to);
}
void PersistentTrack::orderTimePropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>& objectids, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTimeProperties->rearrange(std::move(objectids), to);
}
void PersistentTrack::orderTimePropertiesItemsAtIndex(const Set<count>& indicies, count to)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    manyTimeProperties->rearrange(std::move(indicies), to);
}
void PersistentTrack::addTimePropertiesItem(const std::shared_ptr<PersistentTimeProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectID());

    inverse->manyParentTracks->append(*lockedContext->fetchObject<PersistentTrack>(objectId));
    manyTimeProperties->append(inverse);
}
void PersistentTrack::removeAllTimeProperties()
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    for (auto && inverse : *manyTimeProperties) {
        lockedContext->updateObject(inverse->objectId);
        inverse->manyParentTracks->removeObjectWithID(objectId);
    }
    manyTimeProperties->removeAll();
}
void PersistentTrack::removeTimePropertiesItemAtIndex(count index)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto inverse = (*manyTimeProperties)[index];
    lockedContext->updateObject(inverse->objectId);
    manyTimeProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}
void PersistentTrack::removeTimePropertiesItem(const std::shared_ptr<PersistentTimeProperty>& inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    lockedContext->updateObject(inverse->objectId);
    manyTimeProperties->removeObjectWithID(inverse->objectId);
    inverse->manyParentTracks->removeObjectWithID(objectId);
}
