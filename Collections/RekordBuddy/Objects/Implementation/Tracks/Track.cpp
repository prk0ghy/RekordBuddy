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

#include <RekordBuddyCollection/Crates/Crates.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>

#include <RekordBuddyCollection/Implementation/Tracks/TrackDisplayCache.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Internal/TrackInternal.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>
#include <RekordBuddyCollection/Implementation/Artist.hpp>

#include <CommonCollection/Markers/MarkerOffset.hpp>
#include <CommonCollection/Tracks/TrackDisplayCache.hpp>

#include <TrackFiles/TrackFile.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Implementation

#define NXA_OBJECT_CLASS                                        Track
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Protected Factory Methods

Track Track::p_createTrackWithRelativeFilePathInCollection(const FilePath& path, Collection& collection)
{
    auto newImplementationTrack = Track{ collection.persistentContext()->createObject<PersistentTrack>() };

    // -- We call the Track method here because this can potentially do extra work underneath the covers (as in setting the bookmark on macOS).
    newImplementationTrack.setRelativeFilePath(path);
    newImplementationTrack.markAsModifiedNow();

    return newImplementationTrack;
}

// -- Protected Instance Methods

Blob Track::p_displayCacheData() const
{
    MutableBlob result;

    Common::TrackDisplayCache::addStringValueForTagOfTypeToBlob(nxa_const_internal->filePathRelativeToCollectionVolume(), Common::Property::TypeID::RelativeFilePath, result);

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& integerProperties = nxa_internal->integerProperties();
    for (auto&& tag : integerProperties) {
#if defined(NXA_BETA_BUILD)
        auto type = tag->objectID().objectType();
        if ((type == RekordBuddy::CollectionImplementation::RBSchema::Type::Track) || (type == RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
            NXA_ALOG("Invalid Type.");
        }
#endif
        Common::TrackDisplayCache::addIntegerValueForTagOfTypeToBlob(tag->integerValue(),
                                                                     RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(tag->objectID().objectType()),
                                                                     result);
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& decimalProperties = nxa_internal->decimalProperties();
    for (auto&& tag : decimalProperties) {
#if defined(NXA_BETA_BUILD)
        auto type = tag->objectID().objectType();
        if ((type == RekordBuddy::CollectionImplementation::RBSchema::Type::Track) || (type == RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
            NXA_ALOG("Invalid Type.");
        }
#endif
        Common::TrackDisplayCache::addDecimalValueForTagOfTypeToBlob(tag->decimalValue(),
                                                                     RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(tag->objectID().objectType()),
                                                                     result);
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& timeProperties = nxa_internal->timeProperties();
    for (auto&& tag : timeProperties) {
#if defined(NXA_BETA_BUILD)
        auto type = tag->objectID().objectType();
        if ((type == RekordBuddy::CollectionImplementation::RBSchema::Type::Track) || (type == RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
            NXA_ALOG("Invalid Type.");
        }
#endif
        Common::TrackDisplayCache::addTimeValueForTagOfTypeToBlob(tag->timeValue(),
                                                                  RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(tag->objectID().objectType()),
                                                                  result);
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& stringProperties = nxa_internal->stringProperties();
    for (auto&& tag : stringProperties) {
#if defined(NXA_BETA_BUILD)
        auto objectType = tag->objectID().objectType();
        if ((objectType == RekordBuddy::CollectionImplementation::RBSchema::Type::Track) || (objectType == RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist)) {
            NXA_ALOG("Invalid Type.");
        }
#endif
        auto type = RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(tag->objectID().objectType());
        Common::TrackDisplayCache::addStringValueForTagOfTypeToBlob(tag->stringValue(), type, result);

        if (type == Common::Property::TypeID::Album) {
            auto maybeNumberOfTracks = std::static_pointer_cast<PersistentTrackAlbum>(tag)->maybeNumberOfTracks();
            if (maybeNumberOfTracks.isValid()) {
                Common::TrackDisplayCache::addIntegerValueForTagOfTypeToBlob(*maybeNumberOfTracks,
                                                                             Common::Property::TypeID::NumberOfTracks,
                                                                             result);
            }
        }
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& artists = nxa_internal->artists();
    for (auto&& artist : artists) {
        Common::TrackDisplayCache::addStringValueForTagOfTypeToBlob(artist->name(), Common::Property::TypeID::ArtistCredit, result);
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& remixers = nxa_internal->remixers();
    for (auto&& remixer : remixers) {
        Common::TrackDisplayCache::addStringValueForTagOfTypeToBlob(remixer->name(), Common::Property::TypeID::RemixerCredit, result);
    }

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& producers = nxa_internal->producers();
    for (auto&& producer : producers) {
        Common::TrackDisplayCache::addStringValueForTagOfTypeToBlob(producer->name(), Common::Property::TypeID::ProducerCredit, result);
    }

    return std::move(result);
}

void Track::p_deleteFromCollection()
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    auto markersCopy = internal->markers().objects();
    auto stringPropertiesCopy = internal->stringProperties().objects();
    auto integerPropertiesCopy = internal->integerProperties().objects();
    auto decimalPropertiesCopy = internal->decimalProperties().objects();
    auto timePropertiesCopy = internal->timeProperties().objects();
    auto markerImportOffsetsCopy = internal->markerImportOffsets().objects();
    auto maybeDisplayCacheObject = internal->displayCache();

    internal->deleteObject();

    if (maybeDisplayCacheObject.get() != nullptr) {
        TrackDisplayCache(maybeDisplayCacheObject).deleteFromCollection();
    }

    for (auto&& marker : markersCopy) {
        marker->deleteObject();
    }

    for (auto&& tag : stringPropertiesCopy) {
        auto stringProperty = StringProperty(tag);
        if (stringProperty.isAnOrphan() && stringProperty.shouldBeDeletedIfOrphan()) {
            stringProperty.deleteFromCollection();
        }
        else {
            tag->removeParentTracksItem(*this);
        }
    }

    for (auto&& tag : integerPropertiesCopy) {
        auto integerProperty = IntegerProperty(tag);
        if (integerProperty.isAnOrphan() && integerProperty.shouldBeDeletedIfOrphan()) {
            integerProperty.deleteFromCollection();
        }
        else {
            tag->removeParentTracksItem(*this);
        }
    }

    for (auto&& tag : decimalPropertiesCopy) {
        auto decimalProperty = DecimalProperty(tag);
        if (decimalProperty.isAnOrphan() && decimalProperty.shouldBeDeletedIfOrphan()) {
            decimalProperty.deleteFromCollection();
        }
        else {
            tag->removeParentTracksItem(*this);
        }
    }

    for (auto&& tag : timePropertiesCopy) {
        auto timeProperty = TimeProperty(tag);
        if (timeProperty.isAnOrphan() && timeProperty.shouldBeDeletedIfOrphan()) {
            timeProperty.deleteFromCollection();
        }
        else {
            tag->removeParentTracksItem(*this);
        }
    }

    for (auto&& offset : markerImportOffsetsCopy) {
        offset->deleteObject();
    }
}

// -- Class Methods

void Track::updateSmartContentForParentPlaylistsOfTracks(Array<NotNull<CollectionImplementation::Track*>>& tracks)
{
    if (!tracks.length()) {
        return;
    }

    MutableSet<Playlist> playlistsToUpdate;

    for (auto&& track : tracks) {
        auto parentPlaylists = track->parentPlaylists();
        for (auto&& playlist : parentPlaylists) {
            playlistsToUpdate.add(playlist);
        }
    }

    Array<Playlist> playlistsToUpdateAsArray{ std::move(playlistsToUpdate) };
    for (auto&& playlist : playlistsToUpdateAsArray) {
        playlist.updateSmartContent(Crate::AndUpdateClones::Yes);
    }
}

// -- Instance Methods

ObjectID Track::reference() const
{
    return { nxa_const_internal->objectID() };
}

FilePath Track::relativeFilePath() const
{
    return FilePath::filePathFromCrossPlatformSerialization(nxa_const_internal->filePathRelativeToCollectionVolume());
}

Time Track::lastModificationTime() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackModificationTime>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->value() : Time::distantPast();
}

void Track::markAsModifiedNow()
{
    CollectionImplementation::p_replaceExistingTagsInWith<TrackModificationTime>(*this,
                                                                                 Array<TrackModificationTime>{ TrackModificationTime::withValueInCollection(Time::currentTime(),
                                                                                                                                                            this->collection().asReference()) },
                                                                                 CollectionImplementation::Track::AndDeleteOrphans::Yes);
}

Optional<String> Track::maybeTitle() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackTitle>(nxa_const_internal);
}

void Track::setTitle(const Optional<String>& maybeValue)
{
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackTitle>(*this,
                                                                       maybeValue,
                                                                       p_trackCollection(nxa_internal).asReference(),
                                                                       AndDeleteOrphans::Yes);
}

Optional<String> Track::maybeAlbum() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackAlbum>(nxa_const_internal);
}

void Track::setAlbum(const Optional<String>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackAlbum>(*this, maybeValue, collection,
                                                                       AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeAlbumTrackCount() const
{
    auto tag = p_maybeFirstTagOfTypeIn<TrackAlbum>(nxa_const_internal);
    return tag.isValid() ? tag->maybeNumberOfTracks() : nothing;
}

void Track::setAlbumTrackCount(const Optional<count>& maybeValue)
{
    auto tag = p_maybeFirstTagOfTypeIn<TrackAlbum>(nxa_internal);
    if (tag.isValid()) {
        tag->setNumberOfTracks(maybeValue);
    }
}

Optional<String> Track::maybeComments() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackComments>(nxa_const_internal);
}

void Track::setComments(const Optional<String>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackComments>(*this, maybeValue, collection,
                                                                          AndDeleteOrphans::Yes);
}

Array<String> Track::genres() const
{
    return p_stringValuesOfTagsInTrack<TrackGenre>(nxa_const_internal);
}

void Track::setGenres(const Array<String>& values)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagsInTrackWithStringValuesInCollection<TrackGenre>(*this, values, collection,
                                                                         AndDeleteOrphans::Yes);
}

Optional<String> Track::maybeGrouping() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackGrouping>(nxa_const_internal);
}

void Track::setGrouping(const Optional<String>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackGrouping>(*this, maybeValue, collection,
                                                                          AndDeleteOrphans::Yes);
}

Optional<String> Track::maybeMixName() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackMixName>(nxa_const_internal);
}

void Track::setMixName(const Optional<String>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackMixName>(*this, maybeValue, collection,
                                                                         AndDeleteOrphans::Yes);
}

Optional<String> Track::maybeRecordLabel() const
{
    return p_maybeStringValueOfFirstTagInTrack<TrackRecordLabel>(nxa_const_internal);
}

void Track::setRecordLabel(const Optional<String>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithStringValueInCollection<TrackRecordLabel>(*this, maybeValue, collection,
                                                                             AndDeleteOrphans::Yes);
}

Array<String> Track::tags() const
{
    return p_stringValuesOfTagsInTrack<TrackTag>(nxa_const_internal);
}

void Track::setTags(const Array<String>& values)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagsInTrackWithStringValuesInCollection<TrackTag>(*this, values, collection,
                                                                       AndDeleteOrphans::No);
}

Array<String> Track::artists() const
{
    MutableArray<String> results;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& artists = nxa_internal->artists();
    for (auto&& artist : artists) {
        results.append(artist->name());
    }

    return std::move(results);
}

void Track::setArtists(const Array<String>& values)
{
    auto internal = nxa_internal;

    for (auto&& artist : internal->artists().objects()) {
        internal->removeArtistsItem(artist);
    }

    auto& collection = p_trackCollection(internal).asReference();
    for (auto&& newArtistName : values) {
        internal->addArtistsItem(collection.artistNamed(newArtistName).persistentObject());
    }
}

Array<String> Track::producers() const
{
    MutableArray<String> results;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& producers = nxa_internal->producers();
    for (auto&& producer : producers) {
        results.append(producer->name());
    }

    return std::move(results);
}

void Track::setProducers(const Array<String>& values)
{
    auto internal = nxa_internal;

    for (auto&& producer : internal->producers().objects()) {
        internal->removeProducersItem(producer);
    }

    auto& collection = p_trackCollection(internal).asReference();
    for (auto&& newProducerName : values) {
        internal->addProducersItem(collection.artistNamed(newProducerName).persistentObject());
    }
}

Array<String> Track::remixers() const
{
    MutableArray<String> results;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& remixers = nxa_internal->remixers();
    for (auto&& remixer : remixers) {
        results.append(remixer->name());
    }

    return std::move(results);
}

void Track::setRemixers(const Array<String>& values)
{
    auto internal = nxa_internal;

    for (auto&& remixer : internal->remixers().objects()) {
        internal->removeRemixersItem(remixer);
    }

    auto& collection = p_trackCollection(internal).asReference();
    for (auto&& newRemixerName : values) {
        internal->addRemixersItem(collection.artistNamed(newRemixerName).persistentObject());
    }
}

Optional<Date> Track::maybeDateAdded() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackDateAdded>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<Date>{ maybeTag->value() } : nothing;
}

