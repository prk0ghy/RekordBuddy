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

#pragma once

#include <CommonCollection/Crates/Playlist.hpp>

#include <RekordBuddyCollection/Implementation/Crates/Crate.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Tag.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Common {

// -- Forward Declarations
class TrackPredicate;
class SmartPlaylist;
class MutableSmartPlaylist;

}

namespace RekordBuddy {

// -- Forward Declarations
class MutableCollection;
class MutableTrack;

namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                                        Playlist
#define NXA_OBJECT_BASE_CLASS                                   Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION
#include <Base/ObjectForwardDeclarations.ipp>

class Track;
class Folder;

// -- Public Interface
class Playlist : public NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

public:
    // -- Factory Methods
    static Playlist withNameAndParent(const String&, Folder&);

    // -- Class Methods
    static void updateSmartContentForPlaylistsInCollectionSortedByTagsOfType(Collection&,
                                                                             Common::Property::TypeID);

    // -- Instance Methods
    count numberOfTracks() const;
    Track trackAtIndex(count);
    count addTrackAtIndex(NotNull<RekordBuddy::MutableTrack*>, count);
    void removeTrackAtIndex(count);
    void removeAllTracks();
    Optional<count> maybeIndexForTrackObjectID(const ObjectID&) const;
    ObjectID trackObjectIDAtIndex(count) const;
    Array<ObjectID> trackObjectIDs() const;
    Set<ObjectID> uniqueAndUnorderedObjectIDs() const;
    Set<ObjectID> tracksMatchingPredicate(const Common::TrackPredicate&) const;
    void moveTrackAtIndexTo(count, count);
    void moveTracksAtIndicesToIndex(const Set<count>&, count);
    void moveTracksWithObjectIDsToIndex(const Array<ObjectID>&, count);

    boolean hasAnySortingCriteria() const;
    boolean hasSortingCriteriaForTagTypeID(Common::Property::TypeID) const;
    boolean hasSortingCriteriaForTrackTagWithDescription(const String&) const;

    void addSortingCriteriaForTagTypeID(Common::Property::TypeID);
    void addSortingCriteriaForTrackTag(Common::MutableTag&);

    void removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID);
    void removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String&);

    void updateSmartContent(Crate::AndUpdateClones = Crate::AndUpdateClones::Yes);
};

} } }
