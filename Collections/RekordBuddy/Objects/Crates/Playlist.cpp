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

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Crates/AllTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/RootFolder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/OtherTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>

#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/SmartPlaylist.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Instance Methods

inline Pointer<const RekordBuddy::MutableCollection> MutablePlaylist::p_rekordBuddyCollection() const
{
    return this->p_rekordBuddyPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}
inline Pointer<RekordBuddy::MutableCollection> MutablePlaylist::p_rekordBuddyCollection()
{
    return this->p_rekordBuddyPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& MutablePlaylist::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisAsMutated = const_cast<MutablePlaylist*>(this);
        this->p_maybeSubCrates = thisAsMutated->p_rekordBuddyPlaylist.playlistSubCrates(thisAsMutated);
    }

    return *this->p_maybeSubCrates;
}

Optional<count> MutablePlaylist::p_addExistingTrackAtIndex(NotNull<Common::MutableTrack*> track, count toIndex)
{
    auto maybeRekordBuddyMutableTrack = track.maybeAs<RekordBuddy::MutableTrack*>();
    NXA_ASSERT_TRUE(maybeRekordBuddyMutableTrack.isValid());
    auto thisCollection = this->collection();
    NXA_ASSERT_TRUE(thisCollection == track->collection());

    auto& rekordBuddyPlaylist = this->p_rekordBuddyPlaylist;
    rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();

    if (thisCollection->tracks().get() == this) {
        // -- If the track exists as an argument, that means it's already in the collection.
        // -- If it's in the collection that means creating it has added it to the AllTracksPlaylist already.
        // -- So we just check to see if it's already where it should be and if not, move it.
        auto trackObjectID = (*maybeRekordBuddyMutableTrack)->reference();

        auto maybeCurrentIndex = rekordBuddyPlaylist.maybeIndexForTrackObjectID(trackObjectID);
        NXA_ASSERT_TRUE(maybeCurrentIndex.isValid());

        if (!this->p_maybeParentFolder.isValid()) {
            this->p_rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);
            (*this->p_rekordBuddyCollection()).markAsModifiedNow();

            // -- AllTrackPlaylist doesn't move an already existing track, it leaves it in place to preserve the user's ordering.
            return maybeCurrentIndex;
        }
        else {
            auto& currentIndex = *maybeCurrentIndex;
            if (toIndex == currentIndex) {
                return toIndex;
            }

            count resultingIndex = currentIndex < toIndex ? (toIndex - 1) : toIndex;
            rekordBuddyPlaylist.moveTrackAtIndexTo(currentIndex, toIndex);

            return resultingIndex;
        }
    }
    else {
        return rekordBuddyPlaylist.addTrackAtIndex(*maybeRekordBuddyMutableTrack, toIndex);
    }
}

NotNull<const Common::Collection*> MutablePlaylist::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

NotNull<Common::MutableCollection*> MutablePlaylist::collection()
{
    return this->p_rekordBuddyCollection().asNotNull();
}

Optional<NotNull<const Common::Folder*>> MutablePlaylist::maybeParentFolder() const
{
    return this->p_maybeParentFolder.maybe([](auto folder) {
        return folder.template as<const Common::Folder*>();
    });
}

Optional<NotNull<Common::MutableFolder*>> MutablePlaylist::maybeParentFolder()
{
    return this->p_maybeParentFolder.maybe([](auto folder) {
        return folder.template as<Common::MutableFolder*>();
    });
}

NotNull<Common::MutablePlaylist*> MutablePlaylist::cloneWithName(const String& name)
{
    auto maybeParentFolder = this->p_maybeParentFolder;
    NXA_ASSERT_TRUE(maybeParentFolder.isValid());

    return (*maybeParentFolder)->newCloneOfPlaylistWithName(this, name);
}

NotNull<const Common::Track*> MutablePlaylist::trackAtIndex(count index) const
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddyPlaylist.trackObjectIDAtIndex(index));
}

NotNull<Common::MutableTrack*> MutablePlaylist::trackAtIndex(count index)
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddyPlaylist.trackObjectIDAtIndex(index));
}

Common::SubCrate MutablePlaylist::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<const Common::SmartPlaylist*> {
        return crate.asRawPointer();
    });
}

Common::MutableSubCrate MutablePlaylist::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<Common::MutableSmartPlaylist*> {
        return crate.asRawPointer();
    });
}

void MutablePlaylist::moveTracksAtIndicesToIndex(Array<count> indices, count toIndex)
{
    this->p_rekordBuddyPlaylist.moveTracksAtIndicesToIndex(indices, toIndex);
    this->p_rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::moveTrackAtIndexTo(count index, count toIndex)
{
    this->p_rekordBuddyPlaylist.moveTrackAtIndexTo(index, toIndex);
    this->p_rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::removeTrackAtIndex(count index)
{
    auto& rekordBuddyPlaylist = this->p_rekordBuddyPlaylist;
    rekordBuddyPlaylist.removeTrackAtIndex(index);
    rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::removeAllTracks()
{
    auto& rekordBuddyPlaylist = this->p_rekordBuddyPlaylist;
    rekordBuddyPlaylist.removeAllTracks();
    rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::organizeBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    this->p_rekordBuddyPlaylist.addSortingCriteriaForTrackTag(tag);

    this->p_maybeSubCrates = nothing;

    this->p_rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::removeOrganizationBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    auto rekordBuddyTag = static_cast<const MutableTag&>(tag);
    this->p_rekordBuddyPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());

    this->p_maybeSubCrates = nothing;

    this->p_rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);
    (*this->p_rekordBuddyCollection()).markAsModifiedNow();
}

void MutablePlaylist::organizeBy(Common::Property::TypeID typeID)
{
    auto& rekordBuddyPlaylist = this->p_rekordBuddyPlaylist;
    rekordBuddyPlaylist.addSortingCriteriaForTagTypeID(typeID);
    rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);

    (*this->p_rekordBuddyCollection()).markAsModifiedNow();

    this->p_maybeSubCrates = nothing;
}

void MutablePlaylist::removeOrganizationBy(Common::Property::TypeID typeID)
{
    auto& rekordBuddyPlaylist = this->p_rekordBuddyPlaylist;
    rekordBuddyPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
    rekordBuddyPlaylist.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::No);

    (*this->p_rekordBuddyCollection()).markAsModifiedNow();

    this->p_maybeSubCrates = nothing;
}