void Track::setDateAdded(const Optional<Date>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_maybeReplaceExistingTagInTrackWithStringValueInCollection<TrackDateAdded>(*this, maybeValue.isValid() ?
                                                                                Optional<String>{ maybeValue->asStringSeparatedWith('-') } : nothing,
                                                                                collection,
                                                                                AndDeleteOrphans::Yes);
}

Optional<Date> Track::maybeDateReleased() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackDateReleased>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<Date>{ maybeTag->value() } : nothing;
}

void Track::setDateReleased(const Optional<Date>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_maybeReplaceExistingTagInTrackWithStringValueInCollection<TrackDateReleased>(*this, maybeValue.isValid() ?
                                                                                   Optional<String>{ maybeValue->asStringSeparatedWith('-') } : nothing,
                                                                                   collection,
                                                                                   AndDeleteOrphans::Yes);
}

Optional<boolean> Track::maybeBeatGridLocked() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackBeatGridLockedFlag>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->value() : false;
}

void Track::setBeatGridLocked(boolean value)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackBeatGridLockedFlag>(*this, Optional<integer32>{ value },
                                                                                     collection,
                                                                                     AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeBitDepthInBits() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackBitDepth>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<count>{ maybeTag->valueInBits() } : nothing;
}

void Track::setBitDepthInBits(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackBitDepth>(*this, maybeValue,
                                                                           collection,
                                                                           AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeBitRateInKiloBitsPerSecond() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackBitRate>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<count>{ maybeTag->valueInBitsPerSecond() } : nothing;
}

void Track::setBitRateInKiloBitsPerSecond(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackBitRate>(*this, maybeValue,
                                                                          collection,
                                                                          AndDeleteOrphans::Yes);
}

Optional<DecimalNumber> Track::maybeBeatsPerMinute() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackBpm>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->valueInBeatsPerMinute() : DecimalNumber("0");
}

