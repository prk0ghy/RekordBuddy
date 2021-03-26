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
#include <SeratoCollection/Markers/LoopMarker.hpp>

#include <TrackFiles/SeratoMarkers/SeratoLoopMarker.hpp>

#include "../Tracks/TrackTests.hpp"

#include <SeratoCollectionImplementation/Database.hpp>

#include <CommonCollection/Markers/MarkerColor.hpp>

#include <Base/Test.hpp>
#include <Base/Volume.hpp>

using namespace testing;

namespace NxA { namespace Serato {

class SeratoLoopMarkerTests : public NxA::Test
{
    // -- Private Instance Variables
    Optional<Unique<MutableCollection>> p_maybeCollection;

    Optional<CollectionImplementation::Track> p_maybeTrack;

public:
    // -- Constructors & Destructors
    SeratoLoopMarkerTests()
    {
#if defined(NXA_PLATFORM_MACOS)
        this->setTestUserMusicFolder(Directory{ FilePath{ "/Users/Damien/Music" } });
#elif defined(NXA_PLATFORM_WINDOWS)
        this->setTestUserMusicFolder(Directory{ FilePath{ "C:/Users/Damien/Music" } });
#else
        #error Unsupported platform.
#endif
    }

    // -- Instance Methods
    MutableCollection& testCollection()
    {
        if (!this->p_maybeCollection.isValid()) {
            this->p_maybeCollection = Unique<MutableCollection>::with(CollectionImplementation::Database::databaseOnVolume(Volume::musicFolderVolume()));
        }

        return *this->p_maybeCollection->asRawPointer();
    }
    MutableLoopMarker testMarkerWithHotCueNumber(count hotCueNumber)
    {
        if (!this->p_maybeTrack.isValid()) {
            this->p_maybeTrack = CollectionImplementation::TrackTests::testTrackInCollection(&this->testCollection());
        }

        auto maybeSeratoMarker = SeratoLoopMarker::maybeMarkerWithLabelStartEndPositionsIndexAndColor(String("TEST"), 34985, 44985,0, Color{ 0 });
        NXA_ASSERT_TRUE(maybeSeratoMarker.isValid());

        auto newMarker = MutableLoopMarker{ std::move(*maybeSeratoMarker), *this->p_maybeTrack, MutableLoopMarker::p_isProtected };
        newMarker.setHotCueNumber(hotCueNumber);

        return newMarker;
    }
};

TEST_F(SeratoLoopMarkerTests, setColor_AValidColor_SetsTheColorToNothing)
{
    // -- Given.
    auto marker = this->testMarkerWithHotCueNumber(0);

    // -- When.
    marker.setColor(Common::MarkerColor::TurquoiseColor);

    // -- Then.
    auto maybeColor = marker.maybeColor();
    EXPECT_FALSE(maybeColor.isValid());
}

} }
