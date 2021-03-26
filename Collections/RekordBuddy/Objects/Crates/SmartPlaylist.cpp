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

#include <CommonCollection/Crates/SmartPlaylist.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Instance Methods

MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& MutableSmartPlaylist::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisAsMutated = const_cast<MutableSmartPlaylist*>(this);
        this->p_maybeSubCrates = thisAsMutated->p_rekordBuddySmartPlaylist.playlistSubCrates(thisAsMutated);
    }

    return *this->p_maybeSubCrates;
}

NotNull<Common::MutableSmartPlaylist*> MutableSmartPlaylist::cloneWithName(const String& name)
{
    return this->p_parentFolder->newCloneOfSmartPlaylistWithName(this, name);
}

Common::SubCrate MutableSmartPlaylist::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<const Common::SmartPlaylist*> {
        return crate.asRawPointer();
    });
}

Common::MutableSubCrate MutableSmartPlaylist::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) -> NotNull<Common::MutableSmartPlaylist*> {
        return crate.asRawPointer();
    });
}
