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

#include <CommonCollection/Markers/LoopMarker.hpp>

#include <SeratoCollection/Markers/MarkerUtility.hpp>

#include <TrackFiles/SeratoMarkers/SeratoMarkers.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations
class MutableTrack;

namespace CollectionImplementation {

class Track;

}

// -- Public Interface
class LoopMarker final : public Common::LoopMarker
{
    // -- Friends
    friend class MutableLoopMarker;

    // -- Private Constructors & Destructors
    LoopMarker(SeratoLoopMarker&& from) : p_seratoLoopMarker{ std::move(from) } { }

    // -- Private Instance Variables
    SeratoLoopMarker p_seratoLoopMarker;

public:
    // -- Constructors & Destructors
    ~LoopMarker() override = default;

    // -- Overridden Common::LoopMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return DecimalNumber::withIntegerAndExponant(this->p_seratoLoopMarker.startPositionInMilliseconds(), -3);
    }
    DecimalNumber lengthInSeconds() const override
    {
        return DecimalNumber::withIntegerAndExponant(this->p_seratoLoopMarker.endPositionInMilliseconds() - this->p_seratoLoopMarker.startPositionInMilliseconds(), -3);
    }

    Common::Marker::Flags flags() const override
    {
        return { };
    }

    Optional<count> maybeHotCueNumber() const override
    {
        auto hotCueIndex = this->p_seratoLoopMarker.index();
        if ((hotCueIndex < 0) || (hotCueIndex > MarkerUtility::MaximumHotcueIndex)) {
            // -- Serato markers are all hotcues and all need to be within the allowed indices.
            return nothing;
        }

        return hotCueIndex;
    }

    Optional<String> maybeName() const override
    {
        auto name = this->p_seratoLoopMarker.label();
        return name.length() ? name : Optional<String>{ };
    }

    Optional<Color> maybeColor() const override
    {
        auto color = this->p_seratoLoopMarker.color();
        return color.asRGB() ? color : Optional<Color>{ };
    }
};

class MutableLoopMarker final : public Common::MutableLoopMarker
{
    // -- Friends
    friend class MutableTrack;
    friend class CollectionImplementation::Track;

#if defined(NXA_BUILD_FOR_TESTING)
    friend class SeratoLoopMarkerTests;
#endif

    // -- Private Instance Variables
    Serato::LoopMarker p_marker;
    Serato::CollectionImplementation::Track& p_parentTrack;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Overridden Common::MutableLoopMarker Instance Methods
    void p_setName(const Optional<String>&) override;
    void p_setColor(const Optional<Color>&) override;

public:
    // -- Constructors & Destructors
    MutableLoopMarker(SeratoLoopMarker&& from,
                      Serato::CollectionImplementation::Track& inTrack,
                      const Protected& p_isProtected) : p_marker{ std::move(from) },
                                                        p_parentTrack{ inTrack } { }
    ~MutableLoopMarker() override = default;

    // -- Overridden Common::LoopMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_marker.positionInSeconds();
    }
    DecimalNumber lengthInSeconds() const override
    {
        return this->p_marker.lengthInSeconds();
    }

    Common::Marker::Flags flags() const override
    {
        return this->p_marker.flags();
    }

    Optional<count> maybeHotCueNumber() const override
    {
        return this->p_marker.maybeHotCueNumber();
    }

    Optional<String> maybeName() const override
    {
        return this->p_marker.maybeName();
    }

    Optional<Color> maybeColor() const override
    {
        return this->p_marker.maybeColor();
    }

    // -- Overridden Common::MutableLoopMarker Instance Methods
    void setPositionInSeconds(const DecimalNumber&) override;
    void setLengthInSeconds(const DecimalNumber&) override;

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

    void setHotCueNumber(const Optional<count>&) override;

    // -- Instance Methods
    inline const Serato::LoopMarker& asImmutableReference() const
    {
        return this->p_marker;
    }

    inline SeratoMarker::OfSomeSort asSeratoMarkerOfSomeSort()
    {
        return this->p_marker.p_seratoLoopMarker;
    }
};

} }
