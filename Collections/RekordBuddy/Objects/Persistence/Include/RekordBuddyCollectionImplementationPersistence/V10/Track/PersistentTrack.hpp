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
class PersistentTrack : public GenericPersistentObject<RBSchema>
{
    using LocalType = PersistentTrack;
    String filePathRelativeToCollectionVolumeAttribute;
    Optional<Blob> maybeMacOSBookmarkAttribute;
    friend class PersistentTrackDisplayCache;
    RBSchema::ObjectID displayCacheId;
    friend class PersistentArtist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyArtists;
    friend class PersistentArtist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyProducers;
    friend class PersistentArtist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyRemixers;
    friend class PersistentDecimalProperty;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentDecimalProperty>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyDecimalProperties;
    friend class PersistentIntegerProperty;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentIntegerProperty>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyIntegerProperties;
    friend class PersistentLogEntry;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentLogEntry>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyLogEntries;
    friend class PersistentMarker;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentMarker>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyMarkers;
    friend class PersistentMarkerImportOffset;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentMarkerImportOffset>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyMarkerImportOffsets;
    friend class PersistentPlaylist;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentPlaylist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyParentPlaylists;
    friend class PersistentStringProperty;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentStringProperty>,
                                           RekordBuddy::CollectionImplementation::RBSchema,
                                           LocalType>> manyStringProperties;
    friend class PersistentTimeProperty;
    std::unique_ptr<PersistentRelationship<std::shared_ptr<PersistentTimeProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>> manyTimeProperties;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentTrack(RBSchema::Type type, Context context)
        : PersistentTrack{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentTrack>::value));}
    
