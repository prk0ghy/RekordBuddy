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

#include <CommonCollection/Crates/SmartPlaylist.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Instance Methods

inline Pointer<const RekordBuddy::MutableCollection> MutableSortingPlaylist::p_rekordBuddyCollection() const
{
    return this->p_rekordBuddySortingPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

inline Pointer<RekordBuddy::MutableCollection> MutableSortingPlaylist::p_rekordBuddyCollection()
{
    return this->p_rekordBuddySortingPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& MutableSortingPlaylist::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisAsMutated = const_cast<MutableSortingPlaylist*>(this);
        this->p_maybeSubCrates = thisAsMutated->p_rekordBuddySortingPlaylist.playlistSubCrates(thisAsMutated);
    }

    return *this->p_maybeSubCrates;
}

NotNull<const Common::Collection*> MutableSortingPlaylist::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

NotNull<Common::MutableCollection*> MutableSortingPlaylist::collection()
{
    return this->p_rekordBuddyCollection().asNotNull();
}

Optional<NotNull<const Common::Folder*>> MutableSortingPlaylist::maybeParentFolder() const
{
    return this->p_parentPlaylist.apply([](auto playlist) {
        return playlist.template as<const Common::Folder*>();
    });
}

Optional<NotNull<Common::MutableFolder*>> MutableSortingPlaylist::maybeParentFolder()
{
    // -- This type of playlist has another playlist as a parent so it can masquerade it as a Common::Folder but
    // -- not as a Common::MutableFolder. So here, we return nothing.
    return nothing;
}

Common::SubCrate MutableSortingPlaylist::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<const Common::SmartPlaylist*> {
        return crate.asRawPointer();
    });
}

Common::MutableSubCrate MutableSortingPlaylist::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<Common::MutableSmartPlaylist*> {
        return crate.asRawPointer();
    });
}

NotNull<const Common::Track*> MutableSortingPlaylist::trackAtIndex(count index) const
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddySortingPlaylist.trackObjectIDAtIndex(index)).as<const Common::Track*>();
}

NotNull<Common::MutableTrack*> MutableSortingPlaylist::trackAtIndex(count index)
{
    return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddySortingPlaylist.trackObjectIDAtIndex(index));
}

boolean MutableSortingPlaylist::isOrganizedBy(const Common::Tag& tag) const
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    auto rekordBuddyTag = static_cast<const Tag&>(tag);
    return this->p_rekordBuddySortingPlaylist.hasSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
}

void MutableSortingPlaylist::organizeBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    this->p_rekordBuddySortingPlaylist.addSortingCriteriaForTrackTag(tag);
    this->p_maybeSubCrates = nothing;
}

void MutableSortingPlaylist::removeOrganizationBy(Common::MutableTag& tag)
{
    NXA_ASSERT_TRUE(this->collection() == tag.collection());
    auto rekordBuddyTag = static_cast<const MutableTag&>(tag);
    this->p_rekordBuddySortingPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
    this->p_maybeSubCrates = nothing;
}
