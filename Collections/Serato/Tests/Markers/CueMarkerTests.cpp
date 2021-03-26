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
#include <SeratoCollection/Markers/CueMarker.hpp>

#include <TrackFiles/SeratoMarkers/SeratoCueMarker.hpp>

#include <CommonCollection/Markers/MarkerColor.hpp>

#include "../Tracks/TrackTests.hpp"

#include <SeratoCollectionImplementation/Database.hpp>

#include <Base/Test.hpp>
#include <Base/Volume.hpp>

using namespace testing;

namespace NxA { namespace Serato {

class SeratoCueMarkerTests : public NxA::Test
{
    // -- Private Instance Variables
    Optional<Unique<MutableCollection>> p_maybeCollection;

    Optional<CollectionImplementation::Track> p_maybeTrack;

public:
    // -- Constructors & Destructors
    SeratoCueMarkerTests()
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
    MutableCueMarker testMarkerWithHotCueNumber(count hotCueNumber)
    {
        if (!this->p_maybeTrack.isValid()) {
            this->p_maybeTrack = CollectionImplementation::TrackTests::testTrackInCollection(&this->testCollection());
        }

        auto maybeSeratoMarker = SeratoCueMarker::maybeMarkerWithLabelPositionIndexAndColor(String("TEST"), 34985, 0, Color{ 0 });
        NXA_ASSERT_TRUE(maybeSeratoMarker.isValid());

        auto newMarker = MutableCueMarker{ std::move(*maybeSeratoMarker), *this->p_maybeTrack, MutableCueMarker::p_isProtected };
        newMarker.setHotCueNumber(hotCueNumber);

        return newMarker;
    }
};

TEST_F(SeratoCueMarkerTests, setColor_AValidColor_SetsTheColor)
{
    // -- Given.
    auto marker = this->testMarkerWithHotCueNumber(0);

    // -- When.
    marker.setColor(Common::MarkerColor::TurquoiseColor);

    // -- Then.
    auto maybeColor = marker.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(*maybeColor, Common::MarkerColor::TurquoiseColor);
}

TEST_F(SeratoCueMarkerTests, setColor_AnInvalidColorAndDifferentHotCueNumbers_ReturnsACorrectDefaultColorForEachHotCue)
{
    // -- Given.
    auto marker0 = this->testMarkerWithHotCueNumber(0);
    auto marker1 = this->testMarkerWithHotCueNumber(1);
    auto marker2 = this->testMarkerWithHotCueNumber(2);
    auto marker3 = this->testMarkerWithHotCueNumber(3);
    auto marker4 = this->testMarkerWithHotCueNumber(4);
    auto marker5 = this->testMarkerWithHotCueNumber(5);
    auto marker6 = this->testMarkerWithHotCueNumber(6);
    auto marker7 = this->testMarkerWithHotCueNumber(7);

    // -- When.
    marker0.setColor(Color{ 0x11, 0x22, 0x33 });
    marker1.setColor(Color{ 0x11, 0x22, 0x33 });
    marker2.setColor(Color{ 0x11, 0x22, 0x33 });
    marker3.setColor(Color{ 0x11, 0x22, 0x33 });
    marker4.setColor(Color{ 0x11, 0x22, 0x33 });
    marker5.setColor(Color{ 0x11, 0x22, 0x33 });
    marker6.setColor(Color{ 0x11, 0x22, 0x33 });
    marker7.setColor(Color{ 0x11, 0x22, 0x33 });

    // -- Then.
    auto maybeColor = marker0.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[0], *maybeColor);
    maybeColor = marker1.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[1], *maybeColor);
    maybeColor = marker2.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[2], *maybeColor);
    maybeColor = marker3.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[3], *maybeColor);
    maybeColor = marker4.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[4], *maybeColor);
    maybeColor = marker5.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[5], *maybeColor);
    maybeColor = marker6.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[6], *maybeColor);
    maybeColor = marker7.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[7], *maybeColor);
}

TEST_F(SeratoCueMarkerTests, setColor_NoColorAndDifferentHotCueNumbers_ReturnsACorrectDefaultColorForEachHotCue)
{
    // -- Given.
    auto marker0 = this->testMarkerWithHotCueNumber(0);
    auto marker1 = this->testMarkerWithHotCueNumber(1);
    auto marker2 = this->testMarkerWithHotCueNumber(2);
    auto marker3 = this->testMarkerWithHotCueNumber(3);
    auto marker4 = this->testMarkerWithHotCueNumber(4);
    auto marker5 = this->testMarkerWithHotCueNumber(5);
    auto marker6 = this->testMarkerWithHotCueNumber(6);
    auto marker7 = this->testMarkerWithHotCueNumber(7);

    // -- When.
    marker0.setColor(nothing);
    marker1.setColor(nothing);
    marker2.setColor(nothing);
    marker3.setColor(nothing);
    marker4.setColor(nothing);
    marker5.setColor(nothing);
    marker6.setColor(nothing);
    marker7.setColor(nothing);

    // -- Then.
    auto maybeColor = marker0.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[0], *maybeColor);
    maybeColor = marker1.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[1], *maybeColor);
    maybeColor = marker2.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[2], *maybeColor);
    maybeColor = marker3.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[3], *maybeColor);
    maybeColor = marker4.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[4], *maybeColor);
    maybeColor = marker5.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[5], *maybeColor);
    maybeColor = marker6.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[6], *maybeColor);
    maybeColor = marker7.maybeColor();
    EXPECT_TRUE(maybeColor.isValid());
    EXPECT_EQ(Common::p_seratoDefaultMarkerColors[7], *maybeColor);
}

} }
