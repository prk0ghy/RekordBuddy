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

#include <RekordBuddyCollection/Implementation/Tracks/Internal/TrackInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>
#include <TestRekordBuddyCollectionImplementation/TrackHelpers.hpp>

using namespace testing;

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

class TrackTests : public CollectionTestsBase
{

};

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithNoExistingTagsReplaceTagsWithoutNames_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag1, results[0]);
    EXPECT_EQ(tag2, results[1]);
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithNoExistingTagsReplaceTagsWithNames_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "MyTest:10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "MyTest:8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag1, results[0]);
    EXPECT_EQ(tag2, results[1]);
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithNoExistingTagsReplaceTagsWithAndWithoutName_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "MyTest:8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag1, results[0]);
    EXPECT_EQ(tag2, results[1]);
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithExistingTagsReplaceTags_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::No);
    auto tag3 = TrackTag::withValueInCollection({ "1A" }, collection);
    auto tag4 = TrackTag::withValueInCollection({ "12B" }, collection);

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag3, tag4 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag3, results[0]);
    EXPECT_EQ(tag4, results[1]);
    EXPECT_TRUE(tag1.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(tag2.isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithExistingTagsReplaceTagsButAskNotToDeleteOrphans_ReplacesAllTheTagsButDoesNoeDeleteOrphans)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::No);
    auto tag3 = TrackTag::withValueInCollection({ "1A" }, collection);
    auto tag4 = TrackTag::withValueInCollection({ "12B" }, collection);

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag3, tag4 }, Track::AndDeleteOrphans::No);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag3, results[0]);
    EXPECT_EQ(tag4, results[1]);
    EXPECT_FALSE(tag1.isDeletedOrNotInAnyCollection());
    EXPECT_FALSE(tag2.isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithExistingTagsWithoutNameReplaceTagsWithNames_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::No);
    auto tag3 = TrackTag::withValueInCollection({ "MyTest:1A" }, collection);
    auto tag4 = TrackTag::withValueInCollection({ "MyTest:12B" }, collection);

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag3, tag4 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag3, results[0]);
    EXPECT_EQ(tag4, results[1]);
    EXPECT_TRUE(tag1.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(tag2.isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithExistingTagsWithAndWithoutNameReplaceTagsWithAndWithoutNames_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "MyTest:10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::No);
    auto tag3 = TrackTag::withValueInCollection({ "1A" }, collection);
    auto tag4 = TrackTag::withValueInCollection({ "MyTest:12B" }, collection);

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag3, tag4 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag3, results[0]);
    EXPECT_EQ(tag4, results[1]);
    EXPECT_TRUE(tag1.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(tag2.isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTests, p_replaceExistingTagsInWith_AnObjectWithExistingTagsWithAndWithoutNameReplaceTagsWithNames_ReplacesAllTheTags)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    auto track = emptyTrackInCollection(collection);
    auto tag1 = TrackTag::withValueInCollection({ "MyTest:10A" }, collection);
    auto tag2 = TrackTag::withValueInCollection({ "8B" }, collection);
    auto persistentTrack = track->asRekordBuddyTrack().persistentObject();
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag1, tag2 }, Track::AndDeleteOrphans::No);
    auto tag3 = TrackTag::withValueInCollection({ "MyTest:1A" }, collection);
    auto tag4 = TrackTag::withValueInCollection({ "MyTest:12B" }, collection);

    // -- When.
    p_replaceExistingTagsInWith<TrackTag>(persistentTrack, { tag3, tag4 }, Track::AndDeleteOrphans::Yes);

    // -- Then.
    auto results = p_tagsOfTypeIn<TrackTag>(persistentTrack.get());
    ASSERT_EQ(2u, results.length());
    EXPECT_EQ(tag3, results[0]);
    EXPECT_EQ(tag4, results[1]);
    EXPECT_TRUE(tag1.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(tag2.isDeletedOrNotInAnyCollection());
}

} } }
