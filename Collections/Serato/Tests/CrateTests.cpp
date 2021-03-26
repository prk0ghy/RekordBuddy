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

#include <SeratoCollectionImplementation/Crate.hpp>

#include <SeratoCollection/Collection.hpp>

#include <Base/Test.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class CrateTests : public NxA::Test
{
public:
    // -- CLass Methods
    static Serato::MutableTrack p_testTrackWithRelativeFilePathInCollection(const FilePath& path,
                                                                            Pointer<Serato::MutableCollection> inCollection)
    {
        return { Track{ path, inCollection, Track::p_isProtected }, Serato::MutableTrack::p_isProtected };
    }

    // -- This is obviously a huge hack and will have to be revisited as soon as some test require a real database.
    Optional<Unique<Serato::MutableCollection>> collection;

    // -- Constructors & Destructors
#if defined(NXA_PLATFORM_MACOS)
    CrateTests()
    {
        this->setTestUserMusicFolder(Directory{ FilePath{ "/Users/Damien/Music" } });

        this->collection = Unique<Serato::MutableCollection>::with(Serato::CollectionImplementation::Database::databaseOnVolume(Volume{ FilePath{ "/My/Test/" } }));
    }
#elif defined(NXA_PLATFORM_WINDOWS)
    CrateTests()
    {
        this->setTestUserMusicFolder(Directory{ FilePath{ "C:/Users/Damien/Music" } });

        this->collection = Unique<Serato::MutableCollection>::with(Serato::CollectionImplementation::Database::databaseOnVolume(Volume{ FilePath{ "C:/My/Test/" } }));
    }
#else
    #error Unsupported platform.
#endif

    // -- Instance Methods
    Crate crateWithName(const String& name)
    {
        return Crate{ name, Crate::p_isProtected };
    }
    void addCrateTo(Crate& source, Crate& destination)
    {
        source.p_parentCrate = NotNull<Crate*>{ &destination };
        destination.p_markCrateAsModified();
    }
};

// -- TDOO: Add test for addFullCrateNameWithPrefixAndRecurseToChildren()

TEST_F(CrateTests, filenameIsAValidCrateName_AValidFilename_ReturnsTrue)
{
    // -- Given.
    auto crateFilename = FilePath("Cratename.crate");

    // -- When.
    auto result = Crate::filenameIsAValidCratePath(crateFilename);

    // -- Then.
    EXPECT_TRUE(result);
}

TEST_F(CrateTests, filenameIsAValidCrateName_AValidFilenameWithIncorrectCase_ReturnsTrue)
{
    // -- Given.
    auto crateFilename = FilePath("Cratename.Crate", FilePath::CaseSensitivity::None);

    // -- When.
    auto result = Crate::filenameIsAValidCratePath(crateFilename);

    // -- Then.
    EXPECT_TRUE(result);
}

TEST_F(CrateTests, filenameIsAValidCrateName_AFilenameWithWrongExtension_ReturnsFalse)
{
    // -- Given.
    auto crateFilename = FilePath("Cratename.scrate");

    // -- When.
    auto result = Crate::filenameIsAValidCratePath(crateFilename);

    // -- Then.
    EXPECT_FALSE(result);
}

TEST_F(CrateTests, filenameIsAValidCrateName_AFilenameWithWrongExtensionWithIncorrectCase_ReturnsFalse)
{
    // -- Given.
    auto crateFilename = FilePath("Cratename.SCrate");

    // -- When.
    auto result = Crate::filenameIsAValidCratePath(crateFilename);

    // -- Then.
    EXPECT_FALSE(result);
}

TEST_F(CrateTests, filenameIsAValidCrateName_AFilenameWithAHiddenFileCharacter_ReturnsFalse)
{
    // -- Given.
    auto crateFilename = FilePath(".Cratename.crate");

    // -- When.
    auto result = Crate::filenameIsAValidCratePath(crateFilename);

    // -- Then.
    EXPECT_FALSE(result);
}

TEST_F(CrateTests, AddTrackAtIndex_ACrateAndTwoTracks_AddsTrackCorrectly)
{
    // -- Given.
    auto crate = this->crateWithName(String("MyCrate1"));
    auto track1 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile.mp4" }, this->collection->asRawPointer());
    auto track2 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile2.mp4" }, this->collection->asRawPointer());

    // -- When.
    auto index1 = crate.addTrackAtIndex(&track1, crate.numberOfTracks());
    auto index2 = crate.addTrackAtIndex(&track2, crate.numberOfTracks());

    // -- Then.
    EXPECT_EQ(0u, index1);
    EXPECT_EQ(1u, index2);
    EXPECT_EQ(0u, crate.numberOfCrates());
    EXPECT_EQ(2u, crate.numberOfTracks());
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track1 }, crate.trackAtIndex(0));
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track2 }, crate.trackAtIndex(1));
}

