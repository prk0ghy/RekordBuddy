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

#include <RekordBuddyCollection/Implementation/Crates/Internal/SmartPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Crates/AllTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Folder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/RootFolder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/CrateClone.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        SmartPlaylist
#define NXA_OBJECT_BASE_CLASS                                   FilterPlaylist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

SmartPlaylist SmartPlaylist::withNameAndParent(const String& name, Folder& folder)
{
    auto newPersistentCrate = folder.collection().asReference().persistentContext()->createObject<PersistentSmartPlaylist>();
    newPersistentCrate->setName(name);

    auto newSmartPlaylist = SmartPlaylist(newPersistentCrate);

    folder.persistentObject()->addSubCratesItem(newSmartPlaylist.persistentObject());

    newSmartPlaylist.markAsModifiedNow();

    return newSmartPlaylist;
}
