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

#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/StringProperty.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/DecimalProperty.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/IntegerProperty.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TimeProperty.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Operator Specialization

namespace NxA {

inline constexpr bool operator==(const PersistentSortingCriteria& first, const PersistentSortingCriteria& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const PersistentSortingCriteria& first, const PersistentSortingCriteria& second) noexcept
{
    return first.objectID() < second.objectID();
}

}

// -- Implementation

#include <Base/ResetObjectForwardDeclarations.ipp>

#define NXA_OBJECT_CLASS                                        SortingCriteria
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

SortingCriteria SortingCriteria::withTypeInCollection(Common::Property::TypeID typeID, Collection& collection)
{
    // -- Track tags should be tested using the other factory method.
    NXA_ASSERT_TRUE(typeID != Common::Property::TypeID::Tag);

    auto newCriteria = collection.persistentContext()->createObject<PersistentSortingCriteria>();
    newCriteria->setTagTypeBeingSorted(static_cast<integer32>(typeID));
    newCriteria->setSortingPropertyDescription(Common::Property::nameForType(typeID));
    newCriteria->setSortsABooleanTypeOfTag(false);

    return SortingCriteria{ std::move(newCriteria) };
}

SortingCriteria SortingCriteria::withTrackTagDescriptionInCollection(const String& propertyDescription,
                                                                     Collection& collection,
                                                                     SortsABooleanTypeOfTag sortsABooleanTypeOfTag)
{
    auto newCriteria = collection.persistentContext()->createObject<PersistentSortingCriteria>();
    newCriteria->setTagTypeBeingSorted(static_cast<integer32>(Common::Property::TypeID::Tag));
    newCriteria->setSortingPropertyDescription(propertyDescription);
    newCriteria->setSortsABooleanTypeOfTag(sortsABooleanTypeOfTag == SortsABooleanTypeOfTag::Yes);

    return SortingCriteria{ std::move(newCriteria) };
}

// -- Instance Methods

Common::Property::TypeID SortingCriteria::tagTypeBeingSorted() const
{
    return static_cast<Common::Property::TypeID>(nxa_const_internal->tagTypeBeingSorted());
}

boolean SortingCriteria::sortsABooleanTypeOfTag() const
{
    return nxa_const_internal->sortsABooleanTypeOfTag();
}

String SortingCriteria::sortingPropertyDescription() const
{
    return nxa_const_internal->sortingPropertyDescription();
}

Optional<Playlist> SortingCriteria::maybeParentPlaylist() const
{
    auto persistentParentPlaylist = nxa_const_internal->parentPlaylist();
    return persistentParentPlaylist.get() ? Optional<Playlist>(Playlist(std::move(persistentParentPlaylist))) : nothing;
}

Array<SortingPlaylist> SortingCriteria::sortingPlaylists()
{
    MutableArray<SortingPlaylist> result;

    for (auto&& persistentSortingPlaylist : nxa_internal->sortingPlaylists()) {
        result.append(SortingPlaylist(std::move(persistentSortingPlaylist)));
    }

    return std::move(result);
}

void SortingCriteria::setAscending(boolean ascending)
{
    nxa_internal->setAscending(ascending);
}

boolean SortingCriteria::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void SortingCriteria::deleteFromCollection()
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    if (internal->parentPlaylistObjectID().isValid()) {
        internal->parentPlaylist()->removeSortedByItem(*this);
    }

    auto sortingPlaylistCopy = internal->sortingPlaylists().objects();

    internal->deleteObject();

    for (auto&& sortingPlaylist : sortingPlaylistCopy) {
        SortingPlaylist(std::move(sortingPlaylist)).deleteFromCollection();
    }
}
