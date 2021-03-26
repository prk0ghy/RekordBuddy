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

#include <SeratoCollectionImplementation/Track.hpp>
#include <SeratoCollectionImplementation/Crate.hpp>

#include <TrackFiles/TrackFile.hpp>

namespace NxA { namespace Serato {

namespace CollectionImplementation {

class Database;

}

// -- Internal Interface
class MutableTrack : public Common::MutableTrack, public Common::Track
{
    // -- Friends
    friend class CollectionImplementation::Database;

#if defined(NXA_BUILD_FOR_TESTING)
    friend class CollectionImplementation::CrateTests;
#endif

    // -- Private Instance Variables
    CollectionImplementation::Track p_seratoTrack;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constructors & Destructors
    MutableTrack(CollectionImplementation::Track&& from, const Protected&) :  p_seratoTrack{ std::move(from) } { }
    ~MutableTrack() override = default;

    // -- Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Volume volume() const override;
    FilePath relativeFilePath() const override
    {
        return this->p_seratoTrack.filePathRelativeToDatabaseVolume();
    }
    FilePath absoluteFilePath() const override
    {
        return Common::Track::absolutePathFor(*this);
    }

    Time lastModificationTime() const override
    {
        return { this->p_seratoTrack.timeLastModifiedInSecondsSinceJanuary1st1970() };
    }
    void markAsModifiedNow() override;

    Optional<String> maybeTitle() const override
    {
        return this->p_seratoTrack.title();
    }
    void setTitle(const Optional<String>& maybeValue) override
    {
        this->p_seratoTrack.setTitle(maybeValue.valueOr(String{ }));
    }

    Optional<String> maybeAlbum() const override
    {
        return this->p_seratoTrack.album();
    }
    void setAlbum(const Optional<String>& maybeValue) override
    {
        this->p_seratoTrack.setAlbum(maybeValue.valueOr(String{ }));
    }

    Optional<count> maybeAlbumTrackCount() const override
    {
        return nothing;
    }
    void setAlbumTrackCount(const Optional<count>& maybeValue) override
    {
    }

    Optional<String> maybeComments() const override
    {
        return this->p_seratoTrack.comments();
    }
    void setComments(const Optional<String>& maybeValue) override
    {
        this->p_seratoTrack.setComments(maybeValue.valueOr(String{ }));
    }

    Array<String> genres() const override
    {
        return this->p_seratoTrack.genre().splitBySeparator(", ");
    }
    void setGenres(const Array<String>& values) override
    {
        this->p_seratoTrack.setGenre(String::stringByJoiningArrayWithString(values, ", "));
    }

    Optional<String> maybeGrouping() const override
    {
        return this->p_seratoTrack.grouping();
    }
    void setGrouping(const Optional<String>& maybeValue) override
    {
        this->p_seratoTrack.setGrouping(maybeValue.valueOr(String{ }));
    }

    Optional<String> maybeMixName() const override
    {
        // -- This is not supported by Serato.
        return nothing;
    }
    void setMixName(const Optional<String>& maybeValue) override
    {
        // -- This is not supported by Serato.
    }

    Optional<String> maybeRecordLabel() const override
    {
        return this->p_seratoTrack.recordLabel();
    }
    void setRecordLabel(const Optional<String>& maybeValue) override
    {
        this->p_seratoTrack.setRecordLabel(maybeValue.valueOr(String{ }));
    }

    Array<String> tags() const override
    {
        // -- This is not supported by Serato.
        return { };
    }
    void setTags(const Array<String>& values) override
    {
        // -- This is not supported by Serato.
    }

    Array<String> artists() const override;
    void setArtists(const Array<String>& values) override;

    Array<String> producers() const override;
    void setProducers(const Array<String>& values) override;

    Array<String> remixers() const override;
    void setRemixers(const Array<String>& values) override;

