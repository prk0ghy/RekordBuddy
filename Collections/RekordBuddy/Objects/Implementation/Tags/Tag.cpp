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
#include <RekordBuddyCollection/Implementation/Tags/Tag.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Properties.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackProperties.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Playlist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>

// -- Private Functions

namespace NxA {  namespace RekordBuddy {  namespace CollectionImplementation {

inline void p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID typeID,
                                                         const RBSchema::RootContext &context,
                                                         MutableArray<Common::Property::TypeID> results)
{
    auto maybeObjectID = context->fetchFirstObjectIdOfType(
            RekordBuddy::CollectionImplementation::Property::persistentObjectTypeForPropertyTypeID(typeID));
    if (maybeObjectID.isValid()) {
        results.append(typeID);
    }
}

} }

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Class Methods

void Tag::updateSmartContentForCratesSortedByTagAndMarktItAsModifiedOn(Tag::OfSomeSort &tag,
                                                                       const Time &modificationTime) {
    auto sortingPlaylistsForTag = withVariant(tag, [](auto&& tag) {
        return tag.sortingPlaylists();
    });

    for (auto&& sortingPlaylist : sortingPlaylistsForTag) {
        sortingPlaylist.topParentPlaylist().updateSmartContent(Crate::AndUpdateClones::No);
    }
}

Array<Common::Property::TypeID> Tag::tagsTypesFoundInCollection(const Collection& collection)
{
    MutableArray<Common::Property::TypeID> results;

    auto context = collection.persistentContext().get();
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::MusicalKey, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Genre, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Comments, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::MixName, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::FileType, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::TrackNumber, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Rating, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::BitDepthInBits, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::SampleRateInHertz, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::FileSize, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::LengthInSeconds, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::BitDepthInBits, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::PlayCount, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::DateReleased, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::DateAdded, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::LastModifiedOn, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::RecordLabel, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Grouping, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Album, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::ArtistCredit, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::ProducerCredit, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::RemixerCredit, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::BeatGridLockedFlag, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::Color, context, results);
    p_ifAnyObjectsOfTagTypeInContextAddTagTypeTo(Common::Property::TypeID::BeatsPerMinute, context, results);

    return std::move(results);
}

}
