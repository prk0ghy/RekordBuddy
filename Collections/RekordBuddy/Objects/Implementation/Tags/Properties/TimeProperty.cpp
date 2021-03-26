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
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/TimePropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackModificationTime.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        TimeProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Instance Methods

String TimeProperty::stringValue() const
{
    return nxa_const_internal->timeValue().stringValueInLocalTimeZoneUsingFormat(Time::defaultStringFormat);
}

String TimeProperty::sortingValue() const
{
    return nxa_const_internal->timeValue().stringValueInLocalTimeZoneUsingFormat(Time::defaultStringFormatWithJustDay);
}

Common::TrackPredicate TimeProperty::sortingPredicate() const
{
    return Common::TrackPredicate::comparisonWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_const_internal->objectID().objectType()),
                                                  Common::TrackPredicate::ComparisonOperator::Equal,
                                                  this->sortingValue());
}

Array<Const<SortingPlaylist>> TimeProperty::sortingPlaylists() const
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

Array<SortingPlaylist> TimeProperty::sortingPlaylists()
{
    MutableArray<SortingPlaylist> result;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& sortingPlaylists = nxa_internal->sortingPlaylists();
    for (auto&& persistentSortingPlaylist : sortingPlaylists) {
        result.append(SortingPlaylist(persistentSortingPlaylist));
    }

    return std::move(result);
}

Common::Property::TypeID TimeProperty::typeID() const
{
    return RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(nxa_const_internal->objectID().objectType());
}

Array<ObjectID> TimeProperty::parentObjectIDs() const
{
    return nxa_const_internal->parentTracksObjectIDs();
}

boolean TimeProperty::isAnOrphan() const
{
    return p_tagIsAnOrphan<PersistentTimeProperty>(*nxa_internal);
}

boolean TimeProperty::shouldBeDeletedIfOrphan() const
{
    // -- By default, all tags are deleted when they are orphaned.
    return true;
}

boolean TimeProperty::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

Pointer<const Collection> TimeProperty::collection() const
{
    return { nxa_const_internal->getContext()->userData<Collection>() };
}

Pointer<Collection> TimeProperty::collection()
{
    return { nxa_internal->getContext()->userData<Collection>() };
}

void TimeProperty::deleteFromCollection()
{
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    p_deleteTimePropertyFromCollection(*internal);
}
