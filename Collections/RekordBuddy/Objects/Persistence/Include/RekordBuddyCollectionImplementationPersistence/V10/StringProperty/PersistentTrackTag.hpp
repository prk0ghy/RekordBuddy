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
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentTrackTag : public PersistentStringProperty
{
    using LocalType = PersistentTrackTag;
    friend class PersistentTrackTag;
    Optional<RBSchema::ObjectID> maybeParentTagOptionalId;
    friend class PersistentTrackTag;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrackTag>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyChildrenTags;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTrackTag(RBSchema::Type type, Context context)
        : PersistentTrackTag{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTrackTag>::value));}
    
public:
    PersistentTrackTag(ObjectID id, Context context)
        : PersistentStringProperty{id, context},
          maybeParentTagOptionalId{},
          manyChildrenTags{std::make_unique<decltype(manyChildrenTags)::element_type>(this, &PersistentTrackTag::loadChildrenTags)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTrackTag>::value));}
    ~PersistentTrackTag() override = default;
    PersistentTrackTag(const PersistentTrackTag&) = delete;
    PersistentTrackTag(PersistentTrackTag&&) = default;
    PersistentTrackTag& operator=(const PersistentTrackTag&) = delete;
    
    
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- maybeParentTag
    Optional<RBSchema::ObjectID> maybeParentTagObjectID() const;
    Optional<std::shared_ptr<PersistentTrackTag>> maybeParentTag() const;
    void setMaybeParentTag(Optional<std::shared_ptr<PersistentTrackTag>>);
    // -- childrenTags
    Array<typename RBSchema::ObjectID> childrenTagsObjectIDs() const;
    Set<typename RBSchema::ObjectID> childrenTagsObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrackTag>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& childrenTags();
    const PersistentRelationship<std::shared_ptr<PersistentTrackTag>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& childrenTagsConst() const;
    count loadChildrenTags(bool) const;
    typename RBSchema::ObjectID objectIDForChildrenTagsItemAtIndex(count) const;
    void addChildrenTagsItem(const std::shared_ptr<PersistentTrackTag>&);
    void removeChildrenTagsItem(const std::shared_ptr<PersistentTrackTag>&);
    void removeChildrenTagsItemAtIndex(count);
    void removeAllChildrenTags();
    void orderChildrenTagsItems(const Array<std::shared_ptr<PersistentTrackTag>>&, count);
    void orderChildrenTagsItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderChildrenTagsItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
