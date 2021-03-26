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
class PersistentCrate : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentCrate;
    Time lastModifiedOnAttribute;
    String nameAttribute;
    friend class PersistentCrate;
    Optional<RBSchema::ObjectID> maybeParentCrateOptionalId;
    friend class PersistentCrate;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentCrate>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manySubCrates;
    friend class PersistentCrateClone;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentCrateClone>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyClones;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentCrate(RBSchema::Type type, Context context)
        : PersistentCrate{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentCrate>::value));}
    
public:
    PersistentCrate(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          lastModifiedOnAttribute{},
          nameAttribute{},
          maybeParentCrateOptionalId{},
          manySubCrates{std::make_unique<decltype(manySubCrates)::element_type>(this, &PersistentCrate::loadSubCrates)},
          manyClones{std::make_unique<decltype(manyClones)::element_type>(this, &PersistentCrate::loadClones)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentCrate>::value));}
    ~PersistentCrate() override = default;
    PersistentCrate(const PersistentCrate&) = delete;
    PersistentCrate(PersistentCrate&&) = default;
    PersistentCrate& operator=(const PersistentCrate&) = delete;
    
    
    template<typename T = PersistentCrate>
        static Array<std::shared_ptr<T>> fetchByLastModifiedOn(Context context, Time query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentCrate>::value), "Must only query PersistentCrate or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentCrate, T>("lastModifiedOn",
                                                                                 TypeEnumeration<T>::value,
                                                                                 &PersistentCrate::lastModifiedOnAttribute,
                                                                                 nothing,
                                                                                 query);
        }
    template<typename T = PersistentCrate>
        static Array<std::shared_ptr<T>> fetchByName(Context context, String query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentCrate>::value), "Must only query PersistentCrate or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentCrate, T>("name", TypeEnumeration<T>::value, &PersistentCrate::nameAttribute, nothing, query, op);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- lastModifiedOn
    Time lastModifiedOn() const;
    void setLastModifiedOn(Time lastModifiedOn);
    
    // -- name
    String name() const;
    void setName(String name);
    
    // -- maybeParentCrate
    Optional<RBSchema::ObjectID> maybeParentCrateObjectID() const;
    Optional<std::shared_ptr<PersistentCrate>> maybeParentCrate() const;
    void setMaybeParentCrate(Optional<std::shared_ptr<PersistentCrate>>);
    // -- subCrates
    Array<typename RBSchema::ObjectID> subCratesObjectIDs() const;
    Set<typename RBSchema::ObjectID> subCratesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentCrate>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& subCrates();
    const PersistentRelationship<std::shared_ptr<PersistentCrate>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& subCratesConst() const;
    count loadSubCrates(bool) const;
    typename RBSchema::ObjectID objectIDForSubCratesItemAtIndex(count) const;
    void addSubCratesItem(const std::shared_ptr<PersistentCrate>&);
    void removeSubCratesItem(const std::shared_ptr<PersistentCrate>&);
    void removeSubCratesItemAtIndex(count);
    void removeAllSubCrates();
    void orderSubCratesItems(const Array<std::shared_ptr<PersistentCrate>>&, count);
    void orderSubCratesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderSubCratesItemsAtIndex(const Set<count>&, count);
    
    
    // -- clones
    Array<typename RBSchema::ObjectID> clonesObjectIDs() const;
    Set<typename RBSchema::ObjectID> clonesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentCrateClone>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& clones();
    const PersistentRelationship<std::shared_ptr<PersistentCrateClone>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& clonesConst() const;
    count loadClones(bool) const;
    typename RBSchema::ObjectID objectIDForClonesItemAtIndex(count) const;
    void addClonesItem(const std::shared_ptr<PersistentCrateClone>&);
    void removeClonesItem(const std::shared_ptr<PersistentCrateClone>&);
    void removeClonesItemAtIndex(count);
    void removeAllClones();
    void orderClonesItems(const Array<std::shared_ptr<PersistentCrateClone>>&, count);
    void orderClonesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderClonesItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
