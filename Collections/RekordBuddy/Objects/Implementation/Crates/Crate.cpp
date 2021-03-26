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

#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/PlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FolderInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/SortingPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/AllTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateCloneInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/OtherTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/RootFolderInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/SmartPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateInternal.hpp>

#include <RekordBuddyCollection/Crates/OtherTracksPlaylist.hpp>
#include <RekordBuddyCollection/Crates/FilterPlaylist.hpp>
#include <RekordBuddyCollection/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Crates/SmartPlaylist.hpp>
#include <RekordBuddyCollection/Crates/Playlist.hpp>
#include <RekordBuddyCollection/Crates/Folder.hpp>
#include <RekordBuddyCollection/Crates/SmartPlaylistClone.hpp>
#include <RekordBuddyCollection/Crates/PlaylistClone.hpp>
#include <RekordBuddyCollection/Crates/FolderClone.hpp>

#include <RekordBuddyCollection/Collection.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Instance Methods

Time Crate::lastModificationTime() const
{
    return nxa_const_internal->lastModifiedOn();
}

void Crate::markAsModifiedNow(Crate::AndUpdateClones andUpdateClones)
{
    auto now = Time::currentTime();

    auto internal = nxa_internal;
    internal->setLastModifiedOn(now);

    if (andUpdateClones == Crate::AndUpdateClones::Yes) {
        for (auto&& clone : internal->clones()) {
            Crate(clone).markAsModifiedNow(Crate::AndUpdateClones::No);
        }
    }
}

count Crate::numberOfClones() const
{
    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& clones = nxa_internal->clones();
    return clones.length();
}

void Crate::setName(String newName)
{
    nxa_internal->setName(newName);
    this->markAsModifiedNow(Crate::AndUpdateClones::No);
}

String Crate::name() const
{
    return nxa_const_internal->name();
}

