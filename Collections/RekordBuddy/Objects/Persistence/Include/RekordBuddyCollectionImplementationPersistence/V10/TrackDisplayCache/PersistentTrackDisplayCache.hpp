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
class PersistentTrackDisplayCache : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentTrackDisplayCache;
    Blob cacheDataAttribute;
    friend class PersistentTrack;
    RBSchema::ObjectID parentTrackId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTrackDisplayCache(RBSchema::Type type, Context context)
        : PersistentTrackDisplayCache{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTrackDisplayCache>::value));}
    
public:
    PersistentTrackDisplayCache(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context}, cacheDataAttribute{}, parentTrackId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTrackDisplayCache>::value));}
    ~PersistentTrackDisplayCache() override = default;
    PersistentTrackDisplayCache(const PersistentTrackDisplayCache&) = delete;
    PersistentTrackDisplayCache(PersistentTrackDisplayCache&&) = default;
    PersistentTrackDisplayCache& operator=(const PersistentTrackDisplayCache&) = delete;
    
    
    template<typename T = PersistentTrackDisplayCache>
        static Array<std::shared_ptr<T>> fetchByCacheData(Context context, Blob query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTrackDisplayCache>::value),
                          "Must only query PersistentTrackDisplayCache or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTrackDisplayCache, T>("cacheData",
                                                                                             TypeEnumeration<T>::value,
                                                                                             &PersistentTrackDisplayCache::cacheDataAttribute,
                                                                                             nothing,
                                                                                             query);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- cacheData
    Blob cacheData() const;
    void setCacheData(Blob cacheData);
    
    // -- parentTrack
    RBSchema::ObjectID parentTrackObjectID() const;
    std::shared_ptr<PersistentTrack> parentTrack() const;
    void setParentTrack(std::shared_ptr<PersistentTrack>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
