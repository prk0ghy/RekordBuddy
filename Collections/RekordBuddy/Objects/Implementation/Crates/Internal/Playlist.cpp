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

#include <RekordBuddyCollection/Implementation/Crates/Internal/PlaylistInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Properties.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

void p_updatePlaylistSmartContent(Playlist& playlist, PersistentPlaylist& persistentPlaylist, Crate::AndUpdateClones andUpdateClones)
{
    boolean hadSortingCrates = p_crateHasSortingCrates(persistentPlaylist);
    boolean hadFilters = false;

    // -- We need to disconnect the criterias from their sorting playlists so that they don't get deleted
    // -- when the sorting playlists are deleted below.
    auto& persistentCriterias = persistentPlaylist.sortedBy();
    for (auto&& criteria : persistentCriterias) {
        for (auto&& sortingPlaylist : criteria->sortingPlaylists().objects()) {
            sortingPlaylist->setCriteria(nullptr);
        }
    }

    // -- Then we remove any old sorting playlists.
    for (auto&& persistentSubCrate : persistentPlaylist.subCrates().objects()) {
        auto type = persistentSubCrate->objectType();
        if (RBSchema::typeIs(type, RBSchema::Type::SortingPlaylist) ||
            RBSchema::typeIs(type, RBSchema::Type::OtherTracksPlaylist)) {
            Crate(std::move(persistentSubCrate)).deleteFromCollection();
        }
        else if (p_crateIsAFilter(*persistentSubCrate)) {
            auto subCrate = Playlist(std::static_pointer_cast<PersistentPlaylist>(persistentSubCrate));
            subCrate.updateSmartContent(Crate::AndUpdateClones::No);

            hadFilters = true;
        }
    }

    // -- And we add the new ones.
    if (persistentCriterias.length()) {
        MutableArray<SortingCriteria> criterias;
        for (auto&& persistentCriteria : persistentCriterias) {
            criterias.append(SortingCriteria(persistentCriteria));
        }

        p_sortPlaylistWithCriterias(playlist, persistentPlaylist, { std::move(criterias) });
    }
    else if (hadFilters) {
        OtherTracksPlaylist::addOrUpdateOtherTracksPlaylistNamedIn(String("Other Tracks"), playlist);
    }

    auto maybeParentCrate = persistentPlaylist.maybeParentCrate();
    if (maybeParentCrate.isValid() &&
        ((hadSortingCrates != p_crateHasSortingCrates(persistentPlaylist)) ||
         (hadFilters != p_crateHasFilters(persistentPlaylist)))) {
        // -- If this playlist's situation regarding sub-sorting or sub-filtering crates has changed, this means it's type has changed for
        // -- destination parsers (i.e. this might have been exported as a playlist but now needs to be exported as a folder, and vice versa)
        // -- so we mark the parent as modified so it gets re-exported.
        Crate(std::move(*maybeParentCrate)).markAsModifiedNow();
    }

    if (andUpdateClones == Crate::AndUpdateClones::Yes) {
        for (auto&& persistentClone : persistentPlaylist.clones()) {
            auto clone = Playlist(std::static_pointer_cast<PersistentPlaylist>(persistentClone->cloneOf()));
            clone.updateSmartContent(Crate::AndUpdateClones::No);
        }
    }
}

