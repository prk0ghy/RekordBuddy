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

#include <RekordBuddyCollection/Implementation/Crates/Internal/SortingPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Properties.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

void p_deleteSortingPlaylistFromCollection(PersistentSortingPlaylist& playlist)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    boolean alreadyFoundOneTag = false;

    if (playlist.maybeStringPropertyUsedToSortByObjectID().isValid()) {
        auto tag = StringProperty(*playlist.maybeStringPropertyUsedToSortBy());
        playlist.setMaybeStringPropertyUsedToSortBy(nothing);
        alreadyFoundOneTag = true;

        if (tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    if (playlist.maybeIntegerPropertyUsedToSortByObjectID().isValid()) {
        NXA_ASSERT_FALSE(alreadyFoundOneTag);

        auto tag = IntegerProperty(*playlist.maybeIntegerPropertyUsedToSortBy());
        playlist.setMaybeIntegerPropertyUsedToSortBy(nothing);
        alreadyFoundOneTag = true;

        if (tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    if (playlist.maybeDecimalPropertyUsedToSortByObjectID().isValid()) {
        NXA_ASSERT_FALSE(alreadyFoundOneTag);

        auto tag = DecimalProperty(*playlist.maybeDecimalPropertyUsedToSortBy());
        playlist.setMaybeDecimalPropertyUsedToSortBy(nothing);
        alreadyFoundOneTag = true;

        if (tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    if (playlist.maybeTimePropertyUsedToSortByObjectID().isValid()) {
        NXA_ASSERT_FALSE(alreadyFoundOneTag);

        auto tag = TimeProperty(*playlist.maybeTimePropertyUsedToSortBy());
        playlist.setMaybeTimePropertyUsedToSortBy(nothing);

        if (tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    Optional<SortingCriteria> maybeSortingCriteriaToDelete;

    if (playlist.criteriaObjectID().isValid()) {
        auto persistentCriteria = playlist.criteria();
        NXA_ASSERT_NOT_NULL(persistentCriteria);
        persistentCriteria->removeSortingPlaylistsItem(playlist.sharedFromThis<PersistentSortingPlaylist>());
        if (persistentCriteria->sortingPlaylists().length() == 0) {
            maybeSortingCriteriaToDelete = SortingCriteria(std::move(persistentCriteria));
        }
    }

    p_deleteFilterPlaylistFromCollection(playlist);

    if (maybeSortingCriteriaToDelete.isValid()) {
        maybeSortingCriteriaToDelete->deleteFromCollection();
    }
}

} } }
