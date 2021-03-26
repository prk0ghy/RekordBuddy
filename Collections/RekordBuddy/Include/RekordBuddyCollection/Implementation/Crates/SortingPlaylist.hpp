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

#include <RekordBuddyCollection/Implementation/Tags/Tag.hpp>
#include <RekordBuddyCollection/Implementation/Crates/FilterPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Common {

// -- Forward Declarations
class TrackPredicate;

}

namespace RekordBuddy { namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                                        SortingPlaylist
#define NXA_OBJECT_BASE_CLASS                                   FilterPlaylist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION
#include <Base/ObjectForwardDeclarations.ipp>

class Collection;
class SortingCriteria;

// -- Public Interface
class SortingPlaylist : public NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

public:
    // -- Factory Methods
    static SortingPlaylist withNamePredicatePropertyCriteriaWithParent(const String&,
                                                                       const Common::TrackPredicate&,
                                                                       const Tag::OfSomeSort&,
                                                                       const SortingCriteria&,
                                                                       Playlist&);

    // -- Instance Methods
    Optional<Tag::OfSomeSort> maybePropertyUsedToSortBy();
    SortingCriteria sortingCriteria() const;
    Const<Playlist> topParentPlaylist() const;
    Playlist topParentPlaylist();

    boolean hasSortingCriteriaForTagTypeID(Common::Property::TypeID) const;
    boolean hasSortingCriteriaForTrackTagWithDescription(const String&) const;

    void addSortingCriteriaForTagTypeID(Common::Property::TypeID);
    void addSortingCriteriaForTrackTag(Common::MutableTag&);

    void removeAndDeleteSortingCriteriaForTagTypeID(Common::Property::TypeID);
    void removeAndDeleteSortingCriteriaForTrackTagWithDescription(const String&);
};
    
} } }