void Track::setBeatsPerMinute(const Optional<DecimalNumber>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithDecimalValueInCollection<TrackBpm>(*this, maybeValue,
                                                                      collection,
                                                                      AndDeleteOrphans::Yes);
}

Optional<Color> Track::maybeColor() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackColor>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<Color>{ maybeTag->value() } : nothing;
}

void Track::setColor(const Optional<Color>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackColor>(*this, maybeValue.isValid() ?
                                                                        Optional<integer32>{ maybeValue->asRGBA() } : nothing,
                                                                        collection,
                                                                        AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeFileSizeInBytes() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackFileSize>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<count>{ maybeTag->valueInBytes() } : nothing;
}

void Track::setFileSizeInBytes(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackFileSize>(*this, maybeValue,
                                                                           collection,
                                                                           AndDeleteOrphans::Yes);
}

AudioFileType Track::fileType() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackFileType>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->value() : AudioFileType::Unknown;
}

void Track::setFileType(AudioFileType value)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackFileType>(*this,
                                                                           Optional<AudioFileType>{ value },
                                                                           collection,
                                                                           AndDeleteOrphans::Yes);
}

Array<String> Track::musicalKeys() const
{
    MutableArray<String> values;

    for (auto&& tag : p_tagsOfTypeIn<TrackKey>(nxa_const_internal)) {
        values.append(Common::MusicalKey::stringValueForKey(tag.value()));
    }

    return { std::move(values) };
}

