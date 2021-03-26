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

#include <RekordBuddyCollection/Implementation/Crates/Internal/AllTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Methods

static Optional<AllTracksPlaylist> p_maybeAllTracksPlaylistInContext(RootContext& context)
{
    auto allTracksPlaylistsFound = context->fetchObjects<PersistentAllTracksPlaylist>();
    if (allTracksPlaylistsFound.length() == 0) {
        return nothing;
    }
    NXA_ASSERT_TRUE(allTracksPlaylistsFound.length() == 1);

    auto persistentResult = allTracksPlaylistsFound.anyObject();
    NXA_ASSERT_FALSE(persistentResult->isDeleted());

    return { AllTracksPlaylist(std::move(persistentResult)) };
}

} } }

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        AllTracksPlaylist
#define NXA_OBJECT_BASE_CLASS                                   Playlist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

Const<AllTracksPlaylist> AllTracksPlaylist::inCollection(const Collection& collection)
{
    // -- The const_cast here is tolerable because we are returning const anyway.
    auto maybeAllTracksPlaylists = p_maybeAllTracksPlaylistInContext(const_cast<Collection&>(collection).persistentContext());
    NXA_ASSERT_TRUE(maybeAllTracksPlaylists.isValid());

    return *maybeAllTracksPlaylists;
}

AllTracksPlaylist AllTracksPlaylist::inCollection(Collection& collection)
{
    auto context = collection.persistentContext();
    auto maybeAllTracksPlaylists = p_maybeAllTracksPlaylistInContext(context);
    if (maybeAllTracksPlaylists.isValid()) {
        return *maybeAllTracksPlaylists;
    }

    auto persistentResult = context->createObject<PersistentAllTracksPlaylist>();
    persistentResult->setName("<All Tracks Playlist>"_String);

    auto result = AllTracksPlaylist(std::move(persistentResult));
    result.markAsModifiedNow();

    return result;
}

Optional<AllTracksPlaylist> AllTracksPlaylist::maybeExistingInCollection(Collection& collection)
{
    return p_maybeAllTracksPlaylistInContext(collection.persistentContext());
}
