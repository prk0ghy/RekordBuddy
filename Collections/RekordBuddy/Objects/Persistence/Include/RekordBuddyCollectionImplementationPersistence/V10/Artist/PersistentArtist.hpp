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
class PersistentArtist : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentArtist;
    String nameAttribute;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyTracksCreditedAsArtist;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyTracksCreditedAsProducer;
    friend class PersistentTrack;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTrack>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyTracksCreditedAsRemixer;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentArtist(RBSchema::Type type, Context context)
        : PersistentArtist{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentArtist>::value));}
    
public:
    PersistentArtist(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          nameAttribute{},
          manyTracksCreditedAsArtist{std::make_unique<decltype(manyTracksCreditedAsArtist)::element_type>(this, &PersistentArtist::loadTracksCreditedAsArtist)},
          manyTracksCreditedAsProducer{std::make_unique<decltype(manyTracksCreditedAsProducer)::element_type>(this, &PersistentArtist::loadTracksCreditedAsProducer)},
          manyTracksCreditedAsRemixer{std::make_unique<decltype(manyTracksCreditedAsRemixer)::element_type>(this, &PersistentArtist::loadTracksCreditedAsRemixer)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentArtist>::value));}
    ~PersistentArtist() override = default;
    PersistentArtist(const PersistentArtist&) = delete;
    PersistentArtist(PersistentArtist&&) = default;
    PersistentArtist& operator=(const PersistentArtist&) = delete;
    
    
    template<typename T = PersistentArtist>
        static Array<std::shared_ptr<T>> fetchByName(Context context, String query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentArtist>::value), "Must only query PersistentArtist or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentArtist, T>("name", TypeEnumeration<T>::value, &PersistentArtist::nameAttribute, nothing, query, op);
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- name
    String name() const;
    void setName(String name);
    
    // -- tracksCreditedAsArtist
    Array<typename RBSchema::ObjectID> tracksCreditedAsArtistObjectIDs() const;
    Set<typename RBSchema::ObjectID> tracksCreditedAsArtistObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracksCreditedAsArtist();
    const PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracksCreditedAsArtistConst() const;
    count loadTracksCreditedAsArtist(bool) const;
    typename RBSchema::ObjectID objectIDForTracksCreditedAsArtistItemAtIndex(count) const;
    void addTracksCreditedAsArtistItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsArtistItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsArtistItemAtIndex(count);
    void removeAllTracksCreditedAsArtist();
    void orderTracksCreditedAsArtistItems(const Array<std::shared_ptr<PersistentTrack>>&, count);
    void orderTracksCreditedAsArtistItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderTracksCreditedAsArtistItemsAtIndex(const Set<count>&, count);
    
    
    // -- tracksCreditedAsProducer
    Array<typename RBSchema::ObjectID> tracksCreditedAsProducerObjectIDs() const;
    Set<typename RBSchema::ObjectID> tracksCreditedAsProducerObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracksCreditedAsProducer();
    const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& tracksCreditedAsProducerConst() const;
    count loadTracksCreditedAsProducer(bool) const;
    typename RBSchema::ObjectID objectIDForTracksCreditedAsProducerItemAtIndex(count) const;
    void addTracksCreditedAsProducerItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsProducerItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsProducerItemAtIndex(count);
    void removeAllTracksCreditedAsProducer();
    void orderTracksCreditedAsProducerItems(const Array<std::shared_ptr<PersistentTrack>>&, count);
    void orderTracksCreditedAsProducerItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderTracksCreditedAsProducerItemsAtIndex(const Set<count>&, count);
    
    
    // -- tracksCreditedAsRemixer
    Array<typename RBSchema::ObjectID> tracksCreditedAsRemixerObjectIDs() const;
    Set<typename RBSchema::ObjectID> tracksCreditedAsRemixerObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTrack>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& tracksCreditedAsRemixer();
    const PersistentRelationship<std::shared_ptr<PersistentTrack>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& tracksCreditedAsRemixerConst() const;
    count loadTracksCreditedAsRemixer(bool) const;
    typename RBSchema::ObjectID objectIDForTracksCreditedAsRemixerItemAtIndex(count) const;
    void addTracksCreditedAsRemixerItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsRemixerItem(const std::shared_ptr<PersistentTrack>&);
    void removeTracksCreditedAsRemixerItemAtIndex(count);
    void removeAllTracksCreditedAsRemixer();
    void orderTracksCreditedAsRemixerItems(const Array<std::shared_ptr<PersistentTrack>>&, count);
    void orderTracksCreditedAsRemixerItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderTracksCreditedAsRemixerItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
