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

#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackProperties.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/StringPropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackTagInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        StringProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Instance Methods

String StringProperty::stringValue() const
{
    return nxa_const_internal->stringValue();
}

String StringProperty::sortingValue() const
{
    auto internal = nxa_const_internal;

    switch (internal->objectID().objectType()) {
        case RBSchema::Type::TrackTag: {
            return p_sortingValueForTrackTag(static_cast<const PersistentTrackTag&>(*internal));
        }
        case RBSchema::Type::TrackAlbum:
        case RBSchema::Type::TrackComments:
        case RBSchema::Type::TrackGenre:
        case RBSchema::Type::TrackGrouping:
        case RBSchema::Type::TrackMixName:
        case RBSchema::Type::TrackRecordLabel:
        case RBSchema::Type::TrackTitle:
        case RBSchema::Type::TrackDateAdded:
        case RBSchema::Type::TrackDateReleased: {
            return this->stringValue();
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid string property type %d.", internal->objectID().objectType());
        }
    }
}

Common::TrackPredicate StringProperty::sortingPredicate() const
{
    // -- TODO: These need to be case insensitive and diacritic insensitive.
    return Common::TrackPredicate::comparisonWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_const_internal->objectID().objectType()),
                                                  Common::TrackPredicate::ComparisonOperator::Equal,
                                                  this->sortingValue());
}

Array<Const<SortingPlaylist>> StringProperty::sortingPlaylists() const
{
    MutableArray<Const<SortingPlaylist>> result;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& sortingPlaylists = nxa_internal->sortingPlaylists();
    for (auto&& persistentSortingPlaylist : sortingPlaylists) {
        result.append(Const<SortingPlaylist>{ SortingPlaylist(persistentSortingPlaylist) });
    }

    return std::move(result);
}

Array<SortingPlaylist> StringProperty::sortingPlaylists()
{
    MutableArray<SortingPlaylist> result;
    for (auto&& persistentSortingPlaylist : nxa_internal->sortingPlaylists()) {
        result.append(SortingPlaylist(persistentSortingPlaylist));
    }

    return std::move(result);
}

Common::Property::TypeID StringProperty::typeID() const
{
    return RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_const_internal->objectID().objectType());
}

Array<ObjectID> StringProperty::parentObjectIDs() const
{
    return nxa_const_internal->parentTracksObjectIDs();
}

boolean StringProperty::isAnOrphan() const
{
    return p_tagIsAnOrphan<PersistentStringProperty>(*nxa_const_internal);
}

boolean StringProperty::shouldBeDeletedIfOrphan() const
{
    // -- By default, all tags are deleted when they are orphaned by track tags are kept around.
    return nxa_const_internal->objectID().objectType() != RBSchema::Type::TrackTag;
}

Pointer<const Collection> StringProperty::collection() const
{
    return { nxa_const_internal->getContext()->userData<Collection>() };
}

Pointer<Collection> StringProperty::collection()
{
    return { nxa_internal->getContext()->userData<Collection>() };
}

boolean StringProperty::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void StringProperty::deleteFromCollection()
{
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    switch (internal->objectID().objectType()) {
        case RBSchema::Type::TrackTag: {
            p_deleteTrackTagFromCollection(static_cast<PersistentTrackTag&>(*internal));
            break;
        }
        case RBSchema::Type::TrackAlbum:
        case RBSchema::Type::TrackComments:
        case RBSchema::Type::TrackGenre:
        case RBSchema::Type::TrackGrouping:
        case RBSchema::Type::TrackMixName:
        case RBSchema::Type::TrackRecordLabel:
        case RBSchema::Type::TrackTitle:
        case RBSchema::Type::TrackDateAdded:
        case RBSchema::Type::TrackDateReleased: {
            p_deleteStringPropertyFromCollection(*internal);
            break;
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid string property type %d.", internal->objectID().objectType());
        }
    }
}
