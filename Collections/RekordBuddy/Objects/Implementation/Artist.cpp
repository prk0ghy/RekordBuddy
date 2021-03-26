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

#include <RekordBuddyCollection/Tracks/Track.hpp>

#include <RekordBuddyCollection/Implementation/Artist.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Tracks.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Operator Specialization

namespace NxA {

inline constexpr bool operator==(const PersistentArtist& first, const PersistentArtist& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const PersistentArtist& first, const PersistentArtist& second) noexcept
{
    return first.objectID() < second.objectID();
}

}

// -- Implementation

#define NXA_OBJECT_CLASS                                        Artist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Class Methods

Array<Artist> Artist::artistsInCollection(Collection& collection)
{
    MutableArray<Artist> artists;

    for (auto persistentArtist : collection.persistentContext()->fetchObjects<PersistentArtist>()) {
        artists.append(Artist{ persistentArtist });
    }

    return std::move(artists);
}

// -- Protected Factory Methods

Artist Artist::p_artistNamedInCollection(const String& name, Collection& collection)
{
    auto newPersistentArtist = collection.persistentContext()->createObject<PersistentArtist>();
    auto newArtist = Artist{ newPersistentArtist };

    newArtist->setName(name);

    return newArtist;
}

// -- Protected Instance Methods

void Artist::p_setName(const String& name)
{
    auto internal = nxa_internal;
    if (name == internal->name()) {
        return;
    }

    internal->setName(name);

    for (auto&& track : this->allTracksCredited()) {
        track->markAsModifiedNow();
    }
}

void Artist::p_deleteFromCollection()
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    for (auto&& track : this->allTracksCredited()) {
        track->markAsModifiedNow();
    }

    internal->deleteObject();
}

// -- Instance Methods

String Artist::name() const
{
    return nxa_const_internal->name();
}

Array<ObjectID> Artist::objectIDsOfAllTracksCredited() const
{
    MutableSet<ObjectID> trackObjectIDs;
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsArtist());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsProducer());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsRemixer());
    return { std::move(trackObjectIDs) };
}

Array<ObjectID> Artist::objectIDsOfTracksCreditedAsArtist() const
{
    return nxa_const_internal->tracksCreditedAsArtistObjectIDs();
}

Array<ObjectID> Artist::objectIDsOfTracksCreditedAsRemixer() const
{
    return nxa_const_internal->tracksCreditedAsRemixerObjectIDs();
}

Array<ObjectID> Artist::objectIDsOfTracksCreditedAsProducer() const
{
    return nxa_const_internal->tracksCreditedAsProducerObjectIDs();
}

Array<NotNull<const RekordBuddy::MutableTrack*>> Artist::allTracksCredited() const
{
    auto& collection = this->collection().asReference();

    MutableSet<ObjectID> trackObjectIDs;
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsArtist());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsProducer());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsRemixer());

    MutableArray<NotNull<const RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : trackObjectIDs) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<const RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsArtist() const
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<const RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : nxa_const_internal->tracksCreditedAsArtistObjectIDs()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<const RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsRemixer() const
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<const RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : nxa_const_internal->tracksCreditedAsRemixerObjectIDs()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<const RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsProducer() const
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<const RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : nxa_const_internal->tracksCreditedAsProducerObjectIDs()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<RekordBuddy::MutableTrack*>> Artist::allTracksCredited()
{
    auto& collection = this->collection().asReference();

    MutableSet<ObjectID> trackObjectIDs;
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsArtist());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsProducer());
    trackObjectIDs.add(this->objectIDsOfTracksCreditedAsRemixer());

    MutableArray<NotNull<RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : trackObjectIDs) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsArtist()
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : this->objectIDsOfTracksCreditedAsArtist()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsRemixer()
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : this->objectIDsOfTracksCreditedAsRemixer()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

Array<NotNull<RekordBuddy::MutableTrack*>> Artist::tracksCreditedAsProducer()
{
    auto& collection = this->collection().asReference();

    MutableArray<NotNull<RekordBuddy::MutableTrack*>> results;

    for (auto&& reference : this->objectIDsOfTracksCreditedAsProducer()) {
        results.append(collection.trackWithObjectID(reference));
    }

    return std::move(results);
}

void Artist::updateSmartContentForParentCratesOfParentTracks()
{
    MutableArray<NotNull<CollectionImplementation::Track*>> rekordBuddyTracks;

    for (auto&& track : this->allTracksCredited()) {
        rekordBuddyTracks.emplaceAppend(&track->asRekordBuddyTrack());
    }

    Array<NotNull<CollectionImplementation::Track*>> asArray{ std::move(rekordBuddyTracks) };
    Track::updateSmartContentForParentPlaylistsOfTracks(asArray);
}

Pointer<const Collection> Artist::collection() const
{
    return { nxa_const_internal->getContext()->userData<Collection>() };
}

Pointer<Collection> Artist::collection()
{
    return { nxa_internal->getContext()->userData<Collection>() };
}

boolean Artist::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void Artist::deleteFromCollection()
{
    (*this->collection()).deleteArtist(*this);
}
