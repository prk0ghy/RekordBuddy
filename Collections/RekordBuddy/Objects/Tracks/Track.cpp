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

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Instance Methods

Optional<std::shared_ptr<CollectionImplementation::PersistentMarkerImportOffset>> MutableTrack::p_maybeMarkerImportOffsetForCollectionIn(Common::Collection::Type collectionType) const
{
    for (auto&& markerImportOffset : this->p_maybeRekordBuddyTrack->asPersistent()->markerImportOffsets()) {
        if (markerImportOffset->collectionTypeID() == static_cast<integer32>(collectionType)) {
            return markerImportOffset;
        }
    }

    return nothing;
}

CollectionImplementation::TrackDisplayCache& MutableTrack::p_ensureTrackDisplayCacheIsLoaded(AndMakeSureTrackIsNotModified andMakeSureTrackIsNotModified) const
{
    if (andMakeSureTrackIsNotModified == AndMakeSureTrackIsNotModified::Yes) {
        // -- If the track was edited without calling markAsModifiedNow() then the cache data may be invalid.
        NXA_ASSERT_FALSE(this->p_wasEdited);
    }

    if (!this->p_maybeRekordBuddyTrackDisplayCache.isValid()) {
        this->p_maybeRekordBuddyTrackDisplayCache = this->p_collection.asReference().asRekordBuddyCollection().asReference().trackDisplayCacheFor(this->p_trackObjectID);
        NXA_ASSERT_TRUE(this->p_maybeRekordBuddyTrackDisplayCache.isValid());
    }

    return *this->p_maybeRekordBuddyTrackDisplayCache;
}

CollectionImplementation::Track& MutableTrack::p_ensureTrackIsLoaded() const
{
    if (!this->p_maybeRekordBuddyTrack.isValid()) {
        // -- p_collection can be considered mutable for the sake of this operation
        auto maybePersistentTrack = const_cast<MutableTrack*>(this)->p_collection.asReference().asRekordBuddyCollection().asReference().persistentContext()->fetchObject<CollectionImplementation::PersistentTrack>(this->p_trackObjectID);
        NXA_ASSERT_TRUE(maybePersistentTrack.isValid());

        this->p_maybeRekordBuddyTrack = CollectionImplementation::Track{ *maybePersistentTrack };
    }

    return *this->p_maybeRekordBuddyTrack;
}

NotNull<const Common::Collection*> MutableTrack::collection() const
{
    return this->p_collection.asNotNull();
}

NotNull<Common::MutableCollection*> MutableTrack::collection()
{
    return this->p_collection.asNotNull();
}

Volume MutableTrack::volume() const
{
    return this->p_collection.asReference().asRekordBuddyCollection().asReference().volume();
}

void MutableTrack::markAsModifiedNow()
{
    Pointer<CollectionImplementation::Track> rekordBuddyTrack{ &this->p_ensureTrackIsLoaded() };

    rekordBuddyTrack.asReference().markAsModifiedNow();

    if (this->p_wasEdited) {
        auto& collection = (*this->p_collection);
        collection.markAsModifiedNow();

        (*collection.asRekordBuddyCollection()).updateCachesForTrack(this);

        Array<NotNull<CollectionImplementation::Track*>> asSingleTrackArray{ rekordBuddyTrack.asNotNull() };
        CollectionImplementation::Track::updateSmartContentForParentPlaylistsOfTracks(asSingleTrackArray);

        this->p_wasEdited = false;
    }
}

Optional<DecimalNumber> MutableTrack::maybeOffsetToAddInSecondsWhenImportingFrom(Common::Collection::Type collectionType) const
{
    for (auto&& markerImportOffset : this->p_maybeRekordBuddyTrack->asPersistent()->markerImportOffsets()) {
        if (markerImportOffset->collectionTypeID() == static_cast<integer32>(collectionType)) {
            return markerImportOffset->offsetToAddInSeconds();
        }
    }

    return nothing;
}

void MutableTrack::updateMarkerOffsets()
{
    auto maybeExistingRekordboxMarkerImportOffset = this->p_maybeMarkerImportOffsetForCollectionIn(Common::Collection::Type::rekordbox);

    // -- Let's calculate the offset needed for rekordbox.
    auto maybeRekordboxMarkerImportOffset = Common::MarkerOffset::maybeOffsetToAddInSecondsForFileAtWhenImportingFrom(this->absoluteFilePath(),
                                                                                                                      Common::Collection::Type::rekordbox);
    if (!maybeRekordboxMarkerImportOffset.isValid()) {
        if (maybeExistingRekordboxMarkerImportOffset.isValid()) {
            // -- If we don't have an offset but we used to have one, we delete the old one.
            (*maybeExistingRekordboxMarkerImportOffset)->deleteObject();
        }
    }
    else if (maybeExistingRekordboxMarkerImportOffset.isValid()) {
        // -- If we had a previous offset, we update it.
        (*maybeExistingRekordboxMarkerImportOffset)->setOffsetToAddInSeconds(*maybeRekordboxMarkerImportOffset);
    }
    else {
        auto context = this->p_collection.asReference().asRekordBuddyCollection().asReference().persistentContext();

        // -- Otherwise we create a new one.
        auto newMarkerImportOffset = context->createObject<CollectionImplementation::PersistentMarkerImportOffset>();
        newMarkerImportOffset->setCollectionTypeID(static_cast<integer32>(Common::Collection::Type::rekordbox));
        newMarkerImportOffset->setOffsetToAddInSeconds(*maybeRekordboxMarkerImportOffset);

        this->p_ensureTrackIsLoaded().asPersistent()->addMarkerImportOffsetsItem(newMarkerImportOffset);
    }
}
