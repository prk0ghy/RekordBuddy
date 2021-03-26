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
class PersistentTrackAlbum : public PersistentStringProperty
{
    using LocalType = PersistentTrackAlbum;
    Optional<integer16> maybeNumberOfTracksAttribute;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTrackAlbum(RBSchema::Type type, Context context)
        : PersistentTrackAlbum{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTrackAlbum>::value));}
    
public:
    PersistentTrackAlbum(ObjectID id, Context context)
        : PersistentStringProperty{id, context}, maybeNumberOfTracksAttribute{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTrackAlbum>::value));}
    ~PersistentTrackAlbum() override = default;
    PersistentTrackAlbum(const PersistentTrackAlbum&) = delete;
    PersistentTrackAlbum(PersistentTrackAlbum&&) = default;
    PersistentTrackAlbum& operator=(const PersistentTrackAlbum&) = delete;
    
    
    template<typename T = PersistentTrackAlbum>
        static Array<std::shared_ptr<T>> fetchByMaybeNumberOfTracks(Context context, Optional<integer16> query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTrackAlbum>::value), "Must only query PersistentTrackAlbum or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTrackAlbum, T>("maybeNumberOfTracks",
                                                                                      TypeEnumeration<T>::value,
                                                                                      &PersistentTrackAlbum::maybeNumberOfTracksAttribute,
                                                                                      nothing,
                                                                                      {query});
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- maybeNumberOfTracks
    Optional<integer16> maybeNumberOfTracks() const;
    void setMaybeNumberOfTracks(Optional<integer16> maybeNumberOfTracks);
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