void p_sortPlaylistWithCriteria(Playlist& playlist, const SortingCriteria& criteria)
{
    auto typeID = criteria.tagTypeBeingSorted();

    MutableMap<String, Tag::OfSomeSort> tagsFoundBySortingValue;
    switch (Common::Property::valueTypeIDFromPropertyType(typeID)) {
        case Common::Property::StringValueType: {
            for (count index = 0; index < playlist.numberOfTracks(); ++index) {
                p_addTagsFoundOfTypeInTrackTo<StringProperty>(typeID, playlist.trackAtIndex(index), criteria.sortingPropertyDescription(), tagsFoundBySortingValue);
            }
            break;
        }
        case Common::Property::IntegerValueType: {
            for (count index = 0; index < playlist.numberOfTracks(); ++index) {
                p_addTagsFoundOfTypeInTrackTo<IntegerProperty>(typeID, playlist.trackAtIndex(index), criteria.sortingPropertyDescription(), tagsFoundBySortingValue);
            }
            break;
        }
        case Common::Property::DecimalValueType: {
            for (count index = 0; index < playlist.numberOfTracks(); ++index) {
                p_addTagsFoundOfTypeInTrackTo<DecimalProperty>(typeID, playlist.trackAtIndex(index), criteria.sortingPropertyDescription(), tagsFoundBySortingValue);
            }
            break;
        }
        case Common::Property::TimeValueType: {
            for (count index = 0; index < playlist.numberOfTracks(); ++index) {
                p_addTagsFoundOfTypeInTrackTo<TimeProperty>(typeID, playlist.trackAtIndex(index), criteria.sortingPropertyDescription(), tagsFoundBySortingValue);
            }
            break;
        }
        default: {
            NXA_ALOG_WITH_FORMAT("Illegal tag type %hhu", typeID);
        }
    }

    // -- We sort all the buckets found by description.
    auto sortedKeys = MutableArray<String>(tagsFoundBySortingValue.allKeys());
    sortedKeys.sortWith([](String& first, String& second) {
        // -- TODO: This should be replaced by an equivalent method to localizedStandardCompare: in ObjC.
        return first < second;
    });

    MutableArray<SortingPlaylist> emptyResults;

    for (auto&& sortingValue : sortedKeys) {
        withVariant(tagsFoundBySortingValue[sortingValue], [&criteria, &emptyResults, &playlist](auto&& property) {
            auto newSortingPlaylist = SortingPlaylist::withNamePredicatePropertyCriteriaWithParent(property.sortingValue(),
                                                                                                   property.sortingPredicate(),
                                                                                                   property,
                                                                                                   criteria,
                                                                                                   playlist);
            if (!newSortingPlaylist.numberOfTracks()) {
                emptyResults.append(std::move(newSortingPlaylist));
            }
        });
    }

    for (auto&& sortingPlaylist : emptyResults) {
        sortingPlaylist.deleteFromCollection();
    }
}

void p_sortPlaylistWithCriterias(Playlist& playlist, PersistentPlaylist& persistentPlaylist, Array<SortingCriteria> criterias)
{
    NXA_ASSERT_TRUE(criterias.length() > 0);

    auto sortingCriteria = criterias.firstObject();
    p_sortPlaylistWithCriteria(playlist, sortingCriteria);

    auto format = String(sortingCriteria.sortsABooleanTypeOfTag() ? "Not %s" : "No %s");
    auto otherTracksPlaylistName = String::stringWithFormat(format, sortingCriteria.sortingPropertyDescription().asUTF8());
    OtherTracksPlaylist::addOrUpdateOtherTracksPlaylistNamedIn(otherTracksPlaylistName, playlist);

    if (criterias.length() > 1) {
        auto subCriterias = criterias.subArrayWithRange(1, criterias.length() - 1);
        for (auto&& persistentSubCrate : persistentPlaylist.subCrates()) {
            NXA_ASSERT_TRUE(RekordBuddy::CollectionImplementation::RBSchema::typeIs(persistentSubCrate->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::FilterPlaylist) ||
                            RekordBuddy::CollectionImplementation::RBSchema::typeIs(persistentSubCrate->objectType(), RekordBuddy::CollectionImplementation::RBSchema::Type::OtherTracksPlaylist));
            auto asPersistentPlaylist = std::static_pointer_cast<PersistentPlaylist>(persistentSubCrate);
            auto subCrate = Playlist(asPersistentPlaylist);
            p_sortPlaylistWithCriterias(subCrate, *asPersistentPlaylist, subCriterias);
        }
    }
}

void p_deletePlaylistFromCollection(PersistentPlaylist& playlist)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto criteriasCopy = playlist.sortedBy().objects();

    p_deleteCrateFromCollection(playlist);

    for (auto&& persistentCriteria : criteriasCopy) {
        SortingCriteria(std::move(persistentCriteria)).deleteFromCollection();
    }
}

// -- Template Specialization

template<>
    void p_addTagsFoundOfTypeInTrackTo<StringProperty>(Common::Property::TypeID typeID,
                                                       Track track,
                                                       String propertyDescription,
                                                       MutableMap<String, Tag::OfSomeSort>& tagsFoundBySortingValue)
    {
        // -- We look at the track's tags which match the sorting criteria.
        auto matchingTags = track.tagsOfBaseTypeWithTypeID<StringProperty>(typeID);
        if (typeID == Common::Property::TypeID::Tag) {
            if (matchingTags.length()) {
                MutableArray<StringProperty> filteredTags;

                for (auto&& tag : matchingTags) {
                    auto& tagAsTrackTag = static_cast<TrackTag&>(tag);
                    if (tagAsTrackTag.propertyDescription() == propertyDescription) {
                        filteredTags.append(tagAsTrackTag);
                    }
                }

                matchingTags = filteredTags;
            }
        }

        // -- If we have tags that match, we go through each different SORTING value found for the tag.
        for (auto&& tag : matchingTags) {
            tagsFoundBySortingValue.setValueForKey(tag, tag.sortingValue());
        }
    }

} } }
