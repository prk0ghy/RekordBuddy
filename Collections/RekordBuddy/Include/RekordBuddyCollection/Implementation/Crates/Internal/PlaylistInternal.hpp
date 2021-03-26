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

#pragma once

#include <RekordBuddyCollection/Implementation/Crates/Playlist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/OtherTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTag.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateInternal.hpp>

namespace NxA {

// -- Operator Specialization
inline constexpr bool operator==(const RekordBuddy::CollectionImplementation::PersistentPlaylist& first,
                                 const RekordBuddy::CollectionImplementation::PersistentPlaylist& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const RekordBuddy::CollectionImplementation::PersistentPlaylist& first,
                                const RekordBuddy::CollectionImplementation::PersistentPlaylist& second) noexcept
{
    return first.objectID() < second.objectID();
}

namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions
template<typename T>
    void p_addTagsFoundOfTypeInTrackTo(Common::Property::TypeID typeID, Track track, String propertyDescription, MutableMap<String, Tag::OfSomeSort>& tagsFoundBySortingValue)
    {
        // -- We look at the track's tags which match the sorting criteria.
        // -- If we have tags that match, we go through each different SORTING value found for the tag.
        for (auto&& tag : track.tagsOfBaseTypeWithTypeID<T>(typeID)) {
            tagsFoundBySortingValue.setValueForKey(tag, tag.sortingValue());
        }
    }

void p_updatePlaylistSmartContent(Playlist&, PersistentPlaylist&, Crate::AndUpdateClones = Crate::AndUpdateClones::No);
void p_sortPlaylistWithCriteria(Playlist&, const SortingCriteria&);
void p_sortPlaylistWithCriterias(Playlist&, PersistentPlaylist&, Array<SortingCriteria>);
void p_deletePlaylistFromCollection(PersistentPlaylist&);

// -- Template Specialization
template<>
    void p_addTagsFoundOfTypeInTrackTo<StringProperty>(Common::Property::TypeID, Track, String, MutableMap<String, Tag::OfSomeSort>&);

} } }
