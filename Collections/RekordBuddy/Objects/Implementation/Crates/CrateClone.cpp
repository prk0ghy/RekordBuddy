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

#include <RekordBuddyCollection/Implementation/Crates/Internal/CrateCloneInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/Playlist.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        CrateClone
#define NXA_OBJECT_BASE_CLASS                                   Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

CrateClone CrateClone::ofPlaylistWithNameAndParent(Playlist& playlist, const String& name, Folder& parent)
{
    auto persistentResult = playlist.collection().asReference().persistentContext()->createObject<PersistentCrateClone>();
    persistentResult->setCloneOf(playlist.persistentObject());

    auto clone = CrateClone(persistentResult);
    clone->setName(name);

    parent.persistentObject()->addSubCratesItem(clone.persistentObject());

    parent.markAsModifiedNow();
    clone.markAsModifiedNow();

    return clone;
}

CrateClone CrateClone::ofFolderWithNameAndParent(Folder& folder, const String& name, Folder& parent)
{
    auto persistentResult = folder.collection().asReference().persistentContext()->createObject<PersistentCrateClone>();
    persistentResult->setCloneOf(folder.persistentObject());

    auto clone = CrateClone(persistentResult);
    clone->setName(name);

    parent.persistentObject()->addSubCratesItem(clone.persistentObject());

    clone.markAsModifiedNow();
    parent.markAsModifiedNow();

    return clone;
}

CrateClone CrateClone::ofSmartPlaylistWithNameAndParent(SmartPlaylist& smartPlaylist, const String& name, Folder& parent)
{
    auto persistentResult = smartPlaylist.collection().asReference().persistentContext()->createObject<PersistentCrateClone>();
    persistentResult->setCloneOf(smartPlaylist.persistentObject());

    auto clone = CrateClone(persistentResult);
    clone->setName(name);

    parent.persistentObject()->addSubCratesItem(clone.persistentObject());

    parent.markAsModifiedNow();
    clone.markAsModifiedNow();

    return clone;
}

// -- Instance Methods

CrateClone::Target CrateClone::cloneOf() const
{
    auto cloneOf = nxa_const_internal->cloneOf();
    switch (cloneOf->objectType()) {
        case RekordBuddy::CollectionImplementation::RBSchema::Type::AllTracksPlaylist:
        case RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist: {
            return Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf));
        }
        case RekordBuddy::CollectionImplementation::RBSchema::Type::Folder: {
            return Folder(std::static_pointer_cast<PersistentFolder>(cloneOf));
        }
        default: {
            NXA_ALOG("Illegal folder entry type.");
        }
    }
}

Array<ObjectID> CrateClone::trackObjectIDs() const
{
    auto internal = nxa_const_internal;
    auto cloneOfId = internal->cloneOfObjectID();
    if (RekordBuddy::CollectionImplementation::RBSchema::typeIs(cloneOfId.objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        auto context = internal->getContext();
        auto clone = context->fetchObject<PersistentPlaylist>(cloneOfId);
        NXA_ASSERT_TRUE(clone.isValid());

        return (*clone)->tracksObjectIDs();
    }

    return { };
}

Set<ObjectID> CrateClone::uniqueAndUnorderedObjectIDs() const
{
    auto cloneOf = nxa_const_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        return Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).uniqueAndUnorderedObjectIDs();
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        return SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).uniqueAndUnorderedObjectIDs();
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Folder)) {
        return Folder(std::static_pointer_cast<PersistentFolder>(cloneOf)).uniqueAndUnorderedObjectIDs();
    }

    NXA_ALOG_WITH_FORMAT("Invalid clone of with type %d", cloneOf->objectType());
}

boolean CrateClone::hasSortingCriteriaForTagTypeID(Common::Property::TypeID tagTypeID) const
{
    auto cloneOf = nxa_const_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        return Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).hasSortingCriteriaForTagTypeID(tagTypeID);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        return SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).hasSortingCriteriaForTagTypeID(tagTypeID);
    }

    NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
}

boolean CrateClone::hasSortingCriteriaForTrackTagWithDescription(const String& description) const
{
    auto cloneOf = nxa_const_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        return Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).hasSortingCriteriaForTrackTagWithDescription(description);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        return SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).hasSortingCriteriaForTrackTagWithDescription(description);
    }

    NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
}

void CrateClone::addSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto cloneOf = nxa_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).addSortingCriteriaForTagTypeID(typeID);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).addSortingCriteriaForTagTypeID(typeID);
    }
    else {
        NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
    }
}

void CrateClone::addSortingCriteriaForTrackTag(Common::MutableTag& tag)
{
    auto cloneOf = nxa_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).addSortingCriteriaForTrackTag(tag);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).addSortingCriteriaForTrackTag(tag);
    }
    else {
        NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
    }
}

void CrateClone::removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto cloneOf = nxa_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).removeAndDeleteSortingCriteriaForTagTypeID(typeID);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).removeAndDeleteSortingCriteriaForTagTypeID(typeID);
    }
    else {
        NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
    }
}

void CrateClone::removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String& description)
{
    auto cloneOf = nxa_internal->cloneOf();
    if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
        Playlist(std::static_pointer_cast<PersistentPlaylist>(cloneOf)).removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
    }
    else if (RBSchema::typeIs(cloneOf->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::SmartPlaylist)) {
        SmartPlaylist(std::static_pointer_cast<PersistentSmartPlaylist>(cloneOf)).removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
    }
    else {
        NXA_ALOG_WITH_FORMAT("Invalid call on clone of with type %d", cloneOf->objectType());
    }
}
