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
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentIntegerProperty.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentTrackColor : public PersistentIntegerProperty
{
    using LocalType = PersistentTrackColor;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTrackColor(RBSchema::Type type, Context context)
        : PersistentTrackColor{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTrackColor>::value));}
    
public:
    PersistentTrackColor(ObjectID id, Context context)
        : PersistentIntegerProperty{id, context}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTrackColor>::value));}
    ~PersistentTrackColor() override = default;
    PersistentTrackColor(const PersistentTrackColor&) = delete;
    PersistentTrackColor(PersistentTrackColor&&) = default;
    PersistentTrackColor& operator=(const PersistentTrackColor&) = delete;
    
    
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
