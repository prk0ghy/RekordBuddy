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

#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Internal/FilterPlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        FilterPlaylist
#define NXA_OBJECT_BASE_CLASS                                   Playlist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

FilterPlaylist FilterPlaylist::withNamePredicateWithParent(const String& name,
                                                           const Common::TrackPredicate& predicate,
                                                           Playlist& parent)
{
    NXA_ASSERT_FALSE(parent.hasAnySortingCriteria());

    auto parentInternal = parent.persistentObject();
    auto newPersistentFilterPlaylist = parent.collection().asReference().persistentContext()->createObject<PersistentFilterPlaylist>();
    newPersistentFilterPlaylist->setName(name);
    newPersistentFilterPlaylist->setMaybePredicate(predicate.asBlob());

    parentInternal->addSubCratesItem(newPersistentFilterPlaylist);

    auto filterPlaylist = FilterPlaylist(std::move(newPersistentFilterPlaylist));
    filterPlaylist.updateSmartContent(Crate::AndUpdateClones::No);

    return filterPlaylist;
}

// -- Instance Methods

Optional<Common::TrackPredicate> FilterPlaylist::maybePredicate() const
{
    return nxa_internal->maybePredicate().maybe([](auto predicate)
    {
        return Common::TrackPredicate::fromBlob(std::move(predicate));
    });
}

void FilterPlaylist::setPredicate(const Optional<Common::TrackPredicate>& predicate)
{
    if (predicate.isValid()) {
        nxa_internal->setMaybePredicate(predicate->asBlob());
    }
    else {
        nxa_internal->setMaybePredicate(nothing);
    }

    this->updateSmartContent(Crate::AndUpdateClones::No);
}

void FilterPlaylist::addSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    Playlist::addSortingCriteriaForTagTypeID(typeID);

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        if (sibbling.get() == nxa_internal) {
            // -- We don't want to add this to ourselves again, just our sibblings.
            continue;
        }

        auto sibblingAsPlaylist = Playlist(std::static_pointer_cast<PersistentPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsPlaylist.hasSortingCriteriaForTagTypeID(typeID));
        sibblingAsPlaylist.addSortingCriteriaForTagTypeID(typeID);
    }
}

void FilterPlaylist::addSortingCriteriaForTrackTag(Common::MutableTag& tag)
{
    Playlist::addSortingCriteriaForTrackTag(tag);

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        if (sibbling.get() == nxa_internal) {
            // -- We don't want to add this to ourselves again, just our sibblings.
            continue;
        }

        auto sibblingAsPlaylist = Playlist(std::static_pointer_cast<PersistentPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsPlaylist.hasSortingCriteriaForTrackTagWithDescription(tag.value()));
        sibblingAsPlaylist.addSortingCriteriaForTrackTag(tag);
    }
}

void FilterPlaylist::removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID typeID)
{
    Playlist::removeAndDeleteSortingCriteriaForTagTypeID(typeID);

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        if (sibbling.get() == nxa_internal) {
            // -- We don't want to add this to ourselves again, just our sibblings.
            continue;
        }

        auto sibblingAsPlaylist = Playlist(std::static_pointer_cast<PersistentPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsPlaylist.hasSortingCriteriaForTagTypeID(typeID));
        sibblingAsPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
    }
}

void FilterPlaylist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String& description)
{
    Playlist::removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);

    auto maybeParentCrate = nxa_internal->maybeParentCrate();
    NXA_ASSERT_TRUE(maybeParentCrate.isValid());

    for (auto&& sibbling : (*maybeParentCrate)->subCrates()) {
        if (sibbling.get() == nxa_internal) {
            // -- We don't want to add this to ourselves again, just our sibblings.
            continue;
        }

        auto sibblingAsPlaylist = Playlist(std::static_pointer_cast<PersistentPlaylist>(sibbling));
        NXA_ASSERT_FALSE(sibblingAsPlaylist.hasSortingCriteriaForTrackTagWithDescription(description));
        sibblingAsPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(description);
    }
}
