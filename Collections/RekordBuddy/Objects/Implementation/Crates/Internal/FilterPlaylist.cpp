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

#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

Playlist p_sourcePlaylistFor(std::shared_ptr<PersistentCrate>& persistentCrate)
{
    switch (persistentCrate->objectType()) {
        case RBSchema::Type::Playlist: {
            return Playlist(std::static_pointer_cast<PersistentPlaylist>(persistentCrate));
        }
        case RBSchema::Type::SortingPlaylist:
        case RBSchema::Type::FilterPlaylist: {
            auto maybeParentPersistentCrate = persistentCrate->maybeParentCrate();
            NXA_ASSERT_TRUE(maybeParentPersistentCrate.isValid());
            return Playlist(std::static_pointer_cast<PersistentPlaylist>(*maybeParentPersistentCrate));
        }
        case RBSchema::Type::SmartPlaylist: {
            auto& collection = persistentCrate->getContext()->userData<RekordBuddy::MutableCollection>()->asRekordBuddyCollection().asReference();
            auto maybeAllTracksPlaylist = AllTracksPlaylist::maybeExistingInCollection(collection);
            NXA_ASSERT_TRUE(maybeAllTracksPlaylist);
            return *maybeAllTracksPlaylist;
        }
        case RBSchema::Type::CrateClone: {
            auto persistentClone = std::static_pointer_cast<PersistentCrateClone>(persistentCrate);
            auto cloneOf = persistentClone->cloneOf();
            NXA_ASSERT_EQ(cloneOf->objectType(), RBSchema::Type::Playlist);
            return Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf));
        }
        default: {
            NXA_ALOG("Illegal playlist to find a source for.");
        }
    }
}

void p_updateFilterPlaylistSmartContent(FilterPlaylist& playlist,
                                        PersistentFilterPlaylist& persistentPlaylist)
{
    auto maybePredicate = persistentPlaylist.maybePredicate();
    NXA_ASSERT_TRUE(maybePredicate.isValid());

    auto& collection = playlist.collection().asReference();
    auto asPersistentCrate = std::static_pointer_cast<PersistentCrate>(playlist.persistentObject());
    auto sourcePlaylist = p_sourcePlaylistFor(asPersistentCrate);
    for (auto&& trackObjectID : sourcePlaylist.tracksMatchingPredicate(Common::TrackPredicate::fromBlob(*maybePredicate))) {
        playlist.addTrackAtIndex(collection.trackWithObjectID(trackObjectID), playlist.numberOfTracks());
    }
    
    p_updatePlaylistSmartContent(playlist, persistentPlaylist);
}

Playlist p_topParentPlaylistFor(std::shared_ptr<PersistentSortingPlaylist> persistentPlaylist)
{
    auto maybeParentPersistentCrate = persistentPlaylist->maybeParentCrate();
    if (maybeParentPersistentCrate.isValid()) {
        auto& parentPersistentCrate = *maybeParentPersistentCrate;
        auto objectType = parentPersistentCrate->objectType();
        if (RBSchema::typeIs(objectType, RBSchema::Type::SortingPlaylist)) {
            auto asPersistentSortingPlaylist = std::static_pointer_cast<PersistentSortingPlaylist>(parentPersistentCrate);
            return p_topParentPlaylistFor(asPersistentSortingPlaylist);
        }
        else {
            auto asPersistentPlaylist = std::static_pointer_cast<PersistentPlaylist>(parentPersistentCrate);
            return Playlist(asPersistentPlaylist);
        }
    }

    return Playlist(persistentPlaylist);
}

} } }
