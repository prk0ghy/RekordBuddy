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

#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateInternal.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

void p_deleteCrateFromCollection(PersistentCrate& crate)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto subCratesCopy = crate.subCrates().objects();
    auto clonesCopy = crate.clones().objects();

    crate.deleteObject();

    for (auto&& subCrate : subCratesCopy) {
        Crate(std::move(subCrate)).deleteFromCollection();
    }

    for (auto&& clone : clonesCopy) {
        Crate(std::move(clone)).deleteFromCollection();
    }
}

boolean p_crateIsAPlaylist(const PersistentCrate& crate)
{
    auto type = crate.objectType();
    if (RBSchema::typeIs(type, RBSchema::Type::CrateClone)) {
        return p_crateIsAPlaylist(*static_cast<const PersistentCrateClone&>(crate).cloneOf());
    }

    return RBSchema::typeIs(type, RBSchema::Type::Playlist);
}

boolean p_crateIsAFilter(const PersistentCrate& crate)
{
    return RBSchema::typeIs(crate.objectType(), RBSchema::Type::FilterPlaylist);
}

boolean p_crateHasFilters(const PersistentCrate& crate)
{
    NXA_ASSERT_TRUE(p_crateIsAPlaylist(crate) || p_crateIsAFilter(crate));

    auto crateObjectIDs = crate.subCratesObjectIDs();
    if (!crateObjectIDs.length()) {
        return false;
    }

    return crateObjectIDs.firstObject().objectType() == RBSchema::Type::FilterPlaylist;
}

boolean p_crateHasSortingCrates(const PersistentCrate& crate)
{
    NXA_ASSERT_TRUE(p_crateIsAPlaylist(crate) || p_crateIsAFilter(crate));

    auto crateObjectIDs = crate.subCratesObjectIDs();
    if (!crateObjectIDs.length()) {
        return false;
    }

    auto firstObjectType = crateObjectIDs.firstObject().objectType();
    return RBSchema::typeIs(firstObjectType, RBSchema::Type::SortingPlaylist) ||
           RBSchema::typeIs(firstObjectType, RBSchema::Type::OtherTracksPlaylist);
}

} } }
