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

#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Tracks.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/PlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/SmartPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/OtherTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateCloneInternal.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

#include <Base/Array.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        Playlist
#define NXA_OBJECT_BASE_CLASS                                   Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Private Functions

namespace NxA { namespace RekordBuddy {

static Optional<Const<std::shared_ptr<PersistentSortingCriteria>>>
    p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(const PersistentPlaylist& playlist,
                                                                    Common::Property::TypeID tagTypeID,
                                                                    Optional<String> propertyDescription = nothing)
{
    NXA_ASSERT_TRUE(p_crateIsAFilter(playlist));

    // -- We are using a non-const object here because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& sortedBy = const_cast<PersistentPlaylist&>(playlist).sortedBy();
    for (auto&& criteria : sortedBy) {
        if (static_cast<Common::Property::TypeID>(criteria->tagTypeBeingSorted()) == tagTypeID) {
            if (!propertyDescription.isValid() || (*propertyDescription == criteria->sortingPropertyDescription())) {
                return { criteria };
            }
        }
    }

    return nothing;
}

static Optional<std::shared_ptr<PersistentSortingCriteria>>
    p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(PersistentPlaylist& playlist,
                                                                    Common::Property::TypeID tagTypeID,
                                                                    Optional<String> propertyDescription = nothing)
{
    NXA_ASSERT_TRUE(p_crateIsAFilter(playlist));

    for (auto&& criteria : playlist.sortedBy()) {
        if (static_cast<Common::Property::TypeID>(criteria->tagTypeBeingSorted()) == tagTypeID) {
            if (!propertyDescription.isValid() || (*propertyDescription == criteria->sortingPropertyDescription())) {
                return criteria;
            }
        }
    }

    return nothing;
}

} }

// -- Factory Methods

Playlist Playlist::withNameAndParent(const String& name, Folder& parentFolder)
{
    auto newPersistentCrate = parentFolder.collection().asReference().persistentContext()->createObject<PersistentPlaylist>();
    newPersistentCrate->setName(name);

    auto playlist = Playlist(std::static_pointer_cast<PersistentPlaylist>(newPersistentCrate));

    parentFolder.persistentObject()->addSubCratesItem(playlist.persistentObject());

    playlist.markAsModifiedNow();

    return playlist;
}

// -- Class Methods

void Playlist::updateSmartContentForPlaylistsInCollectionSortedByTagsOfType(Collection& collection,
                                                                            Common::Property::TypeID tagTypeID)
{
    MutableArray<Playlist> playlistsToUpdate;

    Array<std::shared_ptr<PersistentPlaylist>> allPersistentPlaylists{ collection.persistentContext()->fetchObjects<PersistentPlaylist>() };
    for (auto&& persistentPlaylist : allPersistentPlaylists) {
        if (p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(*persistentPlaylist, tagTypeID).isValid()) {
            playlistsToUpdate.append(Playlist(std::move(persistentPlaylist)));
        }
    }

    for (auto&& playlist : playlistsToUpdate) {
        playlist.updateSmartContent(Crate::AndUpdateClones::No);
    }
}

// -- Instance Methods

count Playlist::numberOfTracks() const
{
    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& tracks = nxa_internal->tracks();
    return tracks.length();
}

Track Playlist::trackAtIndex(count index)
{
    return Track(nxa_internal->tracks()[index]);
}

count Playlist::addTrackAtIndex(NotNull<RekordBuddy::MutableTrack*> track, count toIndex)
{
    auto internal = nxa_internal;

    internal->addTracksItem(track->asPersistent());

    auto currentIndex = internal->tracks().length() - 1;
    if (toIndex == currentIndex) {
        return toIndex;
    }

    count resultingIndex = currentIndex < toIndex ? (toIndex - 1) : toIndex;
    internal->orderTracksItemsAtIndex({ currentIndex }, toIndex);

    return resultingIndex;
}

void Playlist::removeTrackAtIndex(count index)
{
    if (nxa_internal->objectID().objectType() == RBSchema::Type::AllTracksPlaylist) {
        (*this->collection()).removeTrackWithRelativeFilePath(this->trackAtIndex(index).relativeFilePath());
    }
    else {
        nxa_internal->tracks().removeAtIndex(index);
    }
}

