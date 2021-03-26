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
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentGridMarker : public PersistentMarker
{
    using LocalType = PersistentGridMarker;
    integer16 beatNumberAttribute;
    DecimalNumber beatsPerMinuteAttribute;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentGridMarker(RBSchema::Type type, Context context)
        : PersistentGridMarker{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentGridMarker>::value));}
    
public:
    PersistentGridMarker(ObjectID id, Context context)
        : PersistentMarker{id, context}, beatNumberAttribute{}, beatsPerMinuteAttribute{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentGridMarker>::value));}
    ~PersistentGridMarker() override = default;
    PersistentGridMarker(const PersistentGridMarker&) = delete;
    PersistentGridMarker(PersistentGridMarker&&) = default;
    PersistentGridMarker& operator=(const PersistentGridMarker&) = delete;
    
    
    template<typename T = PersistentGridMarker>
        static Array<std::shared_ptr<T>> fetchByBeatNumber(Context context, integer16 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentGridMarker>::value), "Must only query PersistentGridMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentGridMarker, T>("beatNumber",
                                                                                      TypeEnumeration<T>::value,
                                                                                      &PersistentGridMarker::beatNumberAttribute,
                                                                                      nothing,
                                                                                      query);
        }
    template<typename T = PersistentGridMarker>
        static Array<std::shared_ptr<T>> fetchByBeatsPerMinute(Context context, DecimalNumber query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentGridMarker>::value), "Must only query PersistentGridMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentGridMarker, T>("beatsPerMinute",
                                                                                      TypeEnumeration<T>::value,
                                                                                      &PersistentGridMarker::beatsPerMinuteAttribute,
                                                                                      nothing,
                                                                                      query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- beatNumber
    integer16 beatNumber() const;
    void setBeatNumber(integer16 beatNumber);
    
    // -- beatsPerMinute
    DecimalNumber beatsPerMinute() const;
    void setBeatsPerMinute(DecimalNumber beatsPerMinute);
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
