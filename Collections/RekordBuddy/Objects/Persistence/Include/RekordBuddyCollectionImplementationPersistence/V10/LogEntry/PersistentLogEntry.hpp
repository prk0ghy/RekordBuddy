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
class PersistentLogEntry : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentLogEntry;
    integer16 idAttribute;
    Optional<String> maybeMessageAttribute;
    Time timeStampAttribute;
    friend class PersistentTrack;
    Optional<RBSchema::ObjectID> maybeRelatedToTrackOptionalId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentLogEntry(RBSchema::Type type, Context context)
        : PersistentLogEntry{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentLogEntry>::value));}
    
public:
    PersistentLogEntry(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context}, idAttribute{}, maybeMessageAttribute{}, timeStampAttribute{}, maybeRelatedToTrackOptionalId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentLogEntry>::value));}
    ~PersistentLogEntry() override = default;
    PersistentLogEntry(const PersistentLogEntry&) = delete;
    PersistentLogEntry(PersistentLogEntry&&) = default;
    PersistentLogEntry& operator=(const PersistentLogEntry&) = delete;
    
    
    template<typename T = PersistentLogEntry>
        static Array<std::shared_ptr<T>> fetchById(Context context, integer16 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentLogEntry>::value), "Must only query PersistentLogEntry or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentLogEntry, T>("id", TypeEnumeration<T>::value, &PersistentLogEntry::idAttribute, nothing, query);
        }
    template<typename T = PersistentLogEntry>
        static Array<std::shared_ptr<T>> fetchByMaybeMessage(Context context, Optional<String> query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentLogEntry>::value), "Must only query PersistentLogEntry or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentLogEntry, T>("maybeMessage",
                                                                                    TypeEnumeration<T>::value,
                                                                                    &PersistentLogEntry::maybeMessageAttribute,
                                                                                    nothing,
                                                                                    {query},
                                                                                    op);
        }
    template<typename T = PersistentLogEntry>
        static Array<std::shared_ptr<T>> fetchByTimeStamp(Context context, Time query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentLogEntry>::value), "Must only query PersistentLogEntry or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentLogEntry, T>("timeStamp",
                                                                                    TypeEnumeration<T>::value,
                                                                                    &PersistentLogEntry::timeStampAttribute,
                                                                                    nothing,
                                                                                    query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- id
    integer16 id() const;
    void setId(integer16 id);
    
    // -- maybeMessage
    Optional<String> maybeMessage() const;
    void setMaybeMessage(Optional<String> maybeMessage);
    
    // -- timeStamp
    Time timeStamp() const;
    void setTimeStamp(Time timeStamp);
    
    // -- maybeRelatedToTrack
    Optional<RBSchema::ObjectID> maybeRelatedToTrackObjectID() const;
    Optional<std::shared_ptr<PersistentTrack>> maybeRelatedToTrack() const;
    void setMaybeRelatedToTrack(Optional<std::shared_ptr<PersistentTrack>>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
