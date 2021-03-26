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

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackDateAdded.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>

using namespace testing;

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

class TrackDateAddedTests : public CollectionTestsBase
{

};

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_ADateAsAString_ReturnsACorrectValue)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("2015-12-30", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "2015-12-30");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_ADateAsAStringWithJustTheYear_ReturnsACorrectValue)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("2015", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "2015");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_ADateAsAStringWithJustTheYearAndTwoDigits_ReturnsACorrectValue)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("15", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "2015");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_ADateAsAStringWithJustTheYearAndTwoDigitsNutInThe90s_ReturnsACorrectValue)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("93", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "1993");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_ADateAsAStringWithJustTheYearAs0_ReturnsACorrectValue)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("0", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "2000");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_AnInvalidDateAsAStringWithATime_ReturnsJustTheDate)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("2015-12-30 12:23:45", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "2015-12-30");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_AnInvalidDateAsAStringWithLetters_ReturnsNothing)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("fdf5sfdf-05-12", this->collection().asReference());

    // -- Then.
    EXPECT_FALSE(dateAdded.isValid());
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_AnInvalidDateWithMissingDay_ReturnsJustTheYear)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("1995-24", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "1995");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_AnInvalidDateWithInvalidDay_ReturnsJustTheYear)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("1995-12-56", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "1995");
}

TEST_F(TrackDateAddedTests, maybeWithValueInCollection_AnInvalidDateWithInvalidMonth_ReturnsJustTheYear)
{
    // -- Given.
    // -- When.
    auto dateAdded = TrackDateAdded::maybeWithValueInCollection("1996-14-6", this->collection().asReference());

    // -- Then.
    EXPECT_TRUE(dateAdded.isValid());
    EXPECT_TRUE(dateAdded->stringValue() == "1996");
}

} } }
