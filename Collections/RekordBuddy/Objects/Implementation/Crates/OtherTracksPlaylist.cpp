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

#include <CommonCollection/Tracks/Tag.hpp>

#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/OtherTracksPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        OtherTracksPlaylist
#define NXA_OBJECT_BASE_CLASS                                   Playlist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

OtherTracksPlaylist OtherTracksPlaylist::withNameInCollection(const String& name,
                                                              Collection& collection)
{
    auto newPersistentCrate = collection.persistentContext()->createObject<PersistentOtherTracksPlaylist>();
    newPersistentCrate->setName(name);

    auto newOtherTracksPlaylist = OtherTracksPlaylist(newPersistentCrate);
    newOtherTracksPlaylist.markAsModifiedNow();

    return newOtherTracksPlaylist;
}

// -- Class Methods

void OtherTracksPlaylist::addOrUpdateOtherTracksPlaylistNamedIn(const String& name,
                                                                Playlist& playlist)
{
    auto persistentParentPlaylist = playlist.persistentObject();
    boolean hadToCreateIt = false;

    auto maybeOtherTracksPlaylist = p_findOtherTrackPlaylistIn(playlist);
    if (!maybeOtherTracksPlaylist.isValid()) {
        maybeOtherTracksPlaylist = OtherTracksPlaylist::withNameInCollection(name, playlist.collection().asReference());
        persistentParentPlaylist->addSubCratesItem(maybeOtherTracksPlaylist->persistentObject());
        hadToCreateIt = true;
    }
    else {
        maybeOtherTracksPlaylist->markAsModifiedNow();
    }

    p_updateOtherTracksPlaylistSmartContent(*maybeOtherTracksPlaylist, *maybeOtherTracksPlaylist->persistentObject());
    if (maybeOtherTracksPlaylist->isDeletedOrNotInAnyCollection()) {
        return;
    }

    if (!hadToCreateIt) {
        // -- If we did have an 'Other Tracks' crate, we make sure it stays at the end of the list of sibbling crates.
        persistentParentPlaylist->orderSubCratesItems({ maybeOtherTracksPlaylist->persistentObject() }, persistentParentPlaylist->subCrates().length());
    }
}

// -- Instance Methods

void OtherTracksPlaylist::addSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    boolean foundAFilter = false;

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        auto objectType = sibbling->objectType();
        if (objectType == RBSchema::Type::OtherTracksPlaylist) {
            // -- We don't want to add this to ourselves to we skip any OtherTracksPlaylists.
            continue;
        }
        else if (objectType == RBSchema::Type::SortingPlaylist) {
            // -- If we are next to a sorting playlist we just pass this to it and it will pass it up to the source playlist.
            auto sibblingAsSortingPlaylist = SortingPlaylist(std::static_pointer_cast<PersistentSortingPlaylist>(sibbling));
            NXA_ASSERT_FALSE(sibblingAsSortingPlaylist.hasSortingCriteriaForTagTypeID(typeID));
            NXA_ASSERT_FALSE(foundAFilter);

            sibblingAsSortingPlaylist.addSortingCriteriaForTagTypeID(typeID);
            return;
        }

        // -- Otherwise sibblings can only be filters.
        NXA_ASSERT_TRUE(objectType == RBSchema::Type::FilterPlaylist);
        foundAFilter = true;

        // -- If we are next to a filter playlist we just pass this to it and it will pass it accross to the sibblings.
        auto sibblingAsFilterPlaylist = FilterPlaylist(std::static_pointer_cast<PersistentFilterPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsFilterPlaylist.hasSortingCriteriaForTagTypeID(typeID));

        sibblingAsFilterPlaylist.addSortingCriteriaForTagTypeID(typeID);
        return;
    }

    Playlist::addSortingCriteriaForTagTypeID(typeID);
}

