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

#pragma once

#include <CommonCollection/Tracks/Track.hpp>

#include <RekordBuddyCollection/Markers/Markers.hpp>

#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/TrackDisplayCache.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Playlist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <TrackFiles/TrackFile.hpp>

namespace NxA { namespace RekordBuddy {

namespace CollectionImplementation {

// -- Forward Declarations
class Collection;
class CollectionInternal;

}

// -- Forward Declarations
class MutableCollection;

// -- Internal Interface
class MutableTrack : public Common::MutableTrack, public Common::Track
{
    // -- Friends
    friend class MutableCollection;
    friend class CollectionImplementation::Collection;
    friend class CollectionImplementation::CollectionInternal;

    // -- Private Constants
    enum class AndMakeSureTrackIsNotModified {
        No,
        Yes
    };

    // -- Private Instance Variables
    // -- Protected Instance Variables
    boolean p_wasEdited;

    Pointer<MutableCollection> p_collection;

    CollectionImplementation::ObjectID p_trackObjectID;

    mutable Optional<CollectionImplementation::TrackDisplayCache> p_maybeRekordBuddyTrackDisplayCache;
    mutable Optional<CollectionImplementation::Track> p_maybeRekordBuddyTrack;
    mutable Optional<MutableArray<Shared<MutableMarkerOfSomeSort>>> p_maybeMarkers;

    // -- Private Instance Methods

    // -- This method is mutable and returns a mutable object even if the method is marked const
    CollectionImplementation::TrackDisplayCache& p_ensureTrackDisplayCacheIsLoaded(AndMakeSureTrackIsNotModified andMakeSureTrackIsNotModified =AndMakeSureTrackIsNotModified::Yes) const;
    // -- This method is mutable and returns a mutable object even if the method is marked const
    CollectionImplementation::Track& p_ensureTrackIsLoaded() const;

