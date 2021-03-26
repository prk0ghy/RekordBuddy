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
class PersistentDateProperty : public PersistentStringProperty
{
    using LocalType = PersistentDateProperty;
    integer16 dayAttribute;
    integer16 monthAttribute;
    integer16 yearAttribute;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentDateProperty(RBSchema::Type type, Context context)
        : PersistentDateProperty{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentDateProperty>::value));}
    
public:
    PersistentDateProperty(ObjectID id, Context context)
        : PersistentStringProperty{id, context}, dayAttribute{}, monthAttribute{}, yearAttribute{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentDateProperty>::value));}
    ~PersistentDateProperty() override = default;
    PersistentDateProperty(const PersistentDateProperty&) = delete;
    PersistentDateProperty(PersistentDateProperty&&) = default;
    PersistentDateProperty& operator=(const PersistentDateProperty&) = delete;
    
    
    template<typename T = PersistentDateProperty>
        static Array<std::shared_ptr<T>> fetchByDay(Context context, integer16 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentDateProperty>::value),
                          "Must only query PersistentDateProperty or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentDateProperty, T>("day",
                                                                                        TypeEnumeration<T>::value,
                                                                                        &PersistentDateProperty::dayAttribute,
                                                                                        nothing,
                                                                                        query);
        }
    template<typename T = PersistentDateProperty>
        static Array<std::shared_ptr<T>> fetchByMonth(Context context, integer16 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentDateProperty>::value),
                          "Must only query PersistentDateProperty or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentDateProperty, T>("month",
                                                                                        TypeEnumeration<T>::value,
                                                                                        &PersistentDateProperty::monthAttribute,
                                                                                        nothing,
                                                                                        query);
        }
    template<typename T = PersistentDateProperty>
        static Array<std::shared_ptr<T>> fetchByYear(Context context, integer16 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentDateProperty>::value),
                          "Must only query PersistentDateProperty or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentDateProperty, T>("year",
                                                                                        TypeEnumeration<T>::value,
                                                                                        &PersistentDateProperty::yearAttribute,
                                                                                        nothing,
                                                                                        query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- day
    integer16 day() const;
    void setDay(integer16 day);
    
    // -- month
    integer16 month() const;
    void setMonth(integer16 month);
    
    // -- year
    integer16 year() const;
    void setYear(integer16 year);
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