void Track::setMusicalKeys(const Array<String>& values)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();

    MutableArray<TrackKey> newTags;

    for (auto&& value : values) {
        auto maybeKeyValue = Common::MusicalKey::maybeKeyValueFromString(value);
        if (maybeKeyValue.isValid()) {
            newTags.append(TrackKey::withValueInCollection(*maybeKeyValue, collection));
        }
    }

    p_replaceExistingTagsInWith<TrackKey>(*this, std::move(newTags), AndDeleteOrphans::Yes);
}

Optional<DecimalNumber> Track::maybeLengthInSeconds() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackLength>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->valueInSeconds() : DecimalNumber{ };
}

void Track::setLengthInSeconds(const Optional<DecimalNumber>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithDecimalValueInCollection<TrackLength>(*this,
                                                                         maybeValue,
                                                                         collection,
                                                                         AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeTrackNumber() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackNumber>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<count>(maybeTag->value()) : nothing;
}

void Track::setTrackNumber(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackNumber>(*this, maybeValue,
                                                                         collection,
                                                                         AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeDiscNumber() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackDiscNumber>(nxa_const_internal);
    return maybeTag.isValid() ? Optional<count>(maybeTag->value()) : nothing;
}

void Track::setDiscNumber(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackDiscNumber>(*this, maybeValue,
                                                                             collection,
                                                                             AndDeleteOrphans::Yes);
}

Optional<count> Track::maybePlayCount() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackPlayCount>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->value() : 0;
}

void Track::setPlayCount(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackPlayCount>(*this, maybeValue,
                                                                            collection,
                                                                            AndDeleteOrphans::Yes);
}

Optional<Common::TrackRating> Track::maybeRating() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackRating>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->maybeValue() : nothing;
}