Set<ObjectID> Crate::uniqueAndUnorderedObjectIDs() const
{
    auto internal = nxa_const_internal;
    if (RBSchema::typeIs(internal->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        return Playlist(std::static_pointer_cast<PersistentPlaylist>(*this)).uniqueAndUnorderedObjectIDs();
    }
    else if (RBSchema::typeIs(internal->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Folder)) {
        return Folder(std::static_pointer_cast<PersistentFolder>(*this)).uniqueAndUnorderedObjectIDs();
    }
    else {
        NXA_ASSERT_TRUE(RBSchema::typeIs(internal->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::CrateClone));

        return CrateClone(std::static_pointer_cast<PersistentCrateClone>(*this)).uniqueAndUnorderedObjectIDs();
    }
}

MutableArray<Crate::MutableSubCrate> Crate::folderSubCrates(Optional<NotNull<RekordBuddy::MutableFolder*>> maybeParentFolder)
{
    MutableArray<Crate::MutableSubCrate> results;

    NXA_ASSERT_TRUE((nxa_internal->objectType() == RekordBuddy::CollectionImplementation::RBSchema::Type::Folder) ||
                    (nxa_internal->objectType() == RekordBuddy::CollectionImplementation::RBSchema::Type::RootFolder));

    for (auto&& subCrate : nxa_internal->subCrates()) {
        switch (subCrate->objectType()) {
            case RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist: {
                NXA_ASSERT_TRUE(maybeParentFolder.isValid());
                results.emplaceAppend(Unique<RekordBuddy::MutablePlaylist>::with(Playlist{ std::static_pointer_cast<PersistentPlaylist>(subCrate) },
                                                                                 *maybeParentFolder,
                                                                                 RekordBuddy::MutablePlaylist::p_isProtected));
                break;
            }
            case RekordBuddy::CollectionImplementation::RBSchema::Type::Folder: {
                results.emplaceAppend(Unique<RekordBuddy::MutableFolder>::with(Folder{ std::static_pointer_cast<PersistentFolder>(subCrate) },
                                                                               maybeParentFolder,
                                                                               RekordBuddy::MutableFolder::p_isProtected));
                break;
            }
            case RekordBuddy::CollectionImplementation::RBSchema::Type::CrateClone: {
                NXA_ASSERT_TRUE(maybeParentFolder.isValid());
                auto clone = std::static_pointer_cast<PersistentCrateClone>(subCrate);
                switch (clone->cloneOf()->objectType()) {
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist: {
                        results.emplaceAppend(Unique<RekordBuddy::MutablePlaylistClone>::with(CrateClone{ clone },
                                                                                              *maybeParentFolder,
                                                                                              RekordBuddy::MutablePlaylistClone::p_isProtected));
                    }
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::Folder: {
                        results.emplaceAppend(Unique<RekordBuddy::MutableFolderClone>::with(CrateClone{ clone },
                                                                                            *maybeParentFolder,
                                                                                            RekordBuddy::MutableFolderClone::p_isProtected));
                    }
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist: {
                        results.emplaceAppend(Unique<RekordBuddy::MutableSmartPlaylistClone>::with(CrateClone{ clone },
                                                                                                   *maybeParentFolder,
                                                                                                   RekordBuddy::MutableSmartPlaylistClone::p_isProtected));
                    }
                    default: {
                        NXA_ALOG("Illegal crate clone type.");
                    }
                }
                break;
            }
            case RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist: {
                NXA_ASSERT_TRUE(maybeParentFolder.isValid());
                results.emplaceAppend(Unique<RekordBuddy::MutableSmartPlaylist>::with(SmartPlaylist{ std::static_pointer_cast<PersistentSmartPlaylist>(subCrate) },
                                                                                      *maybeParentFolder,
                                                                                      RekordBuddy::MutableSmartPlaylist::p_isProtected));
                break;
            }
            default: {
                NXA_ALOG("Illegal folder sub crate type.");
            }
        }
    }

    return results;
}

MutableArray<Crate::MutablePlaylistSubCrate> Crate::playlistSubCrates(RekordBuddy::MutableParentPlaylist parentPlaylist)
{
    MutableArray<Crate::MutablePlaylistSubCrate> results;

    NXA_ASSERT_FALSE(nxa_internal->objectType() == RekordBuddy::CollectionImplementation::RBSchema::Type::Folder);

    for (auto&& subCrate : nxa_internal->subCrates()) {
        switch (subCrate->objectType()) {
            case RekordBuddy::CollectionImplementation::RBSchema::Type::FilterPlaylist: {
                results.emplaceAppend(Unique<RekordBuddy::MutableFilterPlaylist>::with(FilterPlaylist{ std::static_pointer_cast<PersistentFilterPlaylist>(subCrate) },
                                                                                       parentPlaylist,
                                                                                       RekordBuddy::MutableFilterPlaylist::p_isProtected));
                break;
            }
            case RekordBuddy::CollectionImplementation::RBSchema::Type::SortingPlaylist: {
                results.emplaceAppend(Unique<RekordBuddy::MutableSortingPlaylist>::with(SortingPlaylist{ std::static_pointer_cast<PersistentSortingPlaylist>(subCrate) },
                                                                                        parentPlaylist,
                                                                                        RekordBuddy::MutableSortingPlaylist::p_isProtected));
                break;
            }
            case RekordBuddy::CollectionImplementation::RBSchema::Type::OtherTracksPlaylist: {
                results.emplaceAppend(Unique<RekordBuddy::MutableOtherTracksPlaylist>::with(OtherTracksPlaylist{ std::static_pointer_cast<PersistentOtherTracksPlaylist>(subCrate) },
                                                                                            parentPlaylist,
                                                                                            RekordBuddy::MutableOtherTracksPlaylist::p_isProtected));
                break;
            }
            default: {
                NXA_ALOG("Illegal playlist sub crate type.");
            }
        }
    }

    return results;
}

Pointer<const Collection> Crate::collection() const
{
    return { nxa_const_internal->getContext()->userData<Collection>() };
}

Pointer<Collection> Crate::collection()
{
    return { nxa_internal->getContext()->userData<Collection>() };
}

boolean Crate::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void Crate::deleteFromCollection()
{
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    switch (internal->objectID().objectType()) {
        case RBSchema::Type::Playlist: {
            p_deletePlaylistFromCollection(static_cast<PersistentPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::Folder: {
            p_deleteFolderFromCollection(static_cast<PersistentFolder&>(*internal));
            break;
        }
        case RBSchema::Type::SortingPlaylist: {
            p_deleteSortingPlaylistFromCollection(static_cast<PersistentSortingPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::FilterPlaylist: {
            p_deleteFilterPlaylistFromCollection(static_cast<PersistentFilterPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::SmartPlaylist: {
            p_deleteSmartPlaylistFromCollection(static_cast<PersistentSmartPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::AllTracksPlaylist: {
            p_deleteAllTracksPlaylistFromCollection(static_cast<PersistentAllTracksPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::OtherTracksPlaylist: {
            p_deleteOtherTracksPlaylistFromCollection(static_cast<PersistentOtherTracksPlaylist&>(*internal));
            break;
        }
        case RBSchema::Type::RootFolder: {
            p_deleteRootFolderFromCollection(static_cast<PersistentRootFolder&>(*internal));
            break;
        }
        case RBSchema::Type::CrateClone: {
            p_deleteCrateCloneFromCollection(static_cast<PersistentCrateClone&>(*internal));
            break;
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid crate type %d.", internal->objectID().objectType());
        }
    }
}
