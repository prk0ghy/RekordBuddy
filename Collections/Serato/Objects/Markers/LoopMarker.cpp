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

#include <SeratoCollection/Markers/LoopMarker.hpp>
#include <SeratoCollection/Tracks/Track.hpp>

#include <CommonCollection/Markers/MarkerColor.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Overridden Common::MutableLoopMarker Instance Methods

void MutableLoopMarker::p_setName(const Optional<String>& maybeName)
{
    this->p_marker.p_seratoLoopMarker.setLabel(maybeName.isValid() ? *maybeName : String{ });
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableLoopMarker::p_setColor(const Optional<Color>& maybeColor)
{
    // -- For the time being as far as I know, Serato loop markers do not have any colors even though the format supports it.
}

void MutableLoopMarker::setPositionInSeconds(const DecimalNumber& position)
{
    this->p_marker.p_seratoLoopMarker.setStartPositionInMilliseconds((position * 1000).asInteger());
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableLoopMarker::setLengthInSeconds(const DecimalNumber& length)
{
    this->p_marker.p_seratoLoopMarker.setEndPositionInMilliseconds(this->p_marker.p_seratoLoopMarker.startPositionInMilliseconds() + (length * 1000).asInteger());
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableLoopMarker::setHotCueNumber(const Optional<count>& maybeHotCueNumber)
{
    // -- Serato markers are all hotcues.
    NXA_ASSERT_TRUE(maybeHotCueNumber.isValid());

    auto& hotCueIndex = *maybeHotCueNumber;
    NXA_ASSERT_TRUE((hotCueIndex >= 0) && (hotCueIndex <= MarkerUtility::MaximumHotcueIndex));

    this->p_marker.p_seratoLoopMarker.setIndex(hotCueIndex);
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}
