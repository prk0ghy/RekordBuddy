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
class PersistentMarkerImportOffset : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentMarkerImportOffset;
    integer32 collectionTypeIDAttribute;
    DecimalNumber offsetToAddInSecondsAttribute;
    friend class PersistentTrack;
    RBSchema::ObjectID parentTrackId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentMarkerImportOffset(RBSchema::Type type, Context context)
        : PersistentMarkerImportOffset{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentMarkerImportOffset>::value));}
    
public:
    PersistentMarkerImportOffset(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context}, collectionTypeIDAttribute{}, offsetToAddInSecondsAttribute{}, parentTrackId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentMarkerImportOffset>::value));}
    ~PersistentMarkerImportOffset() override = default;
    PersistentMarkerImportOffset(const PersistentMarkerImportOffset&) = delete;
    PersistentMarkerImportOffset(PersistentMarkerImportOffset&&) = default;
    PersistentMarkerImportOffset& operator=(const PersistentMarkerImportOffset&) = delete;
    
    
    template<typename T = PersistentMarkerImportOffset>
        static Array<std::shared_ptr<T>> fetchByCollectionTypeID(Context context, integer32 query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentMarkerImportOffset>::value),
                          "Must only query PersistentMarkerImportOffset or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentMarkerImportOffset, T>("collectionTypeID",
                                                                                              TypeEnumeration<T>::value,
                                                                                              &PersistentMarkerImportOffset::collectionTypeIDAttribute,
                                                                                              nothing,
                                                                                              query);
        }
    template<typename T = PersistentMarkerImportOffset>
        static Array<std::shared_ptr<T>> fetchByOffsetToAddInSeconds(Context context, DecimalNumber query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentMarkerImportOffset>::value),
                          "Must only query PersistentMarkerImportOffset or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentMarkerImportOffset, T>("offsetToAddInSeconds",
                                                                                              TypeEnumeration<T>::value,
                                                                                              &PersistentMarkerImportOffset::offsetToAddInSecondsAttribute,
                                                                                              nothing,
                                                                                              query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- collectionTypeID
    integer32 collectionTypeID() const;
    void setCollectionTypeID(integer32 collectionTypeID);
    
    // -- offsetToAddInSeconds
    DecimalNumber offsetToAddInSeconds() const;
    void setOffsetToAddInSeconds(DecimalNumber offsetToAddInSeconds);
    
    // -- parentTrack
    RBSchema::ObjectID parentTrackObjectID() const;
    std::shared_ptr<PersistentTrack> parentTrack() const;
    void setParentTrack(std::shared_ptr<PersistentTrack>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