void Track::setRating(const Optional<Common::TrackRating>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackRating>(*this, maybeValue.isValid() ? maybeValue->value() : Optional<integer>{ },
                                                                         collection,
                                                                         AndDeleteOrphans::Yes);
}

Optional<count> Track::maybeSampleRateInHertz() const
{
    auto maybeTag = p_maybeFirstTagOfTypeIn<TrackSampleRate>(nxa_const_internal);
    return maybeTag.isValid() ? maybeTag->valueInHertz() : 0;
}

void Track::setSampleRateInHertz(const Optional<count>& maybeValue)
{
    auto& collection = p_trackCollection(nxa_internal).asReference();
    p_replaceExistingTagInTrackWithIntegerValueInCollection<TrackSampleRate>(*this, maybeValue,
                                                                             collection,
                                                                             AndDeleteOrphans::Yes);
}

boolean Track::hasOnlyOnePlaylistPointingToIt() const
{
    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& parentPlaylists = nxa_internal->parentPlaylists();
    return parentPlaylists.length() == 1;
}

boolean Track::hasGridMarkers() const
{
    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& markers = nxa_internal->markers();
    for (auto&& marker : markers) {
        if (marker->objectType() == RekordBuddy::CollectionImplementation::RBSchema::Type::GridMarker) {
            return true;
        }
    }

    return false;
}

Array<Playlist> Track::parentPlaylists()
{
    MutableArray<Playlist> result;

    for (auto&& persistentPlaylist : nxa_internal->parentPlaylists()) {
        result.append(Playlist(persistentPlaylist));
    }

    return std::move(result);
}

Pointer<const Collection> Track::collection() const
{
    return p_trackCollection(nxa_const_internal);
}

Pointer<Collection> Track::collection()
{
    return p_trackCollection(nxa_internal);
}

