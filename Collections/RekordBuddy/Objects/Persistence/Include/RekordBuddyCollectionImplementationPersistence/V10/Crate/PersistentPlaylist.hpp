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
#include <RekordBuddyCollectionImplementationPersistence/RekordBuddySchema.hpp>
#include <Persistence/GenericPersistentObject.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentPlaylist : public PersistentCrate
{
    using LocalType = PersistentPlaylist;
    friend class PersistentSortingCriteria;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentSortingCriteria>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manySortedBy;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyTracks;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentPlaylist(RBSchema::Type type, Context context)
        : PersistentPlaylist{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentPlaylist>::value));}
    
public:
    PersistentPlaylist(ObjectID id, Context context)
        : PersistentCrate{id, context},
          manySortedBy{std::make_unique<decltype(manySortedBy)::element_type>(this, &PersistentPlaylist::loadSortedBy)},
          manyTracks{std::make_unique<decltype(manyTracks)::element_type>(this, &PersistentPlaylist::loadTracks)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentPlaylist>::value));}
    ~PersistentPlaylist() override = default;
    PersistentPlaylist(const PersistentPlaylist&) = delete;
    PersistentPlaylist(PersistentPlaylist&&) = default;
    PersistentPlaylist& operator=(const PersistentPlaylist&) = delete;
    
    
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- sortedBy
    Array<typename RBSchema::ObjectID> sortedByObjectIDs() const;
    Set<typename RBSchema::ObjectID> sortedByObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentSortingCriteria>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& sortedBy();
    const PersistentRelationship<std::shared_ptr<PersistentSortingCriteria>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& sortedByConst() const;
    count loadSortedBy(bool) const;
    typename RBSchema::ObjectID objectIDForSortedByItemAtIndex(count) const;
    void addSortedByItem(const std::shared_ptr<PersistentSortingCriteria>&);
    void removeSortedByItem(const std::shared_ptr<PersistentSortingCriteria>&);
    void removeSortedByItemAtIndex(count);
    void removeAllSortedBy();
    void orderSortedByItems(const Array<std::shared_ptr<PersistentSortingCriteria>>&, count);
    void orderSortedByItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderSortedByItemsAtIndex(const Set<count>&, count);
    
    
    // -- tracks
    Array<typename RBSchema::ObjectID> tracksObjectIDs() const;
    Set<typename RBSchema::ObjectID> tracksObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracks();
    const PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracksConst() const;
    count loadTracks(bool) const;
    typename RBSchema::ObjectID objectIDForTracksItemAtIndex(count) const;
    void addTracksItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksItemAtIndex(count);
    void removeAllTracks();
    void orderTracksItems(const Array<std::shared_ptr<PersistentTrack>>&, count);
    void orderTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderTracksItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
