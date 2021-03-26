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

#include <RekordBuddyCollection/Implementation/Crates/Internal/RootFolderInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Methods

static Optional<RootFolder> p_rootFolderInContext(RootContext& context)
{
    auto rootFoldersFound = context->fetchObjects<PersistentRootFolder>();
    if (rootFoldersFound.length() == 0) {
        return nothing;
    }

    NXA_ASSERT_TRUE(rootFoldersFound.length() == 1);

    auto persistentResult = rootFoldersFound.anyObject();
    NXA_ASSERT_FALSE(persistentResult->isDeleted());

    return { RootFolder(std::move(persistentResult)) };
}

static RootFolder p_rootFolderInContextAndCreateIfDoesNotExist(RootContext& context)
{
    auto maybeRootFolder = p_rootFolderInContext(context);
    if (maybeRootFolder.isValid()) {
        return *maybeRootFolder;
    }

    auto persistentResult = context->createObject<PersistentRootFolder>();
    persistentResult->setName("Root"_String);

    auto result = RootFolder(std::move(persistentResult));
    result.markAsModifiedNow();

    return result;
}

} } }

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        RootFolder
#define NXA_OBJECT_BASE_CLASS                                   Folder
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

Const<RootFolder> RootFolder::inCollection(const Collection& collection)
{
    const auto& context = collection.persistentContext();
    auto rootFoldersFound = context.get()->fetchObjects<PersistentRootFolder>();
    NXA_ASSERT_TRUE(rootFoldersFound.length() == 1);

    auto persistentResult = rootFoldersFound.anyObject();
    NXA_ASSERT_FALSE(persistentResult->isDeleted());

    return { RootFolder(std::move(persistentResult)) };
}

RootFolder RootFolder::inCollection(Collection& collection)
{
    return p_rootFolderInContextAndCreateIfDoesNotExist(collection.persistentContext());
}

Optional<RootFolder> RootFolder::maybeExistingInCollection(Collection& collection)
{
    return p_rootFolderInContext(collection.persistentContext());
}
