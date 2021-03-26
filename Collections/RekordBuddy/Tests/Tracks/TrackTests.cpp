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

#include <CommonCollection/Markers/MarkerColor.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

#include <Base/Test.hpp>

using namespace testing;

namespace NxA { namespace Rekordbox {

class RekordBuddyTrackTests : public NxA::Test
{
public:
    // -- Constructors & Destructors
    RekordBuddyTrackTests()
    {
        Common::MusicalKey::setDefaultNotation(Common::MusicalKey::Notation::Camelot);
    }
};

/* TODO: This can be used to test the marker offsets as stored in the Track itself
TEST_F(RekordBuddyTrackTests, removeContextForVolumePath_AVolumeWhichHadAValidOffset_RemovesTheContext)
{
    // -- Given.
    auto path = FilePath{ NXA_PROJECT_DIR "/Test Tracks/Serato DJ MPEG Gapless Offset.mp3", FilePath::CaseSensitivity::Regular };
    this->setTestPathOfAvailableVolumes(path);
    auto volume = this->testPathOfAvailableVolumes.firstObject();
    auto collection = this->collection();
    auto maybeRelativeFilePath = path.maybeWithPrefixRemoved(volume.asFilePath());
    EXPECT_TRUE(maybeRelativeFilePath.isValid());
    collection.trackWithRelativeFilePath(*maybeRelativeFilePath);
    auto& offsetUtility = MarkerOffset::singleton();
    offsetUtility.addContextForVolumePath(collection.persistentContext(), volume);
    offsetUtility.updateOffsetsForFileAtRelativePathOnVolume(*maybeRelativeFilePath, volume);

    // -- When.
    offsetUtility.removeContextForVolumePath(volume);

    // -- Then.
    EXPECT_THROW(offsetUtility.maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo(*maybeRelativeFilePath,
                                                                                                    volume,
                                                                                                    Common::Collection::Type::rekordbox), AssertionFailed);
}

TEST_F(RekordBuddyTrackTests, maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo_APathInAContextWhichDoesNotExists_ThrowsAnException)
{
    FilePath path(NXA_PROJECT_DIR "/Test Tracks/Serato DJ MPEG Gapless Offset.mp3", FilePath::CaseSensitivity::Regular);
    auto maybeVolume = Volume::maybeVolumeForFilePath(path);
    EXPECT_TRUE(maybeVolume.isValid());
    auto& offsetUtility = MarkerOffset::singleton();
    auto maybeRelativeFilePath = path.maybeWithPrefixRemoved(maybeVolume->asFilePath());
    EXPECT_TRUE(maybeRelativeFilePath.isValid());

    // -- When.
    // -- Then.
    EXPECT_THROW(offsetUtility.maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo(*maybeRelativeFilePath,
                                                                                                    *maybeVolume,
                                                                                                    Common::Collection::Type::rekordbox), AssertionFailed);
}

TEST_F(RekordBuddyTrackTests, maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo_APathWhichDoesNotExists_ReturnsNothing)
{
    // -- Given.
    auto path = FilePath{ NXA_PROJECT_DIR "/WrongCollections/TrackFiles/Tests/Test Tracks/Serato DJ MPEG Gapless Offset.mp3", FilePath::CaseSensitivity::None };
    this->setTestPathOfAvailableVolumes(path);
    auto volume = this->testPathOfAvailableVolumes.firstObject();
    auto collection = this->collection();
    auto maybeRelativeFilePath = path.maybeWithPrefixRemoved(volume.asFilePath());
    EXPECT_TRUE(maybeRelativeFilePath.isValid());
    collection.trackWithRelativeFilePath(*maybeRelativeFilePath);
    auto& offsetUtility = MarkerOffset::singleton();
    offsetUtility.addContextForVolumePath(collection.persistentContext(), volume);

    // -- When.
    auto maybeOffset = offsetUtility.maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo(*maybeRelativeFilePath,
                                                                                                          volume,
                                                                                                          Common::Collection::Type::rekordbox);

    // -- Then.
    EXPECT_FALSE(maybeOffset.isValid());
}

TEST_F(RekordBuddyTrackTests, maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo_APathWhichDoesExists_ReturnsCorrectOffset)
{
    // -- Given.
    auto path = FilePath{ NXA_PROJECT_DIR "/Test Tracks/Serato DJ MPEG Gapless Offset.mp3", FilePath::CaseSensitivity::Regular };
    this->setTestPathOfAvailableVolumes(path);
    auto volume = this->testPathOfAvailableVolumes.firstObject();
    auto collection = this->collection();
    auto maybeRelativeFilePath = path.maybeWithPrefixRemoved(volume.asFilePath());
    EXPECT_TRUE(maybeRelativeFilePath.isValid());
    collection.trackWithRelativeFilePath(*maybeRelativeFilePath);
    auto& offsetUtility = MarkerOffset::singleton();
    offsetUtility.addContextForVolumePath(collection.persistentContext(), volume);
    offsetUtility.updateOffsetsForFileAtRelativePathOnVolume(*maybeRelativeFilePath, volume);

    // -- When.
    auto maybeOffset = offsetUtility.maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo(*maybeRelativeFilePath,
                                                                                                          volume,
                                                                                                          Common::Collection::Type::rekordbox);

    // -- Then.
    EXPECT_TRUE(maybeOffset.isValid());
    EXPECT_STREQ("-0.026", maybeOffset->asString().asUTF8());
}

TEST_F(RekordBuddyTrackTests, maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo_APathWhichDoesExistsButNotForTheRightCollection_ReturnsNothing)
{
    // -- Given.
    auto path = FilePath{ NXA_PROJECT_DIR "/../Collections/TrackFiles/Tests/Test Tracks/Serato DJ MPEG Gapless Offset.mp3", FilePath::CaseSensitivity::Regular };
    this->setTestPathOfAvailableVolumes(path);
    auto volume = this->testPathOfAvailableVolumes.firstObject();
    auto collection = this->collection();
    auto maybeRelativeFilePath = path.maybeWithPrefixRemoved(volume.asFilePath());
    EXPECT_TRUE(maybeRelativeFilePath.isValid());
    collection.trackWithRelativeFilePath(*maybeRelativeFilePath);
    auto& offsetUtility = MarkerOffset::singleton();
    offsetUtility.addContextForVolumePath(collection.persistentContext(), volume);
    offsetUtility.updateOffsetsForFileAtRelativePathOnVolume(*maybeRelativeFilePath, volume);

    // -- When.
    auto maybeOffset = offsetUtility.maybeOffsetToAddInSecondsForFileAtRelativePathOnVolumeWhenExportedTo(*maybeRelativeFilePath,
                                                                                                          volume,
                                                                                                          Common::Collection::Type::Serato);

    // -- Then.
    EXPECT_FALSE(maybeOffset.isValid());
}
*/

} }
