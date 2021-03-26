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

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Instance Methods

Pointer<const RekordBuddy::MutableCollection> Tag::p_rekordBuddyCollection() const
{
    return this->p_rekordBuddyTrackTag.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}
Pointer<RekordBuddy::MutableCollection> Tag::p_rekordBuddyCollection()
{
    return this->p_rekordBuddyTrackTag.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

// -- Overridden Common::Tag Instance Methods

NotNull<const Common::Collection*> Tag::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

Array<NotNull<const Common::Track*>> Tag::tracks() const
{
    auto& collection = this->p_rekordBuddyCollection().asReference();

    MutableArray<NotNull<const Common::Track*>> results;

    for (auto&& trackObjectID : this->p_rekordBuddyTrackTag.trackObjectIDs()) {
        results.emplaceAppend(collection.trackWithObjectID(trackObjectID).as<const Common::Track*>());
    }

    return std::move(results);
}

// -- Overridden Common::MutableTag Instance Methods

NotNull<const Common::Collection*> MutableTag::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

NotNull<Common::MutableCollection*> MutableTag::collection()
{
    return this->p_rekordBuddyCollection().asNotNull();
}

Array<NotNull<Common::MutableTrack*>> MutableTag::tracks()
{
    auto& collection = this->p_rekordBuddyCollection().asReference();

    MutableArray<NotNull<Common::MutableTrack*>> results;

    for (auto&& trackObjectID : this->p_rekordBuddyTrackTag.trackObjectIDs()) {
        results.emplaceAppend(collection.trackWithObjectID(trackObjectID));
    }

    return std::move(results);
}
