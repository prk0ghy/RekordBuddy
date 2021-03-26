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

#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/SmartPlaylistInternal.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>
#include <CommonCollection/Tracks/Track.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

void p_updateSmartPlaylistSmartContent(SmartPlaylist& playlist,
                                       PersistentSmartPlaylist& persistentPlaylist,
                                       Crate::AndUpdateClones andUpdateClones)
{
    auto maybePredicate = persistentPlaylist.maybePredicate();
    NXA_ASSERT_TRUE(maybePredicate.isValid());

    auto& collection = playlist.collection().asReference();
    for (auto&& trackObjectID : collection.tracksMatchingPredicate(Common::TrackPredicate::fromBlob(*maybePredicate))) {
        playlist.addTrackAtIndex(collection.trackWithObjectID(trackObjectID), playlist.numberOfTracks());
    }

    p_updatePlaylistSmartContent(playlist, persistentPlaylist, andUpdateClones);
}

} } }
