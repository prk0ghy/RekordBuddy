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
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentCueMarker.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentLoopMarker : public PersistentCueMarker
{
    using LocalType = PersistentLoopMarker;
    DecimalNumber lengthInSecondsAttribute;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentLoopMarker(RBSchema::Type type, Context context)
        : PersistentLoopMarker{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentLoopMarker>::value));}
    
public:
    PersistentLoopMarker(ObjectID id, Context context)
        : PersistentCueMarker{id, context}, lengthInSecondsAttribute{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentLoopMarker>::value));}
    ~PersistentLoopMarker() override = default;
    PersistentLoopMarker(const PersistentLoopMarker&) = delete;
    PersistentLoopMarker(PersistentLoopMarker&&) = default;
    PersistentLoopMarker& operator=(const PersistentLoopMarker&) = delete;
    
    
    template<typename T = PersistentLoopMarker>
        static Array<std::shared_ptr<T>> fetchByLengthInSeconds(Context context, DecimalNumber query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentLoopMarker>::value), "Must only query PersistentLoopMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentLoopMarker, T>("lengthInSeconds",
                                                                                      TypeEnumeration<T>::value,
                                                                                      &PersistentLoopMarker::lengthInSecondsAttribute,
                                                                                      nothing,
                                                                                      query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- lengthInSeconds
    DecimalNumber lengthInSeconds() const;
    void setLengthInSeconds(DecimalNumber lengthInSeconds);
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
