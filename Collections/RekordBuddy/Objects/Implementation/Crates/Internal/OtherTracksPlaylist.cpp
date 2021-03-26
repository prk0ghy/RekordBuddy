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

#include <RekordBuddyCollection/Implementation/Crates/Internal/OtherTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

Set<ObjectID> p_trackObjectIDsNotFoundInChildrenSortingCratesOf(const Playlist& playlist)
{
    MutableSet<ObjectID> trackObjectIDsNotInChildrenCrates;
    trackObjectIDsNotInChildrenCrates.add(playlist.trackObjectIDs());

    for (auto&& subCrate : playlist.persistentObject()->subCrates()) {
        auto type = subCrate->objectType();
        if (RBSchema::typeIs(type, RBSchema::Type::OtherTracksPlaylist)) {
            continue;
        }

        NXA_ASSERT_TRUE(RBSchema::typeIs(type, RBSchema::Type::Playlist));

        trackObjectIDsNotInChildrenCrates.remove(Playlist(std::static_pointer_cast<PersistentPlaylist>(subCrate)).trackObjectIDs());
    }

    return std::move(trackObjectIDsNotInChildrenCrates);
}

void p_updateOtherTracksPlaylistSmartContent(OtherTracksPlaylist& playlist,
                                             PersistentOtherTracksPlaylist& persistentPlaylist)
{
    auto& collection = playlist.collection().asReference();

    auto maybeParentCrate = persistentPlaylist.maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    auto parentPlaylist = Playlist(std::static_pointer_cast<PersistentPlaylist>(*maybeParentCrate));
    for (auto&& trackObjectID : p_trackObjectIDsNotFoundInChildrenSortingCratesOf(parentPlaylist)) {
        playlist.addTrackAtIndex(collection.trackWithObjectID(trackObjectID), playlist.numberOfTracks());
    }

    if (persistentPlaylist.tracks().length()) {
        p_updatePlaylistSmartContent(playlist, persistentPlaylist);
    }
    else {
        playlist.deleteFromCollection();
    }

    p_updatePlaylistSmartContent(playlist, persistentPlaylist);
}

Optional<OtherTracksPlaylist> p_findOtherTrackPlaylistIn(const Playlist& playlist)
{
    for (auto&& persistentSubCrate : playlist.persistentObject()->subCrates()) {
        if (RBSchema::typeIs(persistentSubCrate->objectType(), RBSchema::Type::OtherTracksPlaylist)) {
            return OtherTracksPlaylist(std::static_pointer_cast<PersistentOtherTracksPlaylist>(persistentSubCrate));
        }
    }

    return nothing;
}

} } }
