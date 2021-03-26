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
class PersistentMarker : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentMarker;
    integer64 flagsAttribute;
    DecimalNumber positionInSecondsAttribute;
    friend class PersistentTrack;
    RBSchema::ObjectID parentTrackId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentMarker(RBSchema::Type type, Context context)
        : PersistentMarker{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentMarker>::value));}
    
public:
    PersistentMarker(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context}, flagsAttribute{}, positionInSecondsAttribute{}, parentTrackId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentMarker>::value));}
    ~PersistentMarker() override = default;
    PersistentMarker(const PersistentMarker&) = delete;
    PersistentMarker(PersistentMarker&&) = default;
    PersistentMarker& operator=(const PersistentMarker&) = delete;
    
    
    template<typename T = PersistentMarker>
        static Array<std::shared_ptr<T>> fetchByFlags(Context context, integer64 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentMarker>::value), "Must only query PersistentMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentMarker, T>("flags", TypeEnumeration<T>::value, &PersistentMarker::flagsAttribute, nothing, query);
        }
    template<typename T = PersistentMarker>
        static Array<std::shared_ptr<T>> fetchByPositionInSeconds(Context context, DecimalNumber query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentMarker>::value), "Must only query PersistentMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentMarker, T>("positionInSeconds",
                                                                                  TypeEnumeration<T>::value,
                                                                                  &PersistentMarker::positionInSecondsAttribute,
                                                                                  nothing,
                                                                                  query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- flags
    integer64 flags() const;
    void setFlags(integer64 flags);
    
    // -- positionInSeconds
    DecimalNumber positionInSeconds() const;
    void setPositionInSeconds(DecimalNumber positionInSeconds);
    
    // -- parentTrack
    RBSchema::ObjectID parentTrackObjectID() const;
    std::shared_ptr<PersistentTrack> parentTrack() const;
    void setParentTrack(std::shared_ptr<PersistentTrack>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
