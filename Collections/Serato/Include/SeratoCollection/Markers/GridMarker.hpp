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

#include <CommonCollection/Markers/GridMarker.hpp>

#include <SeratoCollection/Markers/MarkerUtility.hpp>

#include <TrackFiles/SeratoMarkers/SeratoMarkers.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations
class MutableTrack;

namespace CollectionImplementation {

class Track;

}

// -- Public Interface
class GridMarker final : public Common::GridMarker
{
    // -- Friends
    friend class MutableGridMarker;

    // -- Private Constructors & Destructors
    GridMarker(SeratoGridMarker&& from) : p_seratoGridMarker{ std::move(from) } { }

    // -- Private Instance Variables
    SeratoGridMarker p_seratoGridMarker;

public:
    // -- Constructors & Destructors
    ~GridMarker() override = default;

    // -- Overridden Common::GridMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_seratoGridMarker.positionInSeconds();
    }

    Common::Marker::Flags flags() const override
    {
        return { };
    }

    Common::GridMarker::BeatNumber beatNumber() const override
    {
        // -- Serato only supports grid markers on the first downbeat.
        return Common::GridMarker::BeatNumber::FirstDownBeat;
    }

    DecimalNumber beatsPerMinute() const override
    {
        return this->p_seratoGridMarker.beatsPerMinute();
    }
};

class MutableGridMarker final : public Common::MutableGridMarker
{
    // -- Friends
    friend class MutableTrack;
    friend class CollectionImplementation::Track;

    // -- Private Instance Variables
    Serato::GridMarker p_marker;
    Serato::CollectionImplementation::Track& p_parentTrack;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Overridden Common::MutableGridMarker Instance Methods
    void p_setBeatsPerMinute(const DecimalNumber&) override;

public:
    // -- Constructors & Destructors
    MutableGridMarker(SeratoGridMarker&& from,
                      Serato::CollectionImplementation::Track& inTrack,
                      const Protected& p_isProtected) : p_marker{ std::move(from) },
                                                        p_parentTrack{ inTrack } { }
    ~MutableGridMarker() override = default;

    // -- Overridden Common::GridMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_marker.positionInSeconds();
    }

    Common::Marker::Flags flags() const override
    {
        return this->p_marker.flags();
    }

    Common::GridMarker::BeatNumber beatNumber() const override
    {
        return this->p_marker.beatNumber();
    }

    DecimalNumber beatsPerMinute() const override
    {
        return this->p_marker.beatsPerMinute();
    }

    // -- Overridden Common::MutableGridMarker Instance Methods
    void setPositionInSeconds(const DecimalNumber&) override;

    void setFlag(Common::Marker::Flag) override
    {
        // -- There are currently no custom flags supported by Serato.
    }
    void setFlags(const Common::Marker::Flags&) override
    {
        // -- There are currently no custom flags supported by Serato.
    }
    void clearFlag(Common::Marker::Flag) override
    {
        // -- There are currently no custom flags supported by Serato.
    }

    void setBeatNumber(Common::GridMarker::BeatNumber) override;

    // -- Instance Methods
    inline const Serato::GridMarker& asImmutableReference() const
    {
        return this->p_marker;
    }

    inline SeratoMarker::OfSomeSort asSeratoMarkerOfSomeSort()
    {
        return this->p_marker.p_seratoGridMarker;
    }
};

} }