    MutableArray<Shared<MutableMarkerOfSomeSort>>& p_ensureMarkersAreLoaded() const
    {
        // -- This method is mutable and returns a mutable object even if the method is marked const
        return this->p_maybeMarkers.valueOrSet([this]() {
            MutableArray<Shared<MutableMarkerOfSomeSort>> result;

            for (auto&& marker : this->p_ensureTrackIsLoaded().asPersistent()->markers()) {
                switch (marker->objectType()) {
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::CueMarker: {
                        result.append(Shared<MutableMarkerOfSomeSort>::with(MutableCueMarker{ Shared<CollectionImplementation::PersistentCueMarker>{
                                std::static_pointer_cast<CollectionImplementation::PersistentCueMarker>(marker) },
                                                                                              MutableCueMarker::p_isProtected }));
                        continue;
                    }
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::GridMarker: {
                        result.append(Shared<MutableMarkerOfSomeSort>::with(MutableGridMarker{ Shared<CollectionImplementation::PersistentGridMarker>{
                                std::static_pointer_cast<CollectionImplementation::PersistentGridMarker>(marker) },
                                                                                               MutableGridMarker::p_isProtected }));
                        continue;
                    }
                    case RekordBuddy::CollectionImplementation::RBSchema::Type::LoopMarker: {
                        result.append(Shared<MutableMarkerOfSomeSort>::with(MutableLoopMarker{ Shared<CollectionImplementation::PersistentLoopMarker>{
                                std::static_pointer_cast<CollectionImplementation::PersistentLoopMarker>(marker) },
                                                                                               MutableLoopMarker::p_isProtected }));
                        continue;
                    }
                    default: {
                        NXA_ALOG_WITH_FORMAT("Illegal marker type: %d.", marker->objectType());
                    }
                }
            }

            return result;
        });
    }

    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::CueMarker*>& marker, Optional<DecimalNumber> maybeOffset)
    {
        this->appendCueMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }
    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::GridMarker*>& marker, Optional<DecimalNumber> maybeOffset)
    {
        this->appendGridMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }
    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::LoopMarker*>& marker, Optional<DecimalNumber> maybeOffset)
    {
        this->appendLoopMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }

    Optional<std::shared_ptr<CollectionImplementation::PersistentMarkerImportOffset>> p_maybeMarkerImportOffsetForCollectionIn(Common::Collection::Type) const;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constructors & Destructors
    MutableTrack(CollectionImplementation::ObjectID objectID,
                 Pointer<MutableCollection> inCollection,
                 const Protected&) : p_wasEdited{ false },
                                     p_collection{ inCollection },
                                     p_trackObjectID{ objectID } { }
    MutableTrack(CollectionImplementation::Track&& track,
                 Pointer<MutableCollection> inCollection,
                 const Protected&) : p_wasEdited{ false },
                                     p_collection{ inCollection },
                                     p_trackObjectID{ track.reference() },
                                     p_maybeRekordBuddyTrack{ std::move(track) }  { }
    ~MutableTrack() override
    {
        if (this->p_wasEdited) {
            // -- The track should not be marked as edited at this point in time but, if it is, we can't throw in the destructor
            // -- so instead we print a message in debug and we save the changes behind the scenes to make sure everything is ok.
            NXA_DLOG("Track should not still be edited here.");
            this->markAsModifiedNow();
        }
    }

    // -- Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Volume volume() const override;
    FilePath relativeFilePath() const override
    {
        // -- Relative file path cannot be modified on a Track so we should never be able to get invalid data from the cache.
        auto results = this->p_ensureTrackDisplayCacheIsLoaded(MutableTrack::AndMakeSureTrackIsNotModified::No).
                stringValuesForTagOfType(Common::Property::TypeID::RelativeFilePath);
        NXA_ASSERT_TRUE(results.length() == 1);

        return FilePath{ results[0] };
    }
    FilePath absoluteFilePath() const override
    {
        return Common::Track::absolutePathFor(*this);
    }

    Time lastModificationTime() const override
    {
        // -- Modification time can only be modified on a Track with markAsModifiedNow() so we should never be able to get invalid data from the cache.
        auto results = this->p_ensureTrackDisplayCacheIsLoaded(MutableTrack::AndMakeSureTrackIsNotModified::No).
                timeValuesForTagOfType(Common::Property::TypeID::LastModifiedOn);
        NXA_ASSERT_TRUE(results.length() == 1);

        return results[0];
    }
    void markAsModifiedNow() override;

    Optional<String> maybeTitle() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Title);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setTitle(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setTitle(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<String> maybeAlbum() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Album);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setAlbum(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setAlbum(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeAlbumTrackCount() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::NumberOfTracks);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setAlbumTrackCount(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setAlbumTrackCount(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<String> maybeComments() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Comments);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setComments(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setComments(maybeValue);
        this->p_wasEdited = true;
    }

    Array<String> genres() const override
    {
        return this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Genre);
    }
    void setGenres(const Array<String>&values) override
    {
        this->p_ensureTrackIsLoaded().setGenres(values);
        this->p_wasEdited = true;
    }

    Optional<String> maybeGrouping() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Grouping);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setGrouping(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setGrouping(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<String> maybeMixName() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::MixName);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setMixName(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setMixName(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<String> maybeRecordLabel() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::RecordLabel);
        return results.length() ? Optional<String>{ results[0] } : nothing;
    }
    void setRecordLabel(const Optional<String>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setRecordLabel(maybeValue);
        this->p_wasEdited = true;
    }

    Array<String> tags() const override
    {
        return this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::Tag);
    }
    void setTags(const Array<String>&values) override
    {
        this->p_ensureTrackIsLoaded().setTags(values);
        this->p_wasEdited = true;
    }

    Array<String> artists() const override
    {
        return this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::ArtistCredit);
    }
    void setArtists(const Array<String>&values) override
    {
        this->p_ensureTrackIsLoaded().setArtists(values);
        this->p_wasEdited = true;
    }

    Array<String> producers() const override
    {
        return this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::ProducerCredit);
    }
    void setProducers(const Array<String>&values) override
    {
        this->p_ensureTrackIsLoaded().setProducers(values);
        this->p_wasEdited = true;
    }

    Array<String> remixers() const override
    {
        return this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::RemixerCredit);
    }
    void setRemixers(const Array<String>&values) override
    {
        this->p_ensureTrackIsLoaded().setRemixers(values);
        this->p_wasEdited = true;
    }

    Optional<Date> maybeDateAdded() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::DateAdded);
        return results.length() ? Date::maybeDateWithString(results[0]) : nothing;
    }
    void setDateAdded(const Optional<Date>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setDateAdded(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<Date> maybeDateReleased() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().stringValuesForTagOfType(Common::Property::TypeID::DateReleased);
        return results.length() ? Date::maybeDateWithString(results[0]) : nothing;
    }
    void setDateReleased(const Optional<Date>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setDateReleased(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<boolean> maybeBeatGridLocked() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::BeatGridLockedFlag);
        return results.length() && (results[0] != 0);
    }
    void setBeatGridLocked(boolean value) override
    {
        this->p_ensureTrackIsLoaded().setBeatGridLocked(value);
        this->p_wasEdited = true;
    }

    Optional<count> maybeBitDepthInBits() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::BitDepthInBits);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setBitDepthInBits(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setBitDepthInBits(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeBitRateInKiloBitsPerSecond() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::BitRateInKiloBitsPerSecond);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setBitRateInKiloBitsPerSecond(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setBitRateInKiloBitsPerSecond(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<DecimalNumber> maybeBeatsPerMinute() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().decimalValuesForTagOfType(Common::Property::TypeID::BeatsPerMinute);
        return results.length() ? Optional<DecimalNumber>{ results[0] } : nothing;
    }
    void setBeatsPerMinute(const Optional<DecimalNumber>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setBeatsPerMinute(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<Color> maybeColor() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::Color);
        return results.length() ? Optional<Color>{ inPlace, static_cast<uinteger32>((results[0]) << 8) | 0xff } : nothing;
    }
    void setColor(const Optional<Color>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setColor(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeFileSizeInBytes() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::FileSize);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setFileSizeInBytes(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setFileSizeInBytes(maybeValue);
        this->p_wasEdited = true;
    }

    AudioFileType fileType() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::FileType);
        if (results.length()) {
            auto integerValue = results[0];
            if (static_cast<uinteger32>(integerValue) <= static_cast<uinteger32>(AudioFileType::Maximum)) {
                return static_cast<AudioFileType>(integerValue);
            }
        }

        return AudioFileType::Unknown;
    }
    void setFileType(AudioFileType value) override
    {
        // -- If we are using the Rekord Buddy app, we store the real audio file type internally,
        // -- no matter what is passed here. This is because the type may be used later
        // -- to determine what to do with the track when copying it to other collections.
        value = exactTypeForAudioFileAt(this->absoluteFilePath());
        this->p_ensureTrackIsLoaded().setFileType(value);
        this->p_wasEdited = true;
    }

    Array<String> musicalKeys() const override
    {
        MutableArray<String> results;

        for (auto&& integerValue : this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::MusicalKey)) {
            if (static_cast<uinteger32>(integerValue) <= static_cast<uinteger32>(Common::MusicalKey::Value::Maximum)) {
                results.append(Common::MusicalKey::stringValueForKey(static_cast<Common::MusicalKey::Value>(integerValue)));
            }
        }

        return { std::move(results) };
    }
    void setMusicalKeys(const Array<String>& values) override
    {
        this->p_ensureTrackIsLoaded().setMusicalKeys(values);
        this->p_wasEdited = true;
    }

    Optional<DecimalNumber> maybeLengthInSeconds() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().decimalValuesForTagOfType(Common::Property::TypeID::LengthInSeconds);
        if (results.length() == 0) {
            return nothing;
        }
        return results[0];
    }
    void setLengthInSeconds(const Optional<DecimalNumber>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setLengthInSeconds(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeTrackNumber() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::TrackNumber);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setTrackNumber(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setTrackNumber(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeDiscNumber() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::DiscNumber);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setDiscNumber(const Optional<count>& maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setDiscNumber(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybePlayCount() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::PlayCount);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setPlayCount(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setPlayCount(maybeValue);
        this->p_wasEdited = true;
    }

    String ratingAsString() const override
    {
        return Common::Track::ratingAsStringFor(*this);
    }
    Optional<Common::TrackRating> maybeRating() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::Rating);
        if (!results.length()) {
            return nothing;
        }

        return Common::TrackRating::maybeWithValue(results[0]);
    }
    void setRating(const Optional<Common::TrackRating>& maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setRating(maybeValue);
        this->p_wasEdited = true;
    }

    Optional<count> maybeSampleRateInHertz() const override
    {
        auto results = this->p_ensureTrackDisplayCacheIsLoaded().integerValuesForTagOfType(Common::Property::TypeID::SampleRateInHertz);
        return results.length() ? Optional<count>{ results[0] } : nothing;
    }
    void setSampleRateInHertz(const Optional<count>&maybeValue) override
    {
        this->p_ensureTrackIsLoaded().setSampleRateInHertz(maybeValue);
        this->p_wasEdited = true;
    }

    count numberOfMarkers() const override
    {
        return this->p_ensureMarkersAreLoaded().length();
    }
    Array<Common::MarkerOfSomeSort> markers() const override
    {
        MutableArray<Common::MarkerOfSomeSort> results;

        for (auto&& marker : this->p_ensureMarkersAreLoaded()) {
            withVariant(*marker, [&results](auto& marker) {
                results.emplaceAppend(&marker.asImmutableReference());
            });
        }

        return std::move(results);
    }
    Array<Common::MutableMarkerOfSomeSort> markers() override
    {
        MutableArray<Common::MutableMarkerOfSomeSort> results;

        for (auto&& marker : this->p_ensureMarkersAreLoaded()) {
            withVariant(*marker, [&results](auto& marker) {
                results.emplaceAppend(&marker);
            });
        }

        return std::move(results);
    }
    Common::MarkerOfSomeSort markerAtIndex(count index) const override
    {
        return withVariant(*this->p_ensureMarkersAreLoaded()[index], [](auto& marker) {
            return Common::MarkerOfSomeSort{ &marker };
        });
    }
    Common::MutableMarkerOfSomeSort markerAtIndex(count index) override
    {
        return withVariant(*this->p_ensureMarkersAreLoaded()[index], [](auto& marker) {
            return Common::MutableMarkerOfSomeSort{ &marker };
        });
    }
    NotNull<Common::MutableCueMarker *> appendCueMarker() override
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<MutableMarkerOfSomeSort>::with(MutableCueMarker{ this->asRekordBuddyTrack(),
                                                                               MutableCueMarker::p_isProtected }));

        return &markers.lastObject()->get<MutableCueMarker>();
    }
    NotNull<Common::MutableGridMarker *> appendGridMarker() override
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<MutableMarkerOfSomeSort>::with(MutableGridMarker{ this->asRekordBuddyTrack(),
                                                                                MutableGridMarker::p_isProtected }));

        return &markers.lastObject()->get<MutableGridMarker>();
    }
    NotNull<Common::MutableLoopMarker *> appendLoopMarker() override
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<MutableMarkerOfSomeSort>::with(MutableLoopMarker{ this->asRekordBuddyTrack(),
                                                                                MutableLoopMarker::p_isProtected }));

        return &markers.lastObject()->get<MutableLoopMarker>();
    }
    void removeMarkerAtIndex(count index) override
    {
        auto& markers = this->p_ensureMarkersAreLoaded();

        withVariant(*markers[index], [](auto& marker) {
            marker.deleteFromCollection();
        });

        markers.removeObjectAtIndex(index);

        this->p_ensureTrackIsLoaded().asPersistent()->removeMarkersItemAtIndex(index);
    }
    void setMarkersAndMaybeAddOffsetInSeconds(const Array<Common::MarkerOfSomeSort>& markers, Optional<DecimalNumber> maybeOffset = nothing) override
    {
        auto& existingMarkers = this->p_ensureMarkersAreLoaded();
        for (auto&& marker : existingMarkers) {
            withVariant(*marker, [](auto& marker) {
                marker.deleteFromCollection();
            });
        }

        existingMarkers.removeAll();

        for (auto&& marker : markers) {
            withVariant(marker, [this, &maybeOffset](auto& marker) {
                this->p_appendCommonMarkerAndMaybeAddOffsetInSeconds(marker, maybeOffset);
            });
        }
    }
    Optional<DecimalNumber> maybeOffsetToAddInSecondsWhenImportingFrom(Common::Collection::Type) const;
    void updateMarkerOffsets();

    CollectionImplementation::ObjectID reference() const
    {
        return this->p_trackObjectID;
    }

    std::shared_ptr<CollectionImplementation::PersistentTrack> asPersistent()
    {
        return this->p_ensureTrackIsLoaded().asPersistent();
    }

    const CollectionImplementation::Track& asRekordBuddyTrack() const
    {
        return this->p_ensureTrackIsLoaded();
    }
    CollectionImplementation::Track& asRekordBuddyTrack()
    {
        return this->p_ensureTrackIsLoaded();
    }
};

} }
