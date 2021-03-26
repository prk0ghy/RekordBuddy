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

#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTitle.hpp>

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

using namespace testing;

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

class CollectionTests : public CollectionTestsBase
{

};

TEST_F(CollectionTests, tracksMatchingPredicate_APredicateWithTheEqualOperatorForAStringProperty_ReturnsTheCorrectTracks)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track1 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix).m4a"));
    track1->setTitle({ "My Tst 2" });
    track1->markAsModifiedNow();
    auto track2 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix)2.m4a"));
    track2->setTitle({ "My Test 2" });
    track2->markAsModifiedNow();
    auto predicate = Common::TrackPredicate::comparisonWith(Common::Property::TypeID::Title, Common::TrackPredicate::ComparisonOperator::Equal, "My Tst 2");

    // -- When.
    auto results = collection.tracksMatchingPredicate(predicate);

    // -- Then.
    ASSERT_EQ(1u, results.length());
    EXPECT_TRUE(results.contains(track1->reference()));
}

TEST_F(CollectionTests, tracksMatchingPredicate_APredicateWithTheNotEqualOperatorForAStringProperty_ReturnsTheCorrectTracks)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track1 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix).m4a"));
    track1->setTitle({ "My Tst 2" });
    track1->markAsModifiedNow();
    auto track2 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix)2.m4a"));
    track2->setTitle({ "My Test 2" });
    track2->markAsModifiedNow();
    auto predicate = Common::TrackPredicate::comparisonWith(Common::Property::TypeID::Title, Common::TrackPredicate::ComparisonOperator::NotEqual, "My Tst 2");

    // -- When.
    auto results = collection.tracksMatchingPredicate(predicate);

    // -- Then.
    ASSERT_EQ(1u, results.length());
    EXPECT_TRUE(results.contains(track2->reference()));
}

TEST_F(CollectionTests, tracksMatchingPredicate_APredicateWithTheContainsOperatorForAStringProperty_ReturnsTheCorrectTracks)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track1 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix).m4a"));
    track1->setTitle({ "My Tst 2" });
    track1->markAsModifiedNow();
    auto track2 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix)2.m4a"));
    track2->setTitle({ "My Test 2" });
    track2->markAsModifiedNow();
    auto predicate = Common::TrackPredicate::comparisonWith(Common::Property::TypeID::Title, Common::TrackPredicate::ComparisonOperator::Contains, "Test");

    // -- When.
    auto results = collection.tracksMatchingPredicate(predicate);

    // -- Then.
    ASSERT_EQ(1u, results.length());
    EXPECT_TRUE(results.contains(track2->reference()));
}

TEST_F(CollectionTests, tracksMatchingPredicate_APredicateWithTheDoesNotContainsOperatorForAStringProperty_ReturnsTheCorrectTracks)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track1 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix).m4a"));
    track1->setTitle({ "My Tst 2" });
    track1->markAsModifiedNow();
    auto track2 = collection.trackWithRelativeFilePath(NXA_FILEPATH("Users/didier/Music/Gigs/25 I'm In Love (feat. Donae'O) (Deep Mix)2.m4a"));
    track2->setTitle({ "My Test 2" });
    track2->markAsModifiedNow();
    auto predicate = Common::TrackPredicate::comparisonWith(Common::Property::TypeID::Title, Common::TrackPredicate::ComparisonOperator::DoesNotContains, "Test");

    // -- When.
    auto results = collection.tracksMatchingPredicate(predicate);

    // -- Then.
    ASSERT_EQ(1u, results.length());
    EXPECT_TRUE(results.contains(track1->reference()));
}

/*
 Equal,
 NotEqual,
 LessThan,
 LessThanOrEqual,
 GreaterThan,
 GreaterThanOrEqual,
 StartsWith,
 EndsWith,
 Contains,
 DoesNotContain,
 */

} } }
