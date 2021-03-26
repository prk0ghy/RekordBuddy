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

#include <RekordBuddyCollection/Implementation/Tags/Tag.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

template<typename T>
inline void p_deleteTagFromCollection(T& tag)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto sortingPlaylistsCopy = tag.sortingPlaylists().objects();

    tag.deleteObject();

    for (auto&& sortingPlaylist : sortingPlaylistsCopy) {
        SortingPlaylist(std::move(sortingPlaylist)).deleteFromCollection();
    }
}

template<typename T>
inline boolean p_tagIsAnOrphan(const T& tag)
{
    auto context = tag.getContext();

    for (auto&& objectID : tag.parentTracksObjectIDs()) {
        if (!context->isDeleted(objectID)) {
            return false;
        }
    }

    return true;
}

} } }
