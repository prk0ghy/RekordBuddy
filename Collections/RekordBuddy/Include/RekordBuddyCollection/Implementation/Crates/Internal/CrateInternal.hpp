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

#include <RekordBuddyCollection/Implementation/Crates/Crate.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

namespace NxA {

// -- Operator Specialization
inline constexpr bool operator==(const RekordBuddy::CollectionImplementation::PersistentCrate& first,
                                 const RekordBuddy::CollectionImplementation::PersistentCrate& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const RekordBuddy::CollectionImplementation::PersistentCrate& first,
                                const RekordBuddy::CollectionImplementation::PersistentCrate& second) noexcept
{
    return first.objectID() < second.objectID();
}

namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions
void p_deleteCrateFromCollection(PersistentCrate&);
boolean p_crateIsAPlaylist(const PersistentCrate&);
boolean p_crateIsAFilter(const PersistentCrate&);
boolean p_crateHasFilters(const PersistentCrate&);
boolean p_crateHasSortingCrates(const PersistentCrate&);

} } }