public:
    PersistentTrack(ObjectID id, Context context)
        : GenericPersistentObject<RBSchema>{id, context},
          filePathRelativeToCollectionVolumeAttribute{},
          maybeMacOSBookmarkAttribute{},
          displayCacheId{},
          manyArtists{std::make_unique<decltype(manyArtists)::element_type>(this, &PersistentTrack::loadArtists)},
          manyProducers{std::make_unique<decltype(manyProducers)::element_type>(this, &PersistentTrack::loadProducers)},
          manyRemixers{std::make_unique<decltype(manyRemixers)::element_type>(this, &PersistentTrack::loadRemixers)},
          manyDecimalProperties{std::make_unique<decltype(manyDecimalProperties)::element_type>(this, &PersistentTrack::loadDecimalProperties)},
          manyIntegerProperties{std::make_unique<decltype(manyIntegerProperties)::element_type>(this, &PersistentTrack::loadIntegerProperties)},
          manyLogEntries{std::make_unique<decltype(manyLogEntries)::element_type>(this, &PersistentTrack::loadLogEntries)},
          manyMarkers{std::make_unique<decltype(manyMarkers)::element_type>(this, &PersistentTrack::loadMarkers)},
          manyMarkerImportOffsets{std::make_unique<decltype(manyMarkerImportOffsets)::element_type>(this, &PersistentTrack::loadMarkerImportOffsets)},
          manyParentPlaylists{std::make_unique<decltype(manyParentPlaylists)::element_type>(this, &PersistentTrack::loadParentPlaylists)},
          manyStringProperties{std::make_unique<decltype(manyStringProperties)::element_type>(this, &PersistentTrack::loadStringProperties)},
          manyTimeProperties{std::make_unique<decltype(manyTimeProperties)::element_type>(this, &PersistentTrack::loadTimeProperties)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentTrack>::value));}
    ~PersistentTrack() override = default;
    PersistentTrack(const PersistentTrack&) = delete;
    PersistentTrack(PersistentTrack&&) = default;
    PersistentTrack& operator=(const PersistentTrack&) = delete;
    
    
    template<typename T = PersistentTrack>
        static Array<std::shared_ptr<T>> fetchByFilePathRelativeToCollectionVolume(Context context, String query, QueryOperation op = QueryOperation::Equals)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTrack>::value), "Must only query PersistentTrack or subtypes");
            
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTrack, T>("filePathRelativeToCollectionVolume",
                                                                                 TypeEnumeration<T>::value,
                                                                                 &PersistentTrack::filePathRelativeToCollectionVolumeAttribute,
                                                                                 nothing,
                                                                                 query,
                                                                                 op);
        }
    template<typename T = PersistentTrack>
        static Array<std::shared_ptr<T>> fetchByMaybeMacOSBookmark(Context context, Optional<Blob> query)
        {
            static_assert(RBSchema::typeIs(TypeEnumeration<T>::value, TypeEnumeration<PersistentTrack>::value), "Must only query PersistentTrack or subtypes");
            // -- unindexed
            return context->template fetchObjectsByAttribute<PersistentTrack, T>("maybeMacOSBookmark",
                                                                                 TypeEnumeration<T>::value,
                                                                                 &PersistentTrack::maybeMacOSBookmarkAttribute,
                                                                                 nothing,
                                                                                 {query});
        }
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- filePathRelativeToCollectionVolume
    String filePathRelativeToCollectionVolume() const;
    void setFilePathRelativeToCollectionVolume(String filePathRelativeToCollectionVolume);
    
    // -- maybeMacOSBookmark
    Optional<Blob> maybeMacOSBookmark() const;
    void setMaybeMacOSBookmark(Optional<Blob> maybeMacOSBookmark);
    
    // -- displayCache
    RBSchema::ObjectID displayCacheObjectID() const;
    std::shared_ptr<PersistentTrackDisplayCache> displayCache() const;
    void setDisplayCache(std::shared_ptr<PersistentTrackDisplayCache>);
    // -- artists
    Array<typename RBSchema::ObjectID> artistsObjectIDs() const;
    Set<typename RBSchema::ObjectID> artistsObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& artists();
    const PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& artistsConst() const;
    count loadArtists(bool) const;
    typename RBSchema::ObjectID objectIDForArtistsItemAtIndex(count) const;
    void addArtistsItem(const std::shared_ptr<PersistentArtist>&);
    void removeArtistsItem(const std::shared_ptr<PersistentArtist>&);
    void removeArtistsItemAtIndex(count);
    void removeAllArtists();
    void orderArtistsItems(const Array<std::shared_ptr<PersistentArtist>>&, count);
    void orderArtistsItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderArtistsItemsAtIndex(const Set<count>&, count);
    
    
    // -- producers
    Array<typename RBSchema::ObjectID> producersObjectIDs() const;
    Set<typename RBSchema::ObjectID> producersObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& producers();
    const PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& producersConst() const;
    count loadProducers(bool) const;
    typename RBSchema::ObjectID objectIDForProducersItemAtIndex(count) const;
    void addProducersItem(const std::shared_ptr<PersistentArtist>&);
    void removeProducersItem(const std::shared_ptr<PersistentArtist>&);
    void removeProducersItemAtIndex(count);
    void removeAllProducers();
    void orderProducersItems(const Array<std::shared_ptr<PersistentArtist>>&, count);
    void orderProducersItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderProducersItemsAtIndex(const Set<count>&, count);
    
    
    // -- remixers
    Array<typename RBSchema::ObjectID> remixersObjectIDs() const;
    Set<typename RBSchema::ObjectID> remixersObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& remixers();
    const PersistentRelationship<std::shared_ptr<PersistentArtist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& remixersConst() const;
    count loadRemixers(bool) const;
    typename RBSchema::ObjectID objectIDForRemixersItemAtIndex(count) const;
    void addRemixersItem(const std::shared_ptr<PersistentArtist>&);
    void removeRemixersItem(const std::shared_ptr<PersistentArtist>&);
    void removeRemixersItemAtIndex(count);
    void removeAllRemixers();
    void orderRemixersItems(const Array<std::shared_ptr<PersistentArtist>>&, count);
    void orderRemixersItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderRemixersItemsAtIndex(const Set<count>&, count);
    
    
    // -- decimalProperties
    Array<typename RBSchema::ObjectID> decimalPropertiesObjectIDs() const;
    Set<typename RBSchema::ObjectID> decimalPropertiesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentDecimalProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& decimalProperties();
    const PersistentRelationship<std::shared_ptr<PersistentDecimalProperty>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& decimalPropertiesConst() const;
    count loadDecimalProperties(bool) const;
    typename RBSchema::ObjectID objectIDForDecimalPropertiesItemAtIndex(count) const;
    void addDecimalPropertiesItem(const std::shared_ptr<PersistentDecimalProperty>&);
    void removeDecimalPropertiesItem(const std::shared_ptr<PersistentDecimalProperty>&);
    void removeDecimalPropertiesItemAtIndex(count);
    void removeAllDecimalProperties();
    void orderDecimalPropertiesItems(const Array<std::shared_ptr<PersistentDecimalProperty>>&, count);
    void orderDecimalPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderDecimalPropertiesItemsAtIndex(const Set<count>&, count);
    
    
    // -- integerProperties
    Array<typename RBSchema::ObjectID> integerPropertiesObjectIDs() const;
    Set<typename RBSchema::ObjectID> integerPropertiesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentIntegerProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& integerProperties();
    const PersistentRelationship<std::shared_ptr<PersistentIntegerProperty>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& integerPropertiesConst() const;
    count loadIntegerProperties(bool) const;
    typename RBSchema::ObjectID objectIDForIntegerPropertiesItemAtIndex(count) const;
    void addIntegerPropertiesItem(const std::shared_ptr<PersistentIntegerProperty>&);
    void removeIntegerPropertiesItem(const std::shared_ptr<PersistentIntegerProperty>&);
    void removeIntegerPropertiesItemAtIndex(count);
    void removeAllIntegerProperties();
    void orderIntegerPropertiesItems(const Array<std::shared_ptr<PersistentIntegerProperty>>&, count);
    void orderIntegerPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderIntegerPropertiesItemsAtIndex(const Set<count>&, count);
    
    
    // -- logEntries
    Array<typename RBSchema::ObjectID> logEntriesObjectIDs() const;
    Set<typename RBSchema::ObjectID> logEntriesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentLogEntry>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& logEntries();
    const PersistentRelationship<std::shared_ptr<PersistentLogEntry>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& logEntriesConst() const;
    count loadLogEntries(bool) const;
    typename RBSchema::ObjectID objectIDForLogEntriesItemAtIndex(count) const;
    void addLogEntriesItem(const std::shared_ptr<PersistentLogEntry>&);
    void removeLogEntriesItem(const std::shared_ptr<PersistentLogEntry>&);
    void removeLogEntriesItemAtIndex(count);
    void removeAllLogEntries();
    void orderLogEntriesItems(const Array<std::shared_ptr<PersistentLogEntry>>&, count);
    void orderLogEntriesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderLogEntriesItemsAtIndex(const Set<count>&, count);
    
    
    // -- markers
    Array<typename RBSchema::ObjectID> markersObjectIDs() const;
    Set<typename RBSchema::ObjectID> markersObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentMarker>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& markers();
    const PersistentRelationship<std::shared_ptr<PersistentMarker>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& markersConst() const;
    count loadMarkers(bool) const;
    typename RBSchema::ObjectID objectIDForMarkersItemAtIndex(count) const;
    void addMarkersItem(const std::shared_ptr<PersistentMarker>&);
    void removeMarkersItem(const std::shared_ptr<PersistentMarker>&);
    void removeMarkersItemAtIndex(count);
    void removeAllMarkers();
    void orderMarkersItems(const Array<std::shared_ptr<PersistentMarker>>&, count);
    void orderMarkersItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderMarkersItemsAtIndex(const Set<count>&, count);
    
    
    // -- markerImportOffsets
    Array<typename RBSchema::ObjectID> markerImportOffsetsObjectIDs() const;
    Set<typename RBSchema::ObjectID> markerImportOffsetsObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentMarkerImportOffset>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& markerImportOffsets();
    const PersistentRelationship<std::shared_ptr<PersistentMarkerImportOffset>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& markerImportOffsetsConst() const;
    count loadMarkerImportOffsets(bool) const;
    typename RBSchema::ObjectID objectIDForMarkerImportOffsetsItemAtIndex(count) const;
    void addMarkerImportOffsetsItem(const std::shared_ptr<PersistentMarkerImportOffset>&);
    void removeMarkerImportOffsetsItem(const std::shared_ptr<PersistentMarkerImportOffset>&);
    void removeMarkerImportOffsetsItemAtIndex(count);
    void removeAllMarkerImportOffsets();
    void orderMarkerImportOffsetsItems(const Array<std::shared_ptr<PersistentMarkerImportOffset>>&, count);
    void orderMarkerImportOffsetsItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderMarkerImportOffsetsItemsAtIndex(const Set<count>&, count);
    
    
    // -- parentPlaylists
    Array<typename RBSchema::ObjectID> parentPlaylistsObjectIDs() const;
    Set<typename RBSchema::ObjectID> parentPlaylistsObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentPlaylist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& parentPlaylists();
    const PersistentRelationship<std::shared_ptr<PersistentPlaylist>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& parentPlaylistsConst() const;
    count loadParentPlaylists(bool) const;
    typename RBSchema::ObjectID objectIDForParentPlaylistsItemAtIndex(count) const;
    void addParentPlaylistsItem(const std::shared_ptr<PersistentPlaylist>&);
    void removeParentPlaylistsItem(const std::shared_ptr<PersistentPlaylist>&);
    void removeParentPlaylistsItemAtIndex(count);
    void removeAllParentPlaylists();
    void orderParentPlaylistsItems(const Array<std::shared_ptr<PersistentPlaylist>>&, count);
    void orderParentPlaylistsItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderParentPlaylistsItemsAtIndex(const Set<count>&, count);
    
    
    // -- stringProperties
    Array<typename RBSchema::ObjectID> stringPropertiesObjectIDs() const;
    Set<typename RBSchema::ObjectID> stringPropertiesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentStringProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& stringProperties();
    const PersistentRelationship<std::shared_ptr<PersistentStringProperty>,
                                 RekordBuddy::CollectionImplementation::RBSchema,
                                 LocalType>& stringPropertiesConst() const;
    count loadStringProperties(bool) const;
    typename RBSchema::ObjectID objectIDForStringPropertiesItemAtIndex(count) const;
    void addStringPropertiesItem(const std::shared_ptr<PersistentStringProperty>&);
    void removeStringPropertiesItem(const std::shared_ptr<PersistentStringProperty>&);
    void removeStringPropertiesItemAtIndex(count);
    void removeAllStringProperties();
    void orderStringPropertiesItems(const Array<std::shared_ptr<PersistentStringProperty>>&, count);
    void orderStringPropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderStringPropertiesItemsAtIndex(const Set<count>&, count);
    
    
    // -- timeProperties
    Array<typename RBSchema::ObjectID> timePropertiesObjectIDs() const;
    Set<typename RBSchema::ObjectID> timePropertiesObjectIDsOfType(typename RBSchema::Type) const;
    PersistentRelationship<std::shared_ptr<PersistentTimeProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& timeProperties();
    const PersistentRelationship<std::shared_ptr<PersistentTimeProperty>, RekordBuddy::CollectionImplementation::RBSchema, LocalType>& timePropertiesConst() const;
    count loadTimeProperties(bool) const;
    typename RBSchema::ObjectID objectIDForTimePropertiesItemAtIndex(count) const;
    void addTimePropertiesItem(const std::shared_ptr<PersistentTimeProperty>&);
    void removeTimePropertiesItem(const std::shared_ptr<PersistentTimeProperty>&);
    void removeTimePropertiesItemAtIndex(count);
    void removeAllTimeProperties();
    void orderTimePropertiesItems(const Array<std::shared_ptr<PersistentTimeProperty>>&, count);
    void orderTimePropertiesItemsByObjectID(const Array<typename RBSchema::ObjectID>&, count);
    void orderTimePropertiesItemsAtIndex(const Set<count>&, count);
    
    
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