    Optional<Date> maybeDateAdded() const override
    {
        auto dateAdded = this->p_seratoTrack.dateAddedInSecondsSinceJanuary1st1970();
        if (!dateAdded) {
            return nothing;
        }

        return Date::inLocalTimeZoneFromTime(Time{ dateAdded });
    }
    void setDateAdded(const Optional<Date>& maybeValue) override
    {
        timestamp dateAsTimeStamp = 0;

        if (maybeValue.isValid()) {
            dateAsTimeStamp = maybeValue->asTimeConvertedFromLocalTimeZone().asUnixTimeStamp();
        }

        this->p_seratoTrack.setDateAddedInSecondsSinceJanuary1st1970(dateAsTimeStamp);
    }

    Optional<Date> maybeDateReleased() const override
    {
        auto yearReleased = this->p_seratoTrack.year().integerValue();
        if (yearReleased <= 0) {
            return nothing;
        }

        return Date::maybeDateWithYear(yearReleased);
    }
    void setDateReleased(const Optional<Date>& value) override
    {
        this->p_seratoTrack.setYear(value.isValid() ? value->asStringWithJustYear() : ""_String);
    }

    Optional<boolean> maybeBeatGridLocked() const override
    {
        return this->p_seratoTrack.beatGridIsLocked();
    }
    void setBeatGridLocked(boolean value) override
    {
        this->p_seratoTrack.setBeatGridIsLocked(value);
    }

    Optional<count> maybeBitDepthInBits() const override
    {
        // -- This is not supported by Serato.
        return nothing;
    }
    void setBitDepthInBits(const Optional<count>& maybeValue) override
    {
        // -- This is not supported by Serato.
    }

    Optional<count> maybeBitRateInKiloBitsPerSecond() const override
    {
        return this->p_seratoTrack.bitRateInKiloBitsPerSecond();
    }
    void setBitRateInKiloBitsPerSecond(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setBitRateInKiloBitsPerSecond(maybeValue.valueOr(0));
    }

    Optional<DecimalNumber> maybeBeatsPerMinute() const override
    {
        return this->p_seratoTrack.bpm().decimalValue();
    }
    void setBeatsPerMinute(const Optional<DecimalNumber>& maybeValue) override
    {
        this->p_seratoTrack.setBpm(maybeValue.isValid() ? maybeValue->asStringWithFractionDigitsBetween(0, 1) : String{ });
    }

    Optional<Color> maybeColor() const override
    {
        auto result = this->p_seratoTrack.color();
        return result.asRGB() ? Optional<Color>{ result } : nothing;
    }
    void setColor(const Optional<Color>& maybeValue) override
    {
        this->p_seratoTrack.setColor(maybeValue.valueOr(Color(0, 0, 0)));
    }

    Optional<count> maybeFileSizeInBytes() const override
    {
        auto result = this->p_seratoTrack.sizeInBytes();
        return result ? Optional<count>{ result } : nothing;
    }
    void setFileSizeInBytes(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setSizeInBytes(maybeValue.valueOr(0));
    }

    AudioFileType fileType() const override;
    void setFileType(AudioFileType value) override
    {
        // -- This is not supported by Serato.
    }

    Array<String> musicalKeys() const override
    {
        auto value = this->p_seratoTrack.key();
        if (value.length() != 0) {
            return { value };
        }

        return { };
    }
    void setMusicalKeys(const Array<String>& values) override
    {
        if (values.length() > 0) {
            auto maybeConvertedValue = Common::MusicalKey::maybeStringValueInDefaultNotationFromString(values.firstObject());
            if (maybeConvertedValue.isValid()) {
                this->p_seratoTrack.setKey(*maybeConvertedValue);
                return;
            }
        }

        this->p_seratoTrack.setKey({ });
    }

    Optional<DecimalNumber> maybeLengthInSeconds() const override
    {
        auto result = this->p_seratoTrack.lengthInMilliSeconds();
        if (result == 0) {
            return nothing;
        }
        return DecimalNumber::withIntegerAndExponant(result, -3);
    }
    void setLengthInSeconds(const Optional<DecimalNumber>& maybeValue) override
    {
        this->p_seratoTrack.setLengthInMilliSeconds(maybeValue.isValid() ? (*maybeValue * 1000).asInteger() : 0);
    }

