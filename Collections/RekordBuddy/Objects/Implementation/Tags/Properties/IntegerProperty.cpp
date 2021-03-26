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
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/IntegerPropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackFileTypeInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackBeatGridLockedFlagInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackBitRateInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackBitDepthInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackFileSizeInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackKeyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/Internal/TrackSampleRateInternal.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

#include <CommonCollection/Tracks/TrackRating.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        IntegerProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Instance Methods

String IntegerProperty::stringValue() const
{
    auto internal = nxa_const_internal;

    switch (internal->objectID().objectType()) {
        case RBSchema::Type::TrackFileType: {
            return p_stringValueForTrackFileType(static_cast<const PersistentTrackFileType&>(*internal));
        }
        case RBSchema::Type::TrackRating: {
            return static_cast<const TrackRating*>(this)->stringValue();
        }
        case RBSchema::Type::TrackKey: {
            return p_stringValueForTrackKey(static_cast<const PersistentTrackKey&>(*internal));
        }
        case RBSchema::Type::TrackBeatGridLockedFlag: {
            return p_stringValueForTrackBeatGrisLockedFlag(static_cast<const PersistentTrackBeatGridLockedFlag&>(*internal));
        }
        case RBSchema::Type::TrackBitDepth: {
            return p_stringValueForTrackBitDepth(static_cast<const PersistentTrackBitDepth&>(*internal));
        }
        case RBSchema::Type::TrackBitRate: {
            return p_stringValueForTrackBitRate(static_cast<const PersistentTrackBitRate&>(*internal));
        }
        case RBSchema::Type::TrackFileSize: {
            return p_stringValueForTrackFileSize(static_cast<const PersistentTrackFileSize&>(*internal));
        }
        case RBSchema::Type::TrackSampleRate: {
            return p_stringValueForTrackSampleRate(static_cast<const PersistentTrackSampleRate&>(*internal));
        }
        case RBSchema::Type::TrackColor:
        case RBSchema::Type::TrackNumber:
        case RBSchema::Type::TrackPlayCount: {
            return String::stringWithFormat("%llu", internal->integerValue());
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid string property type %d.", internal->objectID().objectType());
        }
    }
}

String IntegerProperty::sortingValue() const
{
    auto internal = nxa_const_internal;
    switch (internal->objectID().objectType()) {
        case RBSchema::Type::TrackKey: {
            return p_sortingValueForTrackKey(static_cast<const PersistentTrackKey&>(*internal));
        }
        case RBSchema::Type::TrackFileType:
        case RBSchema::Type::TrackRating:
        case RBSchema::Type::TrackBeatGridLockedFlag:
        case RBSchema::Type::TrackBitDepth:
        case RBSchema::Type::TrackBitRate:
        case RBSchema::Type::TrackFileSize:
        case RBSchema::Type::TrackLength:
        case RBSchema::Type::TrackSampleRate:
        case RBSchema::Type::TrackColor:
        case RBSchema::Type::TrackNumber:
        case RBSchema::Type::TrackPlayCount: {
            return this->stringValue();
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Invalid string property type %d.", internal->objectID().objectType());
        }
    }
}

Common::TrackPredicate IntegerProperty::sortingPredicate() const
{
    auto internal = nxa_const_internal;
    return Common::TrackPredicate::comparisonWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_internal->objectID().objectType()),
                                                  Common::TrackPredicate::ComparisonOperator::Equal,
                                                  (internal->objectID().objectType() == RBSchema::Type::TrackKey) ? String::stringWithFormat("%d", internal->integerValue()) : this->sortingValue());
}

Array<Const<SortingPlaylist>> IntegerProperty::sortingPlaylists() const
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

Array<SortingPlaylist> IntegerProperty::sortingPlaylists()
{
    MutableArray<SortingPlaylist> result;
    for (auto&& persistentSortingPlaylist : nxa_internal->sortingPlaylists()) {
        result.append(SortingPlaylist(persistentSortingPlaylist));
    }

    return std::move(result);
}

Common::Property::TypeID IntegerProperty::typeID() const
{
    return RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_const_internal->objectID().objectType());
}

Array<ObjectID> IntegerProperty::parentObjectIDs() const
{
    return nxa_const_internal->parentTracksObjectIDs();
}

boolean IntegerProperty::isAnOrphan() const
{
    return p_tagIsAnOrphan<PersistentIntegerProperty>(*nxa_const_internal);
}

boolean IntegerProperty::shouldBeDeletedIfOrphan() const
{
    // -- By default, all tags are deleted when they are orphaned.
    return true;
}

boolean IntegerProperty::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

Pointer<const Collection> IntegerProperty::collection() const
{
    return { nxa_const_internal->getContext()->userData<Collection>() };
}

Pointer<Collection> IntegerProperty::collection()
{
    return { nxa_internal->getContext()->userData<Collection>() };
}

void IntegerProperty::deleteFromCollection()
{
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    p_deleteIntegerPropertyFromCollection(*internal);
}
