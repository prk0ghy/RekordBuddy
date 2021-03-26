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
class PersistentCueMarker : public PersistentMarker
{
    using LocalType = PersistentCueMarker;
    Optional<integer16> maybeHotCueNumberAttribute;
    Optional<String> maybeNameAttribute;
    Optional<integer32> maybeRgbColorAttribute;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentCueMarker(RBSchema::Type type, Context context)
        : PersistentCueMarker{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentCueMarker>::value));}
    
public:
    PersistentCueMarker(ObjectID id, Context context)
        : PersistentMarker{id, context}, maybeHotCueNumberAttribute{}, maybeNameAttribute{}, maybeRgbColorAttribute{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentCueMarker>::value));}
    ~PersistentCueMarker() override = default;
    PersistentCueMarker(const PersistentCueMarker&) = delete;
    PersistentCueMarker(PersistentCueMarker&&) = default;
    PersistentCueMarker& operator=(const PersistentCueMarker&) = delete;
    
    
    template<typename T = PersistentCueMarker>
        static Array<std::shared_ptr<T>> fetchByMaybeHotCueNumber(Context context, Optional<integer16> query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentCueMarker>::value), "Must only query PersistentCueMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentCueMarker, T>("maybeHotCueNumber",
                                                                                     TypeEnumeration<T>::value,
                                                                                     &PersistentCueMarker::maybeHotCueNumberAttribute,
                                                                                     nothing,
                                                                                     {query});
        }
    template<typename T = PersistentCueMarker>
        static Array<std::shared_ptr<T>> fetchByMaybeName(Context context, Optional<String> query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentCueMarker>::value), "Must only query PersistentCueMarker or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentCueMarker, T>("maybeName",
                                                                                     TypeEnumeration<T>::value,
                                                                                     &PersistentCueMarker::maybeNameAttribute,
                                                                                     nothing,
                                                                                     {query},
                                                                                     op);
        }
    template<typename T = PersistentCueMarker>
        static Array<std::shared_ptr<T>> fetchByMaybeRgbColor(Context context, Optional<integer32> query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentCueMarker>::value), "Must only query PersistentCueMarker or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentCueMarker, T>("maybeRgbColor",
                                                                                     TypeEnumeration<T>::value,
                                                                                     &PersistentCueMarker::maybeRgbColorAttribute,
                                                                                     nothing,
                                                                                     {query});
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- maybeHotCueNumber
    Optional<integer16> maybeHotCueNumber() const;
    void setMaybeHotCueNumber(Optional<integer16> maybeHotCueNumber);
    
    // -- maybeName
    Optional<String> maybeName() const;
    void setMaybeName(Optional<String> maybeName);
    
    // -- maybeRgbColor
    Optional<integer32> maybeRgbColor() const;
    void setMaybeRgbColor(Optional<integer32> maybeRgbColor);
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
