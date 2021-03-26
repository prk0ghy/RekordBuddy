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

#include <SeratoCollection/Crates/AllTracksPlaylist.hpp>
#include <SeratoCollection/Collection.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Instance Methods

Optional<count> MutableAllTracksPlaylist::p_addExistingTrackAtIndex(NotNull<Common::MutableTrack*> track, count index)
{
    auto maybeSeratoTrack = track.maybeAs<Serato::MutableTrack*>();
    NXA_ASSERT_TRUE(maybeSeratoTrack.isValid());

    auto& collection = *this->p_collection;
    NXA_ASSERT_TRUE(static_cast<Common::MutableCollection&>(collection) == *track->collection().get());

    // -- AllTrackPlaylist doesn't move an already existing track, it leaves it in place to preserve the user's ordering.
    auto maybeExistingIndex = collection.seratoDatabase().maybeIndexForTrack(*maybeSeratoTrack);
    NXA_ASSERT_TRUE(maybeExistingIndex.isValid());

    collection.markAsModifiedNow();

    return maybeExistingIndex;
}

NotNull<const Common::Collection*> MutableAllTracksPlaylist::collection() const
{
    return this->p_collection.asNotNull();
}

NotNull<Common::MutableCollection*> MutableAllTracksPlaylist::collection()
{
    return this->p_collection.asNotNull();
}

Time MutableAllTracksPlaylist::lastModificationTime() const
{
    return (*this->p_collection).seratoDatabase().modificationTime();
}

count MutableAllTracksPlaylist::numberOfTracks() const
{
    return (*this->p_collection).seratoDatabase().numberOfTracks();
}

NotNull<const Common::Track*> MutableAllTracksPlaylist::trackAtIndex(count index) const
{
    return (*this->p_collection).seratoDatabase().trackAtIndex(index);
}

NotNull<Common::MutableTrack*> MutableAllTracksPlaylist::trackAtIndex(count index)
{
    return (*this->p_collection).seratoDatabase().trackAtIndex(index);
}

void MutableAllTracksPlaylist::moveTracksAtIndicesToIndex(Array<count> indices, count toIndex)
{
    auto& collection = (*this->p_collection);
    collection.seratoDatabase().moveTracksAtIndicesToIndex(indices, toIndex);
    collection.markAsModifiedNow();
}

void MutableAllTracksPlaylist::moveTrackAtIndexTo(count index, count toIndex)
{
    auto& collection = (*this->p_collection);
    collection.seratoDatabase().moveTrackAtIndexTo(index, toIndex);
    collection.markAsModifiedNow();
}

void MutableAllTracksPlaylist::removeTrackAtIndex(count index)
{
    auto& collection = (*this->p_collection);
    collection.rootFolder()->removeTrackWithRelativeFilePath(this->trackAtIndex(index)->relativeFilePath());
    collection.removeTrackAtIndex(index);
}

void MutableAllTracksPlaylist::removeAllTracks()
{
    auto& collection = (*this->p_collection);
    collection.rootFolder()->removeAllTracks();
    collection.removeAllTracks();
}
