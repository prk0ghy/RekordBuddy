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

#include <SeratoCollection/Markers/GridMarker.hpp>
#include <SeratoCollection/Tracks/Track.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Overridden Common::MutableGridMarker Instance Methods

void MutableGridMarker::p_setBeatsPerMinute(const DecimalNumber& beatsPerMinute)
{
    this->p_marker.p_seratoGridMarker.setBeatsPerMinute(beatsPerMinute);
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableGridMarker::setPositionInSeconds(const DecimalNumber& position)
{
    this->p_marker.p_seratoGridMarker.setPositionInSeconds(position);
    this->p_parentTrack.setNeedsToUpdateTrackFile(true);
}

void MutableGridMarker::setBeatNumber(Common::GridMarker::BeatNumber beatNumber)
{
    // -- Serato only supports grid markers on the first downbeat.
    NXA_ASSERT_TRUE(beatNumber == Common::GridMarker::BeatNumber::FirstDownBeat);
}
