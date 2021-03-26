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
#include <CommonCollection/Crates/Folder.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Instance Methods

MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& MutableFilterPlaylist::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisAsMutated = const_cast<MutableFilterPlaylist*>(this);
        this->p_maybeSubCrates = thisAsMutated->p_rekordBuddyFilterPlaylist.playlistSubCrates(thisAsMutated);
    }

    return *this->p_maybeSubCrates;
}

Optional<NotNull<const Common::Folder*>> MutableFilterPlaylist::maybeParentFolder() const
{
    return this->p_parentPlaylist.apply([](auto playlist) {
        return playlist.template as<const Common::Folder*>();
    });
}

Optional<NotNull<Common::MutableFolder*>> MutableFilterPlaylist::maybeParentFolder()
{
    // -- This type of playlist has another playlist as a parent so it can masquerade it as a Common::Folder but
    // -- not as a Common::MutableFolder. So here, we return nothing.
    return nothing;
}

Common::SubCrate MutableFilterPlaylist::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) {
        return NotNull<const Common::SmartPlaylist*>{ crate.asRawPointer() };
    });
}

Common::MutableSubCrate MutableFilterPlaylist::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) {
        return NotNull<Common::MutableSmartPlaylist*>{ crate.asRawPointer() };
    });
}