TEST_F(CrateTests, AddTrackAtIndex_AddingATrackInBetweenTwo_AddsTrackCorrectly)
{
    // -- Given.
    auto crate = this->crateWithName(String("MyCrate1"));
    auto track1 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile.mp4" }, this->collection->asRawPointer());
    auto track2 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile2.mp4" }, this->collection->asRawPointer());
    auto track3 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile3.mp4" }, this->collection->asRawPointer());

    // -- When.
    auto index1 = crate.addTrackAtIndex(&track1, crate.numberOfTracks());
    auto index2 = crate.addTrackAtIndex(&track2, crate.numberOfTracks());
    auto index3 = crate.addTrackAtIndex(&track3, 1);

    // -- Then.
    EXPECT_EQ(0u, index1);
    EXPECT_EQ(1u, index2);
    EXPECT_EQ(1u, index3);
    EXPECT_EQ(0u, crate.numberOfCrates());
    EXPECT_EQ(3u, crate.numberOfTracks());
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track1 }, crate.trackAtIndex(0));
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track2 }, crate.trackAtIndex(2));
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track3 }, crate.trackAtIndex(1));
}

TEST_F(CrateTests, RemoveTrackAtIndex_ACrateAndAThreeTracks_RemovesTrackCorrectly)
{
    // -- Given.
    auto crate = this->crateWithName(String("MyCrate1"));
    auto track1 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile.mp4" }, this->collection->asRawPointer());
    auto track2 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile2.mp4" }, this->collection->asRawPointer());
    auto track3 = CrateTests::p_testTrackWithRelativeFilePathInCollection(FilePath{ "Test/MyFile3.mp4" }, this->collection->asRawPointer());
    crate.addTrackAtIndex(&track1, crate.numberOfTracks());
    crate.addTrackAtIndex(&track2, crate.numberOfTracks());
    crate.addTrackAtIndex(&track3, crate.numberOfTracks());

    // -- When.
    crate.removeTrackAtIndex(1);

    // -- Then.
    EXPECT_EQ(0u, crate.numberOfCrates());
    EXPECT_EQ(2u, crate.numberOfTracks());
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track1 }, crate.trackAtIndex(0));
    EXPECT_EQ(NotNull<Serato::MutableTrack*>{ &track3 }, crate.trackAtIndex(1));
}

TEST_F(CrateTests, HasParentCrate_ACrateWithAParent_ReturnsTrue)
{
    // -- Given.
    auto crate1 = this->crateWithName(String("MyCrate1"));
    auto crate2 = this->crateWithName(String("MyCrate2"));
    this->addCrateTo(crate2, crate1);

    // -- When.
    // -- Then.
    EXPECT_TRUE(crate2.hasParentCrate());
}

TEST_F(CrateTests, HasParentCrate_ACrateWithNoParent_ReturnsFalse)
{
    // -- Given.
    auto crate1 = this->crateWithName(String("MyCrate1"));
    auto crate2 = this->crateWithName(String("MyCrate2"));

    // -- When.
    // -- Then.
    EXPECT_FALSE(crate2.hasParentCrate());
}

TEST_F(CrateTests, HasParentCrate_ACrateWithNoParentBecauseItWasRemoved_ReturnsFalse)
{
    // -- Given.
    auto crate2 = this->crateWithName(String("MyCrate2"));

    // -- When.
    // -- Then.
    EXPECT_FALSE(crate2.hasParentCrate());
}

TEST_F(CrateTests, ParentCrate_ACrateWithAParent_ReturnsTheCorrectCrate)
{
    // -- Given.
    auto crate1 = this->crateWithName(String("MyCrate1"));
    auto crate2 = this->crateWithName(String("MyCrate2"));
    this->addCrateTo(crate2, crate1);

    // -- When.
    // -- Then.
    EXPECT_EQ(crate2.parentCrate().asRawPointer(), &crate1);
}

TEST_F(CrateTests, ParentCrate_ACrateWithoutAParent_ThrowsException)
{
    // -- Given.
    auto crate2 = this->crateWithName(String("MyCrate2"));

    // -- When.
    // -- Then.
    EXPECT_THROW(crate2.parentCrate(), NxA::AssertionFailed);
}

// -- TODO: Add test for crateFilePath()
// -- TODO: Add test for childrenCratesWereModified()

} } }
