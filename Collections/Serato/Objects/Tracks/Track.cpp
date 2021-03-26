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

#include <SeratoCollection/Tracks/Track.hpp>
#include <SeratoCollection/Collection.hpp>

#include <SeratoCollectionImplementation/Track.hpp>
#include <SeratoCollectionImplementation/Crate.hpp>

using namespace NxA;
using namespace Serato;

// -- Instance Methods

NotNull<const Common::Collection*> MutableTrack::collection() const
{
    return this->p_seratoTrack.collection().asNotNull();
}

NotNull<Common::MutableCollection*> MutableTrack::collection()
{
    return this->p_seratoTrack.collection().asNotNull();
}

Volume MutableTrack::volume() const
{
    return this->p_seratoTrack.collection().asReference().seratoDatabase().volumePath();
}

Array<String> MutableTrack::artists() const
{
    return this->p_seratoTrack.artist().splitBySeparator(this->p_seratoTrack.collection().asReference().artistsSeparator());
}

void MutableTrack::setArtists(const Array<String>& values)
{
    this->p_seratoTrack.setArtist(String::stringByJoiningArrayWithString(values,
                                                                         this->p_seratoTrack.collection().asReference().artistsSeparator()));
}

Array<String> MutableTrack::producers() const
{
    return this->p_seratoTrack.composer().splitBySeparator(this->p_seratoTrack.collection().asReference().artistsSeparator());
}

void MutableTrack::setProducers(const Array<String>& values)
{
    this->p_seratoTrack.setComposer(String::stringByJoiningArrayWithString(values,
                                                                           this->p_seratoTrack.collection().asReference().artistsSeparator()));
}

Array<String> MutableTrack::remixers() const
{
    return this->p_seratoTrack.remixer().splitBySeparator(this->p_seratoTrack.collection().asReference().artistsSeparator());
}

void MutableTrack::setRemixers(const Array<String>& values)
{
    this->p_seratoTrack.setRemixer(String::stringByJoiningArrayWithString(values,
                                                                          this->p_seratoTrack.collection().asReference().artistsSeparator()));
}

void MutableTrack::markAsModifiedNow()
{
    this->collection()->markAsModifiedNow();
    this->p_seratoTrack.setTimeLastModifiedInSecondsSinceJanuary1st1970(Time::currentTime().asUnixTimeStamp());
}

AudioFileType MutableTrack::fileType() const
{
    return genericTypeForAudioFileAt(this->relativeFilePath());
}

void MutableTrack::setMarkersAndMaybeAddOffsetInSeconds(const Array<Common::MarkerOfSomeSort>& markers, Optional<DecimalNumber> maybeOffset)
{
    Common::MarkerValidation::CorrectingFlags flags;
    if ((*this->p_seratoTrack.collection()).convertMemoryCuesToHotCuesOnExport()) {
        flags.set(Common::MarkerValidation::CorrectingFlag::ConvertMemoryCuesToHotCues);
    }

#if defined(NXA_MARKER_COPY_DEBUG_INFO)
    {
        NXA_BETA_LOG_WITH_FORMAT("Before correction found %llu markers to write to Serato\n", markers.length());
        for (auto&& marker : markers) {
            withVariant(marker, [](auto& marker) {
                NXA_BETA_LOG_WITH_FORMAT("  %s\n", marker->description().asUTF8());
            });
        }
    }
#endif

    Common::MarkerValidation markerValidator;
    markerValidator.setCorrectingFlags(flags);

    auto maybeCorrectedMarkers = markerValidator.maybeCorrectedMarkersWithWhenExportedTo(markers, Common::Collection::Type::Serato);
#if defined(NXA_MARKER_COPY_DEBUG_INFO)
    {
        auto& testMarkers = maybeCorrectedMarkers.isValid() ? **maybeCorrectedMarkers : markers;
        NXA_BETA_LOG_WITH_FORMAT("After correction found %llu markers to write to Serato\n", testMarkers.length());
        for (auto&& marker : testMarkers) {
            withVariant(marker, [](auto& marker) {
                NXA_BETA_LOG_WITH_FORMAT("  %s\n", marker->description().asUTF8());
            });
        }
    }
#endif

    this->p_seratoTrack.setMarkersAndMaybeAddOffsetInSeconds(maybeCorrectedMarkers.isValid() ? **maybeCorrectedMarkers : markers, maybeOffset);
}
