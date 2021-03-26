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
class PersistentIntegerProperty : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentIntegerProperty;
    integer32 integerValueAttribute;
    friend class PersistentSortingPlaylist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manySortingPlaylists;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyParentTracks;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentIntegerProperty(RBSchema::Type type, Context context)
        : PersistentIntegerProperty{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentIntegerProperty>::value));}
    
public:
    PersistentIntegerProperty(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          integerValueAttribute{},
          manySortingPlaylists{std::make_unique<decltype(manySortingPlaylists)::element_type>(this, &PersistentIntegerProperty::loadSortingPlaylists)},
          manyParentTracks{std::make_unique<decltype(manyParentTracks)::element_type>(this, &PersistentIntegerProperty::loadParentTracks)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentIntegerProperty>::value));}
    ~PersistentIntegerProperty() override = default;
    PersistentIntegerProperty(const PersistentIntegerProperty&) = delete;
    PersistentIntegerProperty(PersistentIntegerProperty&&) = default;
    PersistentIntegerProperty& operator=(const PersistentIntegerProperty&) = delete;
    
    
    template<typename T = PersistentIntegerProperty>
        static Array<std::shared_ptr<T>> fetchByIntegerValue(Context context, integer32 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentIntegerProperty>::value),
                          "Must only query PersistentIntegerProperty or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentIntegerProperty, T>("integerValue",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentIntegerProperty::integerValueAttribute,
                                                                                           nothing,
                                                                                           query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- integerValue
    integer32 integerValue() const;
    void setIntegerValue(integer32 integerValue);
    
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