void Playlist::removeAllTracks()
{
    if (nxa_internal->objectID().objectType() == RBSchema::Type::AllTracksPlaylist) {
        (*this->collection()).removeAllTracks();
    }
    else {
        nxa_internal->tracks().removeAll();
    }
}

Optional<count> Playlist::maybeIndexForTrackObjectID(const ObjectID& trackObjectID) const
{
#if defined(DEBUG)
    Optional<count> result;
#endif
    count index = 0;

    for (auto&& track : nxa_internal->tracks()) {
        if (track->objectID() == trackObjectID) {
#if defined(DEBUG)
            // -- We should only have one entry per track in a playlist.
            NXA_ASSERT_FALSE(result.isValid());
            result = index;
#else
            return { index };
#endif
        }

        ++index;
    }

#if defined(DEBUG)
    return result;
#else
    return { };
#endif
}

ObjectID Playlist::trackObjectIDAtIndex(count index) const
{
    return nxa_const_internal->objectIDForTracksItemAtIndex(index);
}

Array<ObjectID> Playlist::trackObjectIDs() const
{
    return nxa_const_internal->tracksObjectIDs();
}

Set<ObjectID> Playlist::uniqueAndUnorderedObjectIDs() const
{
    MutableArray<ObjectID> result;
    result.appendObjectsFrom(this->trackObjectIDs());
    return Array<ObjectID>{ std::move(result) };
}

Set<ObjectID> Playlist::tracksMatchingPredicate(const Common::TrackPredicate& predicate) const
{
    auto matchesInCollection = this->collection().asReference().tracksMatchingPredicate(predicate);
    return matchesInCollection.intersectionWith(Set<ObjectID>{ this->trackObjectIDs() });
}

void Playlist::moveTrackAtIndexTo(count index, count toIndex)
{
    nxa_internal->orderTracksItemsAtIndex({ index }, toIndex);
}

void Playlist::moveTracksAtIndicesToIndex(const Set<count>& indices, count toIndex)
{
    nxa_internal->orderTracksItemsAtIndex(indices, toIndex);
}

void Playlist::moveTracksWithObjectIDsToIndex(const Array<ObjectID>& tracks, count toIndex)
{
    nxa_internal->orderTracksItemsByObjectID(tracks, toIndex);
}

boolean Playlist::hasAnySortingCriteria() const
{
    return nxa_internal->loadSortedBy(true) != 0;
}

boolean Playlist::hasSortingCriteriaForTagTypeID(Common::Property::TypeID tagTypeID) const
{
    auto valueTypeID = Common::Property::valueTypeIDFromPropertyType(tagTypeID);
    NXA_ASSERT_TRUE((valueTypeID == Common::Property::StringValueType) ||
                    (valueTypeID == Common::Property::IntegerValueType) ||
                    (valueTypeID == Common::Property::TimeValueType) ||
                    (valueTypeID == Common::Property::DecimalValueType));

    // -- Track tags should be tested using the other hasSorting() method.
    NXA_ASSERT_TRUE(tagTypeID != Common::Property::TypeID::Tag);

    return p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(*nxa_const_internal, tagTypeID).isValid();
}

boolean Playlist::hasSortingCriteriaForTrackTagWithDescription(const String& description) const
{
    return p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(*nxa_const_internal, Common::Property::TypeID::Tag, description).isValid();
}

void Playlist::addSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto valueTypeID = Common::Property::valueTypeIDFromPropertyType(typeID);
    NXA_ASSERT_TRUE((valueTypeID == Common::Property::StringValueType) ||
                    (valueTypeID == Common::Property::IntegerValueType) ||
                    (valueTypeID == Common::Property::TimeValueType) ||
                    (valueTypeID == Common::Property::DecimalValueType));

    auto newCriteria = CollectionImplementation::SortingCriteria::withTypeInCollection(typeID, this->collection().asReference());

    auto internal = nxa_internal;
    NXA_ASSERT_FALSE(p_crateHasFilters(*internal) && !p_crateHasSortingCrates(*internal));

    internal->addSortedByItem(newCriteria.persistentObject());

    this->updateSmartContent(AndUpdateClones::No);
}

