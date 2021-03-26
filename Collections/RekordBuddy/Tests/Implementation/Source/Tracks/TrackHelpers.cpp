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


#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Tracks/Internal/TrackInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

NotNull<RekordBuddy::MutableTrack*> emptyTrackInCollection(Collection& collection)
{
    return collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix).m4a"));
}

} } }
