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
#include <RekordBuddyCollection/Tracks/Tag.hpp>

#include <CommonCollection/Crates/SmartPlaylist.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Instance Methods

MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& MutableOtherTracksPlaylist::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisAsMutated = const_cast<MutableOtherTracksPlaylist*>(this);
        this->p_maybeSubCrates = thisAsMutated->p_rekordBuddyOtherTracksPlaylist.playlistSubCrates(NotNull<MutableOtherTracksPlaylist*>{ thisAsMutated });
    }

    return *this->p_maybeSubCrates;
}

NotNull<const Common::Collection*> MutableOtherTracksPlaylist::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

NotNull<Common::MutableCollection*> MutableOtherTracksPlaylist::collection()
{
    return this->p_rekordBuddyCollection().asNotNull();
}

Optional<NotNull<const Common::Folder*>> MutableOtherTracksPlaylist::maybeParentFolder() const
{
    return this->p_parentPlaylist.apply([](auto playlist) {
        return playlist.template as<const Common::Folder*>();
    });
}

Optional<NotNull<Common::MutableFolder*>> MutableOtherTracksPlaylist::maybeParentFolder()
{
    // -- This type of playlist has another playlist as a parent so it can masquerade it as a Common::Folder but
    // -- not as a Common::MutableFolder. So here, we return nothing.
    return nothing;
}

Common::SubCrate MutableOtherTracksPlaylist::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<const Common::SmartPlaylist*> {
        return crate.asRawPointer();
    });
}

Common::MutableSubCrate MutableOtherTracksPlaylist::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<Common::MutableSmartPlaylist*> {
        return crate.asRawPointer();
    });
}

NotNull<const Common::Track*> MutableOtherTracksPlaylist::trackAtIndex(count index) const
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddyOtherTracksPlaylist.trackObjectIDAtIndex(index)).as<const Common::Track*>();
}

NotNull<Common::MutableTrack*> MutableOtherTracksPlaylist::trackAtIndex(count index)
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddyOtherTracksPlaylist.trackObjectIDAtIndex(index));
}

boolean MutableOtherTracksPlaylist::isOrganizedBy(const Common::Tag& tag) const
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    auto rekordBuddyTag = static_cast<const Tag&>(tag);
    return this->p_rekordBuddyOtherTracksPlaylist.hasSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
}

void MutableOtherTracksPlaylist::organizeBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    this->p_rekordBuddyOtherTracksPlaylist.addSortingCriteriaForTrackTag(tag);
    this->p_maybeSubCrates = nothing;
}

void MutableOtherTracksPlaylist::removeOrganizationBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    auto rekordBuddyTag = static_cast<const MutableTag&>(tag);
    this->p_rekordBuddyOtherTracksPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
    this->p_maybeSubCrates = nothing;
}
