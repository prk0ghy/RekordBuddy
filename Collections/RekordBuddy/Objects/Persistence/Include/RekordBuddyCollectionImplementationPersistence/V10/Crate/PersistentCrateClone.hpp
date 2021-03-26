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
class PersistentCrateClone : public PersistentCrate
{
    using LocalType = PersistentCrateClone;
    friend class PersistentCrate;
    RBSchema::ObjectID cloneOfId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentCrateClone(RBSchema::Type type, Context context)
        : PersistentCrateClone{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentCrateClone>::value));}
    
public:
    PersistentCrateClone(ObjectID id, Context context)
        : PersistentCrate{id, context}, cloneOfId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentCrateClone>::value));}
    ~PersistentCrateClone() override = default;
    PersistentCrateClone(const PersistentCrateClone&) = delete;
    PersistentCrateClone(PersistentCrateClone&&) = default;
    PersistentCrateClone& operator=(const PersistentCrateClone&) = delete;
    
    
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- cloneOf
    RBSchema::ObjectID cloneOfObjectID() const;
    std::shared_ptr<PersistentCrate> cloneOf() const;
    void setCloneOf(std::shared_ptr<PersistentCrate>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
