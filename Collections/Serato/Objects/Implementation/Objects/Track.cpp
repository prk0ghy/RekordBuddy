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

#include <SeratoCollection/Collection.hpp>

#include <SeratoCollectionImplementation/Track.hpp>
#include <SeratoCollectionImplementation/Tags/TextTag.hpp>
#include <SeratoCollectionImplementation/Tags/BooleanTag.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>
#include <SeratoCollectionImplementation/Tags/DatabaseV2Tags.hpp>
#include <SeratoCollectionImplementation/Database.hpp>

#include <CommonCollection/Tracks/TrackColor.hpp>

#include <TrackFiles/TrackFileFactory.hpp>

#include <Base/Base.hpp>

#include <cstdlib>

using namespace NxA;
using namespace NxA::Common;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

// -- Class Methods

#if defined(NXA_PRINT_SERATO_TRACK_DEBUG_INFO)
void CollectionImplementation::Track::debugPrintString(const String& text, const String& name)
{
    NXA_DLOG_WITH_FORMAT("%s '%s'", name.asUTF8(), text.asUTF8());
}

void CollectionImplementation::Track::debugPrintUint(uinteger32 value, const String& name)
{
    NXA_DLOG_WITH_FORMAT("%s '%d'", name.asUTF8(), value);
}

void CollectionImplementation::Track::debugPrintDate(timestamp value, const String& name)
{
    NXA_DLOG_WITH_FORMAT("%s %s", name.asUTF8(), Time{ value }.stringValueInLocalTimeZone().asUTF8());
}

void CollectionImplementation::Track::debugPrint(const Track& track)
{
    CollectionImplementation::Track::debugPrintUint(static_cast<uinteger32>(track.sizeInBytes()), String("size"));
    CollectionImplementation::Track::debugPrintDate(track.timeLastModifiedInSecondsSinceJanuary1st1970(), String("timelastmodified"));
    CollectionImplementation::Track::debugPrintDate(track.dateAddedInSecondsSinceJanuary1st1970(), String("dateadded"));

    CollectionImplementation::Track::debugPrintString(track.title(), String("title"));
    CollectionImplementation::Track::debugPrintString(track.artist(), String("artist"));
    CollectionImplementation::Track::debugPrintString(track.album(), String("album"));
    CollectionImplementation::Track::debugPrintString(track.comments(), String("comments"));
    CollectionImplementation::Track::debugPrintString(track.genre(), String("genre"));
    CollectionImplementation::Track::debugPrintString(track.grouping(), String("grouping"));
    CollectionImplementation::Track::debugPrintString(track.recordLabel(), String("recordlabel"));
    CollectionImplementation::Track::debugPrintString(track.remixer(), String("remixer"));
    CollectionImplementation::Track::debugPrintString(track.composer(), String("composer"));
    CollectionImplementation::Track::debugPrintString(track.key(), String("key"));
    CollectionImplementation::Track::debugPrintString(track.year(), String("year"));
    CollectionImplementation::Track::debugPrintUint(track.lengthInMilliSeconds(), String("length"));
    CollectionImplementation::Track::debugPrintUint(track.bitRateInKiloBitsPerSecond(), String("bitrate"));
    CollectionImplementation::Track::debugPrintUint(track.sampleRateInSamplesPerSecond(), String("samplerate"));
    CollectionImplementation::Track::debugPrintString(track.bpm(), String("bpm"));
    CollectionImplementation::Track::debugPrintUint(static_cast<uinteger32>(track.trackNumber()), String("tracknumber"));
    CollectionImplementation::Track::debugPrintUint(static_cast<uinteger32>(track.discNumber()), String("discnumber"));
}
#endif

// -- Constructors & Destructors

