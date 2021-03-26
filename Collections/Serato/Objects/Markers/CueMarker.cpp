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

#include <SeratoCollection/Markers/CueMarker.hpp>
#include <SeratoCollection/Tracks/Track.hpp>

#include <CommonCollection/Markers/MarkerColor.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Overridden Common::MutableCueMarker Instance Methods

void MutableCueMarker::p_setName(const Optional<String>& maybeName)
{
    this->p_marker.p_seratoCueMarker.setLabel(maybeName.isValid() ? *maybeName : String{ });
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableCueMarker::p_setColor(const Optional<Color>& maybeColor)
{
    // -- If we don't have a color as input, we set it to an invalid color in order to get a default color based on the hot cue index.
    auto color = maybeColor.isValid() ? *maybeColor : Color{ 0x11223344u };
    this->p_marker.p_seratoCueMarker.setColor(Common::MarkerColor::maybeColorForMarkerColorAndMaybeHotCueNumberWhenExportedTo(color,
                                                                                                                               this->maybeHotCueNumber(),
                                                                                                                               Common::Collection::Type::Serato).valueOr(Color{ 0u }));
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableCueMarker::setPositionInSeconds(const DecimalNumber& position)
{
    this->p_marker.p_seratoCueMarker.setPositionInMilliseconds((position * 1000).asInteger());
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableCueMarker::setHotCueNumber(const Optional<count>& maybeHotCueNumber)
{
    // -- Serato markers are all hotcues.
    NXA_ASSERT_TRUE(maybeHotCueNumber.isValid());

    auto& hotCueIndex = *maybeHotCueNumber;
    NXA_ASSERT_TRUE((hotCueIndex >= 0) && (hotCueIndex <= MarkerUtility::MaximumHotcueIndex));

    this->p_marker.p_seratoCueMarker.setIndex(hotCueIndex);
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}
