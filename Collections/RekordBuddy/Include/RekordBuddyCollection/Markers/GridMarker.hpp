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

#include <RekordBuddyCollection/Markers/MarkerUtility.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Pointers.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class GridMarker final : public Common::GridMarker
{
    // -- Friends
    friend class MutableTrack;
    friend class MutableGridMarker;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Variables
    Shared<CollectionImplementation::PersistentGridMarker> p_rekordBuddyMarker;

public:
    // -- Constructors & Destructors
    GridMarker(Shared<CollectionImplementation::PersistentGridMarker>&& from, const Protected&) : p_rekordBuddyMarker{ std::move(from) } { }
    ~GridMarker() override = default;

    // -- Overridden Common::GridMarker Instance Methods
    DecimalNumber positionInSeconds() const override
    {
        return this->p_rekordBuddyMarker->positionInSeconds();
    }

    Common::Marker::Flags flags() const override
    {
        return Common::Marker::Flags{ static_cast<uinteger64>(this->p_rekordBuddyMarker->flags()) };
    }

    Common::GridMarker::BeatNumber beatNumber() const override
    {
        static Common::GridMarker::BeatNumber translationTable[] = {
            Common::GridMarker::BeatNumber::FirstDownBeat,
            Common::GridMarker::BeatNumber::SecondDownBeat,
            Common::GridMarker::BeatNumber::ThirdDownBeat,
            Common::GridMarker::BeatNumber::FourthDownBeat
        };

        auto persistentBeatNumber = this->p_rekordBuddyMarker->beatNumber();
        NXA_ASSERT_TRUE(persistentBeatNumber < 4);

        return translationTable[persistentBeatNumber];
    }

    DecimalNumber beatsPerMinute() const override
    {
        return this->p_rekordBuddyMarker->beatsPerMinute();
    }
};

class MutableGridMarker final : public Common::MutableGridMarker
{
    // -- Friends
    friend class MutableTrack;

    // -- Private Instance Variables
    RekordBuddy::GridMarker p_marker;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Overridden Common::MutableGridMarker Instance Methods
    void p_setBeatsPerMinute(const DecimalNumber& bpm) override
    {
        this->p_marker.p_rekordBuddyMarker->setBeatsPerMinute(bpm);
    }

public:
    // -- Constructors & Destructors
    MutableGridMarker(Shared<CollectionImplementation::PersistentGridMarker>&& from,
                      const Protected&) : p_marker{ std::move(from), RekordBuddy::GridMarker::p_isProtected } { }
    MutableGridMarker(CollectionImplementation::Track& inTrack,
                      const Protected& p_isProtected) :
            p_marker{ inTrack.asPersistent()->getContext()->createObject<CollectionImplementation::PersistentGridMarker>(),
                      RekordBuddy::GridMarker::p_isProtected }
    {
        inTrack.asPersistent()->addMarkersItem(this->p_marker.p_rekordBuddyMarker.asStdSharedPointer());
    }
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

    void setBeatNumber(Common::GridMarker::BeatNumber number) override
    {
        integer16 translatedNumber;
        switch (number) {
            case Common::GridMarker::BeatNumber::FirstDownBeat: {
                translatedNumber = 0;
                break;
            }
            case Common::GridMarker::BeatNumber::SecondDownBeat: {
                translatedNumber = 1;
                break;
            }
            case Common::GridMarker::BeatNumber::ThirdDownBeat: {
                translatedNumber = 2;
                break;
            }
            case Common::GridMarker::BeatNumber::FourthDownBeat: {
                translatedNumber = 3;
                break;
            }
        };

        this->p_marker.p_rekordBuddyMarker->setBeatNumber(translatedNumber);
    }

    // -- Instance Methods
    void deleteFromCollection()
    {
        MarkerUtility::p_deleteFromCollection(*this->p_marker.p_rekordBuddyMarker);
    }

    inline const RekordBuddy::GridMarker& asImmutableReference() const
    {
        return this->p_marker;
    }
};

} }
