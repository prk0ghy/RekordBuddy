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

#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/Playlist.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Crates/Internal/FolderInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        Folder
#define NXA_OBJECT_BASE_CLASS                                   Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

Folder Folder::withNameAndParent(const String& name, Folder& parentFolder)
{
    auto newPersistentCrate = (*parentFolder.collection()).persistentContext()->createObject<PersistentFolder>();
    newPersistentCrate->setName(name);

    auto folder = Folder(std::static_pointer_cast<PersistentFolder>(newPersistentCrate));

    parentFolder.persistentObject()->addSubCratesItem(folder.persistentObject());

    folder.markAsModifiedNow();

    return folder;
}

// -- Instance Methods

void Folder::moveSubCrateAtIndexTo(count fromIndex, count toIndex)
{
    if (fromIndex == toIndex) {
        return;
    }

    nxa_internal->orderSubCratesItemsAtIndex({ fromIndex }, toIndex);
    this->markAsModifiedNow();
}

void Folder::moveCrateFromFolderAtIndexTo(Folder& folder, count fromIndex, count toIndex)
{
    auto internal = nxa_internal;

    auto crate = NXA_INTERNAL_OBJECT_FOR(folder)->subCrates()[fromIndex];
    folder.removeSubCrateAtIndex(fromIndex);

    count appendIndex = internal->loadSubCrates(true);
    internal->addSubCratesItem(crate);

    if (appendIndex != toIndex) {
        internal->orderSubCratesItemsAtIndex({ appendIndex }, toIndex);
    }

    this->markAsModifiedNow();
}

void Folder::removeSubCrateAtIndex(count index)
{
    auto internal = nxa_internal;
    NXA_ASSERT_TRUE(index < internal->subCrates().length());

    internal->removeSubCratesItemAtIndex(index);
}

Set<ObjectID> Folder::uniqueAndUnorderedObjectIDs() const
{
    MutableSet<ObjectID> result;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& subCrates = nxa_internal->subCrates();
    for (auto&& entry : subCrates) {
        if (RBSchema::typeIs(entry->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
            result.add(Playlist(std::static_pointer_cast<PersistentPlaylist>(entry)).uniqueAndUnorderedObjectIDs());
        }
        else if (RBSchema::typeIs(entry->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Folder)) {
            result.add(Folder(std::static_pointer_cast<PersistentFolder>(entry)).uniqueAndUnorderedObjectIDs());
        }
        else if (RBSchema::typeIs(entry->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::CrateClone)) {
            result.add(CrateClone(std::static_pointer_cast<PersistentCrateClone>(entry)).uniqueAndUnorderedObjectIDs());
        }
    }

    return std::move(result);
}