boolean Track::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void Track::deleteFromCollection()
{
    p_trackCollection(nxa_internal).asReference().deleteTrack(*this);
}

template<typename T>
    boolean Track::containsAtLeastOneTag() const
    {
        return p_containsAtLeastOneTagIn<T>((nxa_const_internal->*MemberTraits<T>::objectIDMember)());
    }

template<typename T>
    Array<Const<T>> Track::tagsOfBaseTypeWithTypeID(Common::Property::TypeID typeID) const
    {
        NXA_ASSERT_TRUE(Common::Property::valueTypeIDFromPropertyType(typeID) == T::valueTypeID);

        auto internal = nxa_const_internal;
        MutableArray<Const<T>> result;

        for (auto&& objectID : p_objectIDsOfPropertiesOfTypeInObjectIDs(typeID, (internal->*MemberTraits<T>::objectIDMember)())) {
            auto maybeTag = internal->getContext()->fetchObject<typename PersistentTypeFromBaseType<T>::Type >(objectID);
            if (maybeTag.isValid()) {
                result.append(Const<T>(T{ std::move(*maybeTag) }));
            }
        }

        return std::move(result);
    }

template<typename T>
    Array<T> Track::tagsOfBaseTypeWithTypeID(Common::Property::TypeID typeID)
    {
        NXA_ASSERT_TRUE(Common::Property::valueTypeIDFromPropertyType(typeID) == T::valueTypeID);

        auto internal = nxa_internal;
        MutableArray<T> result;

        for (auto&& objectID : p_objectIDsOfPropertiesOfTypeInObjectIDs(typeID, (internal->*MemberTraits<T>::objectIDMember)())) {
            auto maybeTag = internal->getContext()->fetchObject<typename PersistentTypeFromBaseType<T>::Type >(objectID);
            if (maybeTag.isValid()) {
                result.append(T(std::move(*maybeTag)));
            }
        }

        return std::move(result);
    }

template<typename T>
    void Track::addTagAndSetAsModifiedOn(T tag, const Time& modificationTime, AndDeleteOrphans deleteOrphans)
    {
        auto internal = nxa_internal;
        auto typeID = T::typeID;

        auto existingIDs = p_objectIDsOfPropertiesOfTypeInObjectIDs(typeID, (internal->*MemberTraits<T>::objectIDMember)());
        if (existingIDs.contains(internal->objectID())) {
            return;
        }

        MutableArray<T> existingTags = p_tagsOfTypeIn<T>(internal);
        existingTags.append(tag);

        p_replaceExistingTagsInWith<T>(*this, std::move(existingTags), deleteOrphans);
    }

// -- Forced Template Instantiations

#define CONTAINS_AT_LEAST_ONE_TAG(type, typeID, argument) \
    template boolean Track::containsAtLeastOneTag<type>() const;
ALL_TAG_TYPES(CONTAINS_AT_LEAST_ONE_TAG, None)
#undef CONTAINS_AT_LEAST_ONE_TAG

template Array<Const<StringProperty>> Track::tagsOfBaseTypeWithTypeID<StringProperty>(Common::Property::TypeID) const;
template Array<Const<IntegerProperty>> Track::tagsOfBaseTypeWithTypeID<IntegerProperty>(Common::Property::TypeID) const;
template Array<Const<DecimalProperty>> Track::tagsOfBaseTypeWithTypeID<DecimalProperty>(Common::Property::TypeID) const;
template Array<Const<TimeProperty>> Track::tagsOfBaseTypeWithTypeID<TimeProperty>(Common::Property::TypeID) const;

template Array<StringProperty> Track::tagsOfBaseTypeWithTypeID<StringProperty>(Common::Property::TypeID);
template Array<IntegerProperty> Track::tagsOfBaseTypeWithTypeID<IntegerProperty>(Common::Property::TypeID);
template Array<DecimalProperty> Track::tagsOfBaseTypeWithTypeID<DecimalProperty>(Common::Property::TypeID);
template Array<TimeProperty> Track::tagsOfBaseTypeWithTypeID<TimeProperty>(Common::Property::TypeID);