    Optional<count> maybeTrackNumber() const override
    {
        auto result = this->p_seratoTrack.trackNumber();
        return result ? Optional<count>{ result } : nothing;
    }
    void setTrackNumber(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setTrackNumber(maybeValue.valueOr(0));
    }

    Optional<count> maybeDiscNumber() const override
    {
        auto result = this->p_seratoTrack.discNumber();
        return result ? Optional<count>{ result } : nothing;
    }
    void setDiscNumber(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setDiscNumber(maybeValue.valueOr(0));
    }

    Optional<count> maybePlayCount() const override
    {
        auto result = this->p_seratoTrack.playCount();
        return result ? Optional<count>{ result } : nothing;
    }
    void setPlayCount(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setPlayCount(maybeValue.valueOr(0));
    }

    String ratingAsString() const override
    {
        return Common::Track::ratingAsStringFor(*this);
    }
    Optional<Common::TrackRating> maybeRating() const override
    {
        // -- This is not supported by Serato.
        return nothing;
    }
    virtual void setRating(const Optional<Common::TrackRating>&) override
    {
        // -- This is not supported by Serato.
    }

    Optional<count> maybeSampleRateInHertz() const override
    {
        auto result = this->p_seratoTrack.sampleRateInSamplesPerSecond();
        return result ? Optional<count>{ result } : nothing;
    }
    void setSampleRateInHertz(const Optional<count>& maybeValue) override
    {
        this->p_seratoTrack.setSampleRateInSamplesPerSecond(maybeValue.valueOr(0));
    }

    count numberOfMarkers() const override
    {
        return this->p_seratoTrack.numberOfMarkers();
    }
    Common::MarkerOfSomeSort markerAtIndex(count index) const override
    {
        return this->p_seratoTrack.markerAtIndex(index);
    }
    Common::MutableMarkerOfSomeSort markerAtIndex(count index) override
    {
        return this->p_seratoTrack.markerAtIndex(index);
    }
    Array<Common::MarkerOfSomeSort> markers() const override
    {
        return this->p_seratoTrack.markers();
    }
    Array<Common::MutableMarkerOfSomeSort> markers() override
    {
        return this->p_seratoTrack.markers();
    }
    NotNull<Common::MutableCueMarker *> appendCueMarker() override
    {
        return this->p_seratoTrack.appendCueMarker();
    }
    NotNull<Common::MutableGridMarker *> appendGridMarker() override
    {
        auto newMarker = this->p_seratoTrack.appendGridMarker();

        // -- We try to set some sensible defaults given this collection type, in this case the average track bpm or 123bpm.
        newMarker->setBeatsPerMinute(this->maybeBeatsPerMinute().valueOr(DecimalNumber::withInteger(123)));

        return newMarker;
    }
    NotNull<Common::MutableLoopMarker *> appendLoopMarker() override
    {
        auto newMarker = this->p_seratoTrack.appendLoopMarker();

        // -- We try to set some sensible defaults given this collection type, in this case a 4 beat default length.
        newMarker->setLengthInSeconds((DecimalNumber::withInteger(60) / this->maybeBeatsPerMinute().valueOr(DecimalNumber::withInteger(123))) * 4);

        return newMarker;
    }
    void removeMarkerAtIndex(count index) override
    {
        this->p_seratoTrack.removeMarkerAtIndex(index);
    }
    void setMarkersAndMaybeAddOffsetInSeconds(const Array<Common::MarkerOfSomeSort>&, Optional<DecimalNumber> = nothing) override;

    inline const CollectionImplementation::Track& asSeratoTrack() const
    {
        return this->p_seratoTrack;
    }
    inline CollectionImplementation::Track& asSeratoTrack()
    {
        return this->p_seratoTrack;
    }
};

} }
