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
class PersistentSortingCriteria : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentSortingCriteria;
    boolean ascendingAttribute;
    boolean shouldOnlySortTagsWithSamePropertyDescriptionAttribute;
    String sortingPropertyDescriptionAttribute;
    boolean sortsABooleanTypeOfTagAttribute;
    integer32 tagTypeBeingSortedAttribute;
    friend class PersistentPlaylist;
    RBSchema::ObjectID parentPlaylistId;
    friend class PersistentSortingPlaylist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentSortingPlaylist>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manySortingPlaylists;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentSortingCriteria(RBSchema::Type type, Context context)
        : PersistentSortingCriteria{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentSortingCriteria>::value));}
    
public:
    PersistentSortingCriteria(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          ascendingAttribute{},
          shouldOnlySortTagsWithSamePropertyDescriptionAttribute{},
          sortingPropertyDescriptionAttribute{},
          sortsABooleanTypeOfTagAttribute{},
          tagTypeBeingSortedAttribute{},
          parentPlaylistId{},
          manySortingPlaylists{std::make_unique<decltype(manySortingPlaylists)::element_type>(this, &PersistentSortingCriteria::loadSortingPlaylists)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentSortingCriteria>::value));}
    ~PersistentSortingCriteria() override = default;
    PersistentSortingCriteria(const PersistentSortingCriteria&) = delete;
    PersistentSortingCriteria(PersistentSortingCriteria&&) = default;
    PersistentSortingCriteria& operator=(const PersistentSortingCriteria&) = delete;
    
    
    template<typename T = PersistentSortingCriteria>
        static Array<std::shared_ptr<T>> fetchByAscending(Context context, boolean query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentSortingCriteria>::value),
                          "Must only query PersistentSortingCriteria or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentSortingCriteria, T>("ascending",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentSortingCriteria::ascendingAttribute,
                                                                                           nothing,
                                                                                           query);
        }
    template<typename T = PersistentSortingCriteria>
        static Array<std::shared_ptr<T>> fetchByShouldOnlySortTagsWithSamePropertyDescription(Context context, boolean query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentSortingCriteria>::value),
                          "Must only query PersistentSortingCriteria or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentSortingCriteria, T>("shouldOnlySortTagsWithSamePropertyDescription",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentSortingCriteria::shouldOnlySortTagsWithSamePropertyDescriptionAttribute,
                                                                                           nothing,
                                                                                           query);
        }
    template<typename T = PersistentSortingCriteria>
        static Array<std::shared_ptr<T>> fetchBySortingPropertyDescription(Context context, String query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentSortingCriteria>::value),
                          "Must only query PersistentSortingCriteria or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentSortingCriteria, T>("sortingPropertyDescription",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentSortingCriteria::sortingPropertyDescriptionAttribute,
                                                                                           nothing,
                                                                                           query,
                                                                                           op);
        }
    template<typename T = PersistentSortingCriteria>
        static Array<std::shared_ptr<T>> fetchBySortsABooleanTypeOfTag(Context context, boolean query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentSortingCriteria>::value),
                          "Must only query PersistentSortingCriteria or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentSortingCriteria, T>("sortsABooleanTypeOfTag",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentSortingCriteria::sortsABooleanTypeOfTagAttribute,
                                                                                           nothing,
                                                                                           query);
        }
    template<typename T = PersistentSortingCriteria>
        static Array<std::shared_ptr<T>> fetchByTagTypeBeingSorted(Context context, integer32 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentSortingCriteria>::value),
                          "Must only query PersistentSortingCriteria or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentSortingCriteria, T>("tagTypeBeingSorted",
                                                                                           TypeEnumeration<T>::value,
                                                                                           &PersistentSortingCriteria::tagTypeBeingSortedAttribute,
                                                                                           nothing,
                                                                                           query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- ascending
    boolean ascending() const;
    void setAscending(boolean ascending);
    
    // -- shouldOnlySortTagsWithSamePropertyDescription
    boolean shouldOnlySortTagsWithSamePropertyDescription() const;
    void setShouldOnlySortTagsWithSamePropertyDescription(boolean shouldOnlySortTagsWithSamePropertyDescription);
    
    // -- sortingPropertyDescription
    String sortingPropertyDescription() const;
    void setSortingPropertyDescription(String sortingPropertyDescription);
    
    // -- sortsABooleanTypeOfTag
    boolean sortsABooleanTypeOfTag() const;
    void setSortsABooleanTypeOfTag(boolean sortsABooleanTypeOfTag);
    
    // -- tagTypeBeingSorted
    integer32 tagTypeBeingSorted() const;
    void setTagTypeBeingSorted(integer32 tagTypeBeingSorted);
    
    // -- parentPlaylist
    RBSchema::ObjectID parentPlaylistObjectID() const;
    std::shared_ptr<PersistentPlaylist> parentPlaylist() const;
    void setParentPlaylist(std::shared_ptr<PersistentPlaylist>);
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
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
