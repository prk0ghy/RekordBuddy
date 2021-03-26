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

#include <RekordBuddyCollection/Implementation/Tags/Tags.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/SortingPlaylistInternal.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        SortingPlaylist
#define NXA_OBJECT_BASE_CLASS                                   FilterPlaylist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

SortingPlaylist SortingPlaylist::withNamePredicatePropertyCriteriaWithParent(const String& name,
                                                                             const Common::TrackPredicate& predicate,
                                                                             const Tag::OfSomeSort& property,
                                                                             const SortingCriteria& criteria,
                                                                             Playlist& parent)
{
    auto parentInternal = std::static_pointer_cast<PersistentCrate>(parent.persistentObject());

    NXA_ASSERT_TRUE(p_sourcePlaylistFor(parentInternal).hasAnySortingCriteria());
    NXA_ASSERT_FALSE(p_crateHasFilters(*parentInternal));

    auto newPersistentSortingPlaylist = parent.collection().asReference().persistentContext()->createObject<PersistentSortingPlaylist>();
    newPersistentSortingPlaylist->setName(name);
    newPersistentSortingPlaylist->setMaybePredicate(predicate.asBlob());
    newPersistentSortingPlaylist->setCriteria(criteria.persistentObject());

    withVariant(property, [&newPersistentSortingPlaylist](auto&& property) {
        property.persistentObject()->addSortingPlaylistsItem(newPersistentSortingPlaylist);
    });

    parentInternal->addSubCratesItem(newPersistentSortingPlaylist);

    auto sortingPlaylist = SortingPlaylist(std::move(newPersistentSortingPlaylist));
    sortingPlaylist.updateSmartContent(Crate::AndUpdateClones::No);

    return sortingPlaylist;
}

// -- Instance Methods

Optional<Tag::OfSomeSort> SortingPlaylist::maybePropertyUsedToSortBy()
{
    auto internal = nxa_internal;

    auto maybeStringProperty = internal->maybeStringPropertyUsedToSortBy();
    if (maybeStringProperty.isValid()) {
        return { StringProperty(std::move(*maybeStringProperty)) };
    }

    auto maybeIntegerProperty = internal->maybeIntegerPropertyUsedToSortBy();
    if (maybeIntegerProperty.isValid()) {
        return { IntegerProperty(std::move(*maybeIntegerProperty)) };
    }

    auto maybeDecimalProperty = internal->maybeDecimalPropertyUsedToSortBy();
    if (maybeDecimalProperty.isValid()) {
        return { DecimalProperty(std::move(*maybeDecimalProperty)) };
    }

    auto maybeTimeProperty = internal->maybeTimePropertyUsedToSortBy();
    if (maybeTimeProperty.isValid()) {
        return { TimeProperty(std::move(*maybeTimeProperty)) };
    }

    return nothing;
}

SortingCriteria SortingPlaylist::sortingCriteria() const
{
    return SortingCriteria(nxa_const_internal->criteria());
}

Const<Playlist> SortingPlaylist::topParentPlaylist() const
{
    return { p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this)) };
}

Playlist SortingPlaylist::topParentPlaylist()
{
    return p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
}

boolean SortingPlaylist::hasSortingCriteriaForTagTypeID(Common::Property::TypeID tagTypeID) const
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    return topParentPlaylist.hasSortingCriteriaForTagTypeID(tagTypeID);
}

boolean SortingPlaylist::hasSortingCriteriaForTrackTagWithDescription(const String& description) const
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    return topParentPlaylist.hasSortingCriteriaForTrackTagWithDescription(description);
}

void SortingPlaylist::addSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    topParentPlaylist.addSortingCriteriaForTagTypeID(typeID);
}

void SortingPlaylist::addSortingCriteriaForTrackTag(Common::MutableTag& tag)
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    topParentPlaylist.addSortingCriteriaForTrackTag(tag);
}

void SortingPlaylist::removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    topParentPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
}

void SortingPlaylist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String& description)
{
    auto topParentPlaylist = p_topParentPlaylistFor(std::static_pointer_cast<PersistentSortingPlaylist>(*this));
    topParentPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
}
