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

#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentTimeProperty : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentTimeProperty;
    String sortingValueAttribute;
    Time timeValueAttribute;
    friend class PersistentSortingPlaylist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manySortingPlaylists;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyParentTracks;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTimeProperty(RBSchema::Type type, Context context)
        : PersistentTimeProperty{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTimeProperty>::value));}
    
public:
    PersistentTimeProperty(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          sortingValueAttribute{},
          timeValueAttribute{},
          manySortingPlaylists{std::make_unique<decltype(manySortingPlaylists)::element_type>(this, &PersistentTimeProperty::loadSortingPlaylists)},
          manyParentTracks{std::make_unique<decltype(manyParentTracks)::element_type>(this, &PersistentTimeProperty::loadParentTracks)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTimeProperty>::value));}
    ~PersistentTimeProperty() override = default;
    PersistentTimeProperty(const PersistentTimeProperty&) = delete;
    PersistentTimeProperty(PersistentTimeProperty&&) = default;
    PersistentTimeProperty& operator=(const PersistentTimeProperty&) = delete;
    
    
    template<typename T = PersistentTimeProperty>
        static Array<std::shared_ptr<T>> fetchBySortingValue(Context context, String query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTimeProperty>::value),
                          "Must only query PersistentTimeProperty or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTimeProperty, T>("sortingValue",
                                                                                        TypeEnumeration<T>::value,
                                                                                        &PersistentTimeProperty::sortingValueAttribute,
                                                                                        nothing,
                                                                                        query,
                                                                                        op);
        }
    template<typename T = PersistentTimeProperty>
        static Array<std::shared_ptr<T>> fetchByTimeValue(Context context, Time query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTimeProperty>::value),
                          "Must only query PersistentTimeProperty or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTimeProperty, T>("timeValue",
                                                                                        TypeEnumeration<T>::value,
                                                                                        &PersistentTimeProperty::timeValueAttribute,
                                                                                        nothing,
                                                                                        query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- sortingValue
    String sortingValue() const;
    void setSortingValue(String sortingValue);
    
    // -- timeValue
    Time timeValue() const;
    void setTimeValue(Time timeValue);
    
    // -- sortingPlaylists
    Array<typename RBSchema::ObjectID> sortingPlaylistsObjectIDs() const;
    Set<typename RBSchema::ObjectID> sortingPlaylistsObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& sortingPlaylists();
    const PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& sortingPlaylistsConst() const;
    count loadSortingPlaylists(bool) const;
    typename RBSchema::ObjectID objectIDForSortingPlaylistsItemAtIndex(count) const;
    void addSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>&);
    void removeSortingPlaylistsItem(const std::shared_ptr<PersistentSortingPlaylist>&);
    void removeSortingPlaylistsItemAtIndex(count);
    void removeAllSortingPlaylists();
    void orderSortingPlaylistsItems(const Array<std::shared_ptr<PersistentSortingPlaylist>>&, count);
    void orderSortingPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderSortingPlaylistsItemsAtIndex(const Set<count>&, count);
    
    
    // -- parentTracks
    Array<typename RBSchema::ObjectID> parentTracksObjectIDs() const;
    Set<typename RBSchema::ObjectID> parentTracksObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& parentTracks();
    const PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& parentTracksConst() const;
    count loadParentTracks(bool) const;
    typename RBSchema::ObjectID objectIDForParentTracksItemAtIndex(count) const;
    void addParentTracksItem(const std::shared_ptr<PersistentTrack>&);
    void removeParentTracksItem(const std::shared_ptr<PersistentTrack>&);
    void removeParentTracksItemAtIndex(count);
    void removeAllParentTracks();
    void orderParentTracksItems(const Array<std::shared_ptr<PersistentTrack>>&, count);
    void orderParentTracksItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderParentTracksItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
