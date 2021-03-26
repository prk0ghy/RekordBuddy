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

#include <CommonCollection/Markers/CueMarker.hpp>

#include <RekordBuddyCollection/Markers/MarkerUtility.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Pointers.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class CueMarker final : public Common::CueMarker
{
    // -- Friends
    friend class MutableTrack;
    friend class MutableCueMarker;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Variables
    Shared<CollectionImplementation::PersistentCueMarker> p_rekordBuddyMarker;

public:
    // -- Constructors & Destructors
    CueMarker(Shared<CollectionImplementation::PersistentCueMarker>&& from, const Protected&) : p_rekordBuddyMarker{ std::move(from) } { }
    ~CueMarker() override = default;

    // -- Overridden Common::CueMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_rekordBuddyMarker->positionInSeconds();
    }

    Common::Marker::Flags flags() const override
    {
        return Common::Marker::Flags{ static_cast<uinteger64>(this->p_rekordBuddyMarker->flags()) };
    }

    Optional<count> maybeHotCueNumber() const override
    {
        return { this->p_rekordBuddyMarker->maybeHotCueNumber().maybe([](auto&& hotCueNumber) -> count {
            return static_cast<uinteger>(hotCueNumber);
        }) };
    }

    Optional<String> maybeName() const override
    {
        return this->p_rekordBuddyMarker->maybeName();
    }

    Optional<Color> maybeColor() const override
    {
        auto maybeRgbColor = this->p_rekordBuddyMarker->maybeRgbColor();
        if (!maybeRgbColor.isValid() || !*maybeRgbColor) {
            return nothing;
        }

        return Color{ static_cast<uinteger32>((*maybeRgbColor << 8) | 0xff) };
    }
};

class MutableCueMarker final : public Common::MutableCueMarker
{
    // -- Friends
    friend class MutableTrack;

    // -- Private Instance Variables
    RekordBuddy::CueMarker p_marker;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Overridden Common::MutableCueMarker Instance Methods
    void p_setName(const Optional<String>& maybeName) override
    {
        this->p_marker.p_rekordBuddyMarker->setMaybeName(maybeName);
    }

    void p_setColor(const Optional<Color>& maybeColor) override
    {
        this->p_marker.p_rekordBuddyMarker->setMaybeRgbColor({ maybeColor.maybe([](auto&& color) -> integer32 {
            return color.asRGB();
        }) });
    }

public:
    // -- Constructors & Destructors
    MutableCueMarker(Shared<CollectionImplementation::PersistentCueMarker>&& from,
                     const Protected&) : p_marker{ std::move(from),
                                                   RekordBuddy::CueMarker::p_isProtected } { }
    MutableCueMarker(CollectionImplementation::Track& inTrack,
                     const Protected&) :
            p_marker{ inTrack.asPersistent()->getContext()->createObject<CollectionImplementation::PersistentCueMarker>(),
                      RekordBuddy::CueMarker::p_isProtected }
    {
        inTrack.asPersistent()->addMarkersItem(this->p_marker.p_rekordBuddyMarker.asStdSharedPointer());
    }
    ~MutableCueMarker() override = default;

    // -- Overridden Common::CueMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_marker.positionInSeconds();
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

    // -- Overridden Common::MutableCueMarker Instance Methods
    void setPositionInSeconds(const DecimalNumber& position) override
    {
        this->p_marker.p_rekordBuddyMarker->setPositionInSeconds(position);
    }

    void setFlag(Common::Marker::Flag flag) override
    {
        this->setFlags(this->flags().andAlso(flag));
    }
    void setFlags(const Common::Marker::Flags& flags) override
    {
        this->p_marker.p_rekordBuddyMarker->setFlags(flags.asPackedValue());
    }
    void clearFlag(Common::Marker::Flag flag) override
    {
        this->setFlags(this->flags().without(flag));
    }

    void setHotCueNumber(const Optional<count>& maybeHotCueNumber) override
    {
        this->p_marker.p_rekordBuddyMarker->setMaybeHotCueNumber({ maybeHotCueNumber.maybe([](auto&& hotCueNumber) -> integer16 {
            return hotCueNumber;
        }) });
    }

    // -- Instance Methods
    void deleteFromCollection()
    {
        MarkerUtility::p_deleteFromCollection(*this->p_marker.p_rekordBuddyMarker);
    }

    inline const RekordBuddy::CueMarker& asImmutableReference() const
    {
        return this->p_marker;
    }
};

} }