void Playlist::addSortingCriteriaForTrackTag(Common::MutableTag& tag)
{
    auto rekordBuddyTag = static_cast<const RekordBuddy::MutableTag&>(tag);
    auto sortsABooleanTag = rekordBuddyTag.hasChildrenTags() ?
                            CollectionImplementation::SortingCriteria::SortsABooleanTypeOfTag::Yes :
                            CollectionImplementation::SortingCriteria::SortsABooleanTypeOfTag::No;
    auto newCriteria = CollectionImplementation::SortingCriteria::withTrackTagDescriptionInCollection(rekordBuddyTag.propertyDescription(),
                                                                                                      this->collection().asReference(),
                                                                                                      sortsABooleanTag);

    auto internal = nxa_internal;
    NXA_ASSERT_FALSE(p_crateHasFilters(*internal) && !p_crateHasSortingCrates(*internal));

    internal->addSortedByItem(newCriteria.persistentObject());

    this->updateSmartContent(AndUpdateClones::No);
}

void Playlist::removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto valueTypeID = Common::Property::valueTypeIDFromPropertyType(typeID);
    NXA_ASSERT_TRUE((valueTypeID == Common::Property::StringValueType) ||
                    (valueTypeID == Common::Property::IntegerValueType) ||
                    (valueTypeID == Common::Property::TimeValueType) ||
                    (valueTypeID == Common::Property::DecimalValueType));

    // -- Track tags should be tested using the other removeAndDelete() method.
    NXA_ASSERT_TRUE(typeID != Common::Property::TypeID::Tag);

    auto maybePersistentCriteria = p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(*nxa_internal, typeID);
    NXA_ASSERT_TRUE(maybePersistentCriteria.isValid());

    auto internal = nxa_internal;
    internal->removeSortedByItem(*maybePersistentCriteria);

    this->updateSmartContent(AndUpdateClones::No);

    // -- We have to delete the criteria last because we don't want it to cascade deletions of other criterias found after it.
    SortingCriteria(*maybePersistentCriteria).deleteFromCollection();
}

void Playlist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String& description)
{
    auto maybePersistentCriteria = p_maybePersistentSortingCriteriaForTagTypeIDAndMaybeDescription(*nxa_internal,
                                                                                                   Common::Property::TypeID::Tag,
                                                                                                   description);
    NXA_ASSERT_TRUE(maybePersistentCriteria.isValid());

    auto internal = nxa_internal;
    internal->removeSortedByItem(*maybePersistentCriteria);

    this->updateSmartContent(AndUpdateClones::No);

    // -- We have to delete the criteria last because we don't want it to cascade deletions of other criterias found after it.
    SortingCriteria(*maybePersistentCriteria).deleteFromCollection();
}

void Playlist::updateSmartContent(Crate::AndUpdateClones andUpdateClones)
{
    auto internal = nxa_internal;
    switch (internal->objectID().objectType()) {
        case RBSchema::Type::AllTracksPlaylist: {
            break;
        }
        case RBSchema::Type::Playlist: {
            p_updatePlaylistSmartContent(*this,
                                         static_cast<PersistentPlaylist&>(*internal),
                                         andUpdateClones);
            break;
        }
        case RBSchema::Type::FilterPlaylist:
        case RBSchema::Type::SortingPlaylist: {
            p_updateFilterPlaylistSmartContent(static_cast<FilterPlaylist&>(*this),
                                               static_cast<PersistentFilterPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::SmartPlaylist: {
            p_updateSmartPlaylistSmartContent(static_cast<SmartPlaylist&>(*this),
                                              static_cast<PersistentSmartPlaylist&>(*internal),
                                              andUpdateClones);
            break;
        }
        case RBSchema::Type::OtherTracksPlaylist: {
            p_updateOtherTracksPlaylistSmartContent(static_cast<OtherTracksPlaylist&>(*this),
                                                    static_cast<PersistentOtherTracksPlaylist&>(*internal));
            break;
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid playlist type %d.", internal->objectID().objectType());
        }
    }

    this->markAsModifiedNow(andUpdateClones);
}