void OtherTracksPlaylist::addSortingCriteriaForTrackTag(Common::MutableTag& tag)
{
    boolean foundAFilter = false;

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        auto objectType = sibbling->objectType();
        if (objectType == RBSchema::Type::OtherTracksPlaylist) {
            // -- We don't want to add this to ourselves to we skip any OtherTracksPlaylists.
            continue;
        }
        else if (objectType == RBSchema::Type::SortingPlaylist) {
            // -- If we are next to a sorting playlist we just pass this to it and it will pass it up to the source playlist.
            auto sibblingAsSortingPlaylist = SortingPlaylist(std::static_pointer_cast<PersistentSortingPlaylist>(sibbling));
            NXA_ASSERT_FALSE(sibblingAsSortingPlaylist.hasSortingCriteriaForTrackTagWithDescription(tag.value()));
            NXA_ASSERT_FALSE(foundAFilter);

            sibblingAsSortingPlaylist.addSortingCriteriaForTrackTag(tag);
            return;
        }

        // -- Otherwise sibblings can only be filters.
        NXA_ASSERT_TRUE(objectType == RBSchema::Type::FilterPlaylist);
        foundAFilter = true;

        // -- If we are next to a filter playlist we just pass this to it and it will pass it accross to the sibblings.
        auto sibblingAsFilterPlaylist = FilterPlaylist(std::static_pointer_cast<PersistentFilterPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsFilterPlaylist.hasSortingCriteriaForTrackTagWithDescription(tag.value()));

        sibblingAsFilterPlaylist.addSortingCriteriaForTrackTag(tag);
        return;
    }

    Playlist::addSortingCriteriaForTrackTag(tag);
}

void OtherTracksPlaylist::removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    boolean foundAFilter = false;

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        auto objectType = sibbling->objectType();
        if (objectType == RBSchema::Type::OtherTracksPlaylist) {
            // -- We don't want to add this to ourselves to we skip any OtherTracksPlaylists.
            continue;
        }
        else if (objectType == RBSchema::Type::SortingPlaylist) {
            // -- If we are next to a sorting playlist we just pass this to it and it will pass it up to the source playlist.
            auto sibblingAsSortingPlaylist = SortingPlaylist(std::static_pointer_cast<PersistentSortingPlaylist>(sibbling));
            NXA_ASSERT_FALSE(sibblingAsSortingPlaylist.hasSortingCriteriaForTagTypeID(typeID));
            NXA_ASSERT_FALSE(foundAFilter);

            sibblingAsSortingPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
            return;
        }

        // -- Otherwise sibblings can only be filters.
        NXA_ASSERT_TRUE(objectType == RBSchema::Type::FilterPlaylist);
        foundAFilter = true;

        // -- If we are next to a filter playlist we just pass this to it and it will pass it accross to the sibblings.
        auto sibblingAsFilterPlaylist = FilterPlaylist(std::static_pointer_cast<PersistentFilterPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsFilterPlaylist.hasSortingCriteriaForTagTypeID(typeID));

        sibblingAsFilterPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
        return;
    }

    Playlist::removeAndDeleteSortingCriteriaForTagTypeID(typeID);
}

void OtherTracksPlaylist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String& description)
{
    boolean foundAFilter = false;

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        auto objectType = sibbling->objectType();
        if (objectType == RBSchema::Type::OtherTracksPlaylist) {
            // -- We don't want to add this to ourselves to we skip any OtherTracksPlaylists.
            continue;
        }
        else if (objectType == RBSchema::Type::SortingPlaylist) {
            // -- If we are next to a sorting playlist we just pass this to it and it will pass it up to the source playlist.
            auto sibblingAsSortingPlaylist = SortingPlaylist(std::static_pointer_cast<PersistentSortingPlaylist>(sibbling));
            NXA_ASSERT_FALSE(sibblingAsSortingPlaylist.hasSortingCriteriaForTrackTagWithDescription(description));
            NXA_ASSERT_FALSE(foundAFilter);

            sibblingAsSortingPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
            return;
        }

        // -- Otherwise sibblings can only be filters.
        NXA_ASSERT_TRUE(objectType == RBSchema::Type::FilterPlaylist);
        foundAFilter = true;

        // -- If we are next to a filter playlist we just pass this to it and it will pass it accross to the sibblings.
        auto sibblingAsFilterPlaylist = FilterPlaylist(std::static_pointer_cast<PersistentFilterPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsFilterPlaylist.hasSortingCriteriaForTrackTagWithDescription(description));

        sibblingAsFilterPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
        return;
    }

    Playlist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
}