CollectionImplementation::Track::Track(const FilePath& withRelativeFilePath,
                                       Pointer<Serato::MutableCollection> inCollection,
                                       const Protected&) : p_collection(inCollection)
{
    MutableArray<CollectionImplementation::Tag> tags;
    tags.append(CollectionImplementation::PathTag::tagWithIdentifierAndValue(CollectionImplementation::trackFilePathTagIdentifier, withRelativeFilePath));

    this->p_trackTag = CollectionImplementation::ObjectTag::tagWithIdentifierAndValue(CollectionImplementation::trackObjectTagIdentifier, std::move(tags));

    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackBeatGridIsLockedTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackIsCorruptedTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(true, CollectionImplementation::trackMetadataWasReadTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackIsReadOnlyTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackIsFromItunesTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackLoopingTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackFileIsMissingTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackWasRecentlyPlayedTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackFileTypeIsUnsupportedTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackIsWhiteLabelTagIdentifier);
    this->p_setBooleanForSubTagForIdentifier(false, CollectionImplementation::trackIsAccessControlledWhiteLabelTagIdentifier);
    this->p_setUInt32ForSubTagForIdentifier(0, CollectionImplementation::trackLabelColorTagIdentifier);

    const character* fileTypeString = nullptr;
    switch (genericTypeForAudioFileAt(withRelativeFilePath)) {
        case AudioFileType::AIFF: {
            fileTypeString = "aiff";
            break;
        }
        case AudioFileType::MP3: {
            fileTypeString = "mp3";
            break;
        }
        case AudioFileType::WAV: {
            fileTypeString = "wave";
            break;
        }
        case AudioFileType::OGG: {
            fileTypeString = "oggvorbis";
            break;
        }
        case AudioFileType::MP4: {
            fileTypeString = "quicktime";
            break;
        }
        case AudioFileType::FLAC: {
            fileTypeString = "flac";
            break;
        }
        default: {
            fileTypeString = "unknown";
        }
    }

    this->p_setStringForSubTagForIdentifier(String(fileTypeString), CollectionImplementation::trackFileTypeTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

// -- Operators

inline bool CollectionImplementation::Track::operator==(const Track& other) const noexcept
{
    return (this->p_collection == other.p_collection) &&
           (this->filePathRelativeToDatabaseVolume() == other.filePathRelativeToDatabaseVolume());
}

// -- Instance Methods

Pointer<const Serato::MutableCollection> CollectionImplementation::Track::collection() const
{
    return this->p_collection;
}

Shared<Serato::MutableMarkerOfSomeSort> CollectionImplementation::Track::p_mutableMarkerWith(SeratoCueMarker&& from) const
{
    return Shared<Serato::MutableMarkerOfSomeSort>::with(Serato::MutableCueMarker{ std::move(from),
                                                                                   *const_cast<CollectionImplementation::Track*>(this),
                                                                                   Serato::MutableCueMarker::p_isProtected });
}

Shared<Serato::MutableMarkerOfSomeSort> CollectionImplementation::Track::p_mutableMarkerWith(SeratoGridMarker&& from) const
{
    return Shared<Serato::MutableMarkerOfSomeSort>::with(Serato::MutableGridMarker{ std::move(from),
                                                                                    *const_cast<CollectionImplementation::Track*>(this),
                                                                                    Serato::MutableGridMarker::p_isProtected });
}

Shared<Serato::MutableMarkerOfSomeSort> CollectionImplementation::Track::p_mutableMarkerWith(SeratoLoopMarker&& from) const
{
    return Shared<Serato::MutableMarkerOfSomeSort>::with(Serato::MutableLoopMarker{ std::move(from),
                                                                                    *const_cast<CollectionImplementation::Track*>(this),
                                                                                    Serato::MutableLoopMarker::p_isProtected });
}

MutableArray<Shared<Serato::MutableMarkerOfSomeSort>>& CollectionImplementation::Track::p_ensureMarkersAreLoaded() const
{
    if (!this->p_maybeMarkers.isValid()) {
        this->p_maybeMarkers = Optional<MutableArray<Shared<Serato::MutableMarkerOfSomeSort>>>{ inPlace };

        auto fullFilePath = FilePath::filePathByJoiningPaths((*this->p_collection).volume(),
                                                             this->filePathRelativeToDatabaseVolume());
        auto maybeTrackFile = TrackFileFactory::maybeTrackFileForPath(fullFilePath);
        if (maybeTrackFile.isValid()) {
            try {
                auto& markers = *this->p_maybeMarkers;

                for (auto&& marker : maybeTrackFile->seratoMarkers()) {
                    withVariant(marker, [this, &markers](auto&& marker) {
                        markers.append(this->p_mutableMarkerWith(std::move(marker)));
                    });
                }
            }
            catch (...) {
                NXA_DLOG_WITH_FORMAT("Error reading markers from file '%s'.\n",
                                     fullFilePath.asEncodedString().asUTF8());
            }
        }
    }

    return *this->p_maybeMarkers;
}

Optional<String> CollectionImplementation::Track::p_maybeStringForSubTagForIdentifier(uinteger32 identifier) const
{
    auto& trackObjectTag = this->p_trackTag;
    auto maybeSubTag = trackObjectTag.maybeSubTagForIdentifier(identifier);
    if (maybeSubTag.isValid()) {
        return static_cast<const TextTag&>(*maybeSubTag).value();
    }

    return nothing;
}

Optional<FilePath> CollectionImplementation::Track::p_maybePathForSubTagForIdentifier(uinteger32 identifier) const
{
    auto& trackObjectTag = this->p_trackTag;
    auto maybeSubTag = trackObjectTag.maybeSubTagForIdentifier(identifier);
    if (maybeSubTag.isValid()) {
        return FilePath(static_cast<const PathTag&>(*maybeSubTag).value());
    }

    return nothing;
}

Optional<uinteger32> CollectionImplementation::Track::p_maybeUint32ForSubTagForIdentifier(uinteger32 identifier) const
{
    auto& trackObjectTag = this->p_trackTag;
    auto maybeSubTag = trackObjectTag.maybeSubTagForIdentifier(identifier);
    if (maybeSubTag.isValid()) {
        return static_cast<const UInteger32Tag&>(*maybeSubTag).value();
    }

    return nothing;
}

Optional<boolean> CollectionImplementation::Track::p_maybeBooleanForSubTagForIdentifier(uinteger32 identifier) const
{
    auto& trackObjectTag = this->p_trackTag;
    auto maybeSubTag = trackObjectTag.maybeSubTagForIdentifier(identifier);
    if (maybeSubTag.isValid()) {
        return static_cast<const BooleanTag&>(*maybeSubTag).value();
    }

    return nothing;
}

void CollectionImplementation::Track::p_setStringForSubTagForIdentifier(const String& value, uinteger32 identifier)
{
    this->p_trackTag.setSubTag(TextTag::tagWithIdentifierAndValue(identifier, value));
    this->p_needsToUpdateDatabaseFile = true;
}

void CollectionImplementation::Track::p_setPathForSubTagForIdentifier(const FilePath& value, uinteger32 identifier)
{
    this->p_trackTag.setSubTag(PathTag::tagWithIdentifierAndValue(identifier, value));
    this->p_needsToUpdateDatabaseFile = true;
}

void CollectionImplementation::Track::p_setUInt32ForSubTagForIdentifier(uinteger32 value, uinteger32 identifier)
{
    this->p_trackTag.setSubTag(UInteger32Tag::tagWithIdentifierAndValue(identifier, value));
    this->p_needsToUpdateDatabaseFile = true;
}

void CollectionImplementation::Track::p_setBooleanForSubTagForIdentifier(boolean value, uinteger32 identifier)
{
    this->p_trackTag.setSubTag(BooleanTag::tagWithIdentifierAndValue(identifier, value));
    this->p_needsToUpdateDatabaseFile = true;
}

FilePath CollectionImplementation::Track::filePathRelativeToDatabaseVolume() const
{
    auto path = this->p_maybePathForSubTagForIdentifier(trackFilePathTagIdentifier);
    NXA_ASSERT_TRUE(path.isValid());
    return *path;
}

String CollectionImplementation::Track::title() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackTitleTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setTitle(const String& title)
{
    this->p_setStringForSubTagForIdentifier(title, trackTitleTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::artist() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackArtistTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setArtist(const String& artist)
{
    this->p_setStringForSubTagForIdentifier(artist, trackArtistTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::album() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackAlbumTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setAlbum(const String& album)
{
    this->p_setStringForSubTagForIdentifier(album, trackAlbumTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::genre() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackGenreTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setGenre(const String& genre)
{
    this->p_setStringForSubTagForIdentifier(genre, trackGenreTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::comments() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackCommentsTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setComments(const String& comments)
{
    this->p_setStringForSubTagForIdentifier(comments, trackCommentsTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::grouping() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackGroupingTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setGrouping(const String& grouping)
{
    this->p_setStringForSubTagForIdentifier(grouping, trackGroupingTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::remixer() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackRemixerTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setRemixer(const String& remixer)
{
    this->p_setStringForSubTagForIdentifier(remixer, trackRemixerTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::recordLabel() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackLabelTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setRecordLabel(const String& recordLabel)
{
    this->p_setStringForSubTagForIdentifier(recordLabel, trackLabelTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::composer() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackComposerTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setComposer(const String& composer)
{
    this->p_setStringForSubTagForIdentifier(composer, trackComposerTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::key() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackKeyTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setKey(const String& key)
{
    this->p_setStringForSubTagForIdentifier(key, trackKeyTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::bpm() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackBpmTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setBpm(const String& bpm)
{
    this->p_setStringForSubTagForIdentifier(bpm, trackBpmTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

String CollectionImplementation::Track::year() const
{
    auto result = this->p_maybeStringForSubTagForIdentifier(trackYearTagIdentifier);
    return result ? *result : String{ };
}

void CollectionImplementation::Track::setYear(const String& year)
{
    this->p_setStringForSubTagForIdentifier(year, trackYearTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

count CollectionImplementation::Track::trackNumber() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackNumberTagIdentifier);
    return result ? *result : 0;
}

void CollectionImplementation::Track::setTrackNumber(count trackNumber)
{
    // TODO: Should make sure the cast doesn't chop off anything.
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(trackNumber), trackNumberTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

count CollectionImplementation::Track::discNumber() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackDiscNumberTagIdentifier);
    return result ? *result : 0;
}

void CollectionImplementation::Track::setDiscNumber(count discNumber)
{
    // TODO: Should make sure the cast doesn't chop off anything.
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(discNumber), trackDiscNumberTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

count CollectionImplementation::Track::playCount() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackPlayCount);
    return result ? *result : 0;
}

void CollectionImplementation::Track::setPlayCount(count playCount)
{
    // TODO: Should make sure the cast doesn't chop off anything.
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(playCount), trackPlayCount);
    this->p_needsToUpdateDatabaseFile = true;
}

Color CollectionImplementation::Track::color() const
{
    auto maybeResult = this->p_maybeUint32ForSubTagForIdentifier(trackLabelColorTagIdentifier);
    return maybeResult ? Color{ (static_cast<uinteger32>(*maybeResult) << 8) | 0xff } : Color{ 0, 0, 0 };
}

void CollectionImplementation::Track::setColor(const Color& color)
{
    auto maybeColor = TrackColor::maybeColorForTrackColorWhenExportedTo(color, Common::Collection::Type::Serato);
    this->p_setUInt32ForSubTagForIdentifier(maybeColor.isValid() ? maybeColor->asRGB() : 0u,
                                            trackLabelColorTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

timestamp CollectionImplementation::Track::timeLastModifiedInSecondsSinceJanuary1st1970() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackModificationTimeTagIdentifier);
    return result ? *result : 0;
}

void CollectionImplementation::Track::setTimeLastModifiedInSecondsSinceJanuary1st1970(timestamp timeLastModified)
{
    // TODO: Should make sure the cast doesn't chop off anything.
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(timeLastModified), trackModificationTimeTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

timestamp CollectionImplementation::Track::dateAddedInSecondsSinceJanuary1st1970() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackDateAddedTagIdentifier);
    return result ? *result : 0;
}

void CollectionImplementation::Track::setDateAddedInSecondsSinceJanuary1st1970(timestamp dateAdded)
{
    auto dateAsString = Time{ dateAdded }.stringValueInLocalTimeZoneUsingFormat("%d-%m-%Y");

    // TODO: Should make sure the cast doesn't chop off anything.
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(dateAdded), trackDateAddedTagIdentifier);
    this->p_setStringForSubTagForIdentifier(dateAsString, trackDateAddedStringTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
}

boolean CollectionImplementation::Track::beatGridIsLocked() const
{
    auto result = this->p_maybeBooleanForSubTagForIdentifier(trackBeatGridIsLockedTagIdentifier);
    return result ? *result : false;
}

void CollectionImplementation::Track::setBeatGridIsLocked(boolean locked)
{
    this->p_setBooleanForSubTagForIdentifier(locked, trackBeatGridIsLockedTagIdentifier);
    this->p_needsToUpdateDatabaseFile = true;
    this->p_needsToUpdateTrackFile = true;
}

count CollectionImplementation::Track::sizeInBytes() const
{
    auto result = this->p_maybeUint32ForSubTagForIdentifier(trackSizeTagIdentifier);
    if (!result.isValid()) {
        return 0;
    }

    return result ? *result : 0;
}

void CollectionImplementation::Track::setSizeInBytes(count size)
{
    this->p_setUInt32ForSubTagForIdentifier(static_cast<uinteger32>(size), trackSizeTagIdentifier);

    double sizeInMb = static_cast<double>(size) / (1024.0f * 1024.0f);
    auto sizeString = String::stringWithFormat("%.1fMB", sizeInMb);
    this->p_setStringForSubTagForIdentifier(sizeString, trackSizeStringTagIdentifier);

    this->p_needsToUpdateDatabaseFile = true;
}

uinteger32 CollectionImplementation::Track::lengthInMilliSeconds() const
{
    uinteger32 result = 0;

    auto maybeAsString = this->p_maybeStringForSubTagForIdentifier(trackLengthTagIdentifier);
    if (maybeAsString.isValid() && (maybeAsString->length() > 6)) {
        auto components = maybeAsString->splitBySeparator(':');
        if (components.length() == 2) {
            auto otherComponents = components[1].splitBySeparator('.');
            if (otherComponents.length() == 2) {
                auto minutes = components[0].integerValue();
                auto seconds = otherComponents[0].integerValue();
                auto milliSeconds = otherComponents[1].integerValue();

                result = milliSeconds + (seconds * 1000) + (minutes * 60 * 1000);
            }
        }
    }

    return result;
}

void CollectionImplementation::Track::setLengthInMilliSeconds(uinteger32 length)
{
    uinteger32 minutes = length / 60000;
    uinteger32 milliseconds = length % 1000;
    uinteger32 seconds = (length - milliseconds - (minutes * 60000)) / 1000;

    milliseconds *= 60;
    milliseconds /= 1000;

    auto lengthString = String::stringWithFormat("%02d:%02d.%02d", minutes, seconds, milliseconds);
    this->p_setStringForSubTagForIdentifier(lengthString, trackLengthTagIdentifier);

    this->p_needsToUpdateDatabaseFile = true;
}

uinteger32 CollectionImplementation::Track::bitRateInKiloBitsPerSecond() const
{
    uinteger32 result = 0;

    auto maybeAsString = this->p_maybeStringForSubTagForIdentifier(trackBitrateTagIdentifier);
    if (maybeAsString.isValid() && (maybeAsString->length() > 6)) {
        // -- Valid strings should end with ".0kbps"
        result = maybeAsString->subString(0, maybeAsString->length() - 6).integerValue();
    }

    return result;
}

void CollectionImplementation::Track::setBitRateInKiloBitsPerSecond(uinteger32 bitRate)
{
    auto bitrateString = bitRate ? String::stringWithFormat("%ld.0kbps", bitRate) : String{ };
    this->p_setStringForSubTagForIdentifier(bitrateString, trackBitrateTagIdentifier);

    this->p_needsToUpdateDatabaseFile = true;
}

uinteger32 CollectionImplementation::Track::sampleRateInSamplesPerSecond() const
{
    uinteger32 result = 0;

    auto maybeAsString = this->p_maybeStringForSubTagForIdentifier(trackSampleRateTagIdentifier);
    if (maybeAsString.isValid() && (maybeAsString->length() > 1)) {
        // -- Valid strings should end with "k"
        auto asDecimal = DecimalNumber(maybeAsString->subString(0, maybeAsString->length() - 1));
        asDecimal *= 1000;

        result = asDecimal.asInteger();
    }

    return result;
}

void CollectionImplementation::Track::setSampleRateInSamplesPerSecond(uinteger32 sampleRate)
{
    double sampleRateInThousandOfSamplesPerSecond = static_cast<double>(sampleRate) / 1000.0f;
    auto sampleRateString = sampleRateInThousandOfSamplesPerSecond ?
                            String::stringWithFormat("%.1fk", sampleRateInThousandOfSamplesPerSecond) :
                            String{ };
    this->p_setStringForSubTagForIdentifier(sampleRateString, trackSampleRateTagIdentifier);

    this->p_needsToUpdateDatabaseFile = true;
}

void CollectionImplementation::Track::setMarkersAndMaybeAddOffsetInSeconds(const Array<Common::MarkerOfSomeSort>& markers, Optional<DecimalNumber> maybeOffset)
{
    // -- Since we are replacing all the markers, we don't need to load the old ones from the track file.
    this->p_maybeMarkers = MutableArray<Shared<MutableMarkerOfSomeSort>>{ };

    for (auto&& marker : markers) {
        withVariant(marker, [this, &maybeOffset](auto&& marker) {
            this->p_appendCommonMarkerAndMaybeAddOffsetInSeconds(marker, maybeOffset);
        });
    }

    this->p_needsToUpdateTrackFile = true;
}

boolean CollectionImplementation::Track::needsToUpdateDatabaseFile() const
{
    return this->p_needsToUpdateDatabaseFile;
}

boolean CollectionImplementation::Track::needsToUpdateTrackFile() const
{
    return this->p_needsToUpdateTrackFile;
}

void CollectionImplementation::Track::setNeedsToUpdateTrackFile(boolean flag)
{
    this->p_needsToUpdateTrackFile = flag;
}

void CollectionImplementation::Track::updateTrackFile() const
{
    auto fullFilePath = FilePath::filePathByJoiningPaths((*this->p_collection).volume(), this->filePathRelativeToDatabaseVolume());
    auto maybeTrackFile = TrackFileFactory::maybeTrackFileForPath(fullFilePath);
    if (!maybeTrackFile.isValid()) {
        NXA_DLOG("Error opening Serato file to save.\n");
        return;
    }

    maybeTrackFile->setSeratoBeatGridIsLocked(this->beatGridIsLocked());

    if (this->p_maybeMarkers.isValid()) {
        MutableArray<SeratoMarker::OfSomeSort> newMarkers;

        for (auto&& marker : *this->p_maybeMarkers) {
            withVariant(*marker, [&newMarkers](auto& marker) {
                newMarkers.emplaceAppend(marker.asSeratoMarkerOfSomeSort());
            });
        }

        maybeTrackFile->setSeratoMarkers(newMarkers);
    }

    if (!maybeTrackFile->save()) {
        NXA_DLOG("Error saving to Serato file.\n");
    }
}

void CollectionImplementation::Track::addTo(MutableBlob& destination) const
{
    this->p_trackTag.addInSeratoTrackOrderTo(destination);
}
