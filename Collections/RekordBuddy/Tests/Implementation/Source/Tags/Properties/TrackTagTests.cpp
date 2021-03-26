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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTag.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>

using namespace testing;

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

class TrackTagTests : public CollectionTestsBase
{

};

TEST_F(TrackTagTests, withValueInCollection_ATrackTagWithChildrenTgags_TheTrackTagChildIsInTheTrackTagAndItsParentIsTheTrackTag)
{
    // -- Given.
    // -- When.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());

    // -- Then.
    auto maybeParentTag = childrenTag.maybeParentTag();
    EXPECT_TRUE(maybeParentTag.isValid());
    EXPECT_FALSE(maybeParentTag->maybeParentTag().isValid());
    auto childrenTags = maybeParentTag->childrenTags();
    EXPECT_EQ(1u, childrenTags.length());
    EXPECT_TRUE(childrenTags[0] == childrenTag);
    EXPECT_EQ(0u, childrenTag.childrenTags().length());
    EXPECT_FALSE(childrenTag.isDeletedOrNotInAnyCollection());
    EXPECT_FALSE(maybeParentTag->isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTagTests, deleteFromCollection_ATrackTagWithChildren_DeletesTheTrackTagAndItsChildrenAndSetTheChildrenParentToNil)
{
    // -- Given.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());
    auto maybeParentTag = childrenTag.maybeParentTag();
    EXPECT_TRUE(maybeParentTag.isValid());

    // -- When.
    maybeParentTag->deleteFromCollection();

    // -- Then.
    EXPECT_FALSE(maybeParentTag->maybeParentTag().isValid());
    auto childrenTags = maybeParentTag->childrenTags();
    EXPECT_EQ(0u, childrenTags.length());
    EXPECT_EQ(0u, childrenTag.childrenTags().length());
    EXPECT_TRUE(childrenTag.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(maybeParentTag->isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTagTests, deleteFromCollection_ATrackTagChildren_DeletesTheChildrenAndItsParentTrackTagAndSetTheChildrenParentToNil)
{
    // -- Given.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());
    auto maybeParentTag = childrenTag.maybeParentTag();
    EXPECT_TRUE(maybeParentTag.isValid());

    // -- When.
    childrenTag.deleteFromCollection();

    // -- Then.
    EXPECT_FALSE(maybeParentTag->maybeParentTag().isValid());
    auto childrenTags = maybeParentTag->childrenTags();
    EXPECT_EQ(0u, childrenTags.length());
    EXPECT_EQ(0u, childrenTag.childrenTags().length());
    EXPECT_TRUE(childrenTag.isDeletedOrNotInAnyCollection());
    EXPECT_TRUE(maybeParentTag->isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTagTests, deleteFromCollection_ATrackTagWithMoreThanOneChild_DeletesTheTrackTagButNotItsParentTrackTagAndSetTheChildrenParentToNil)
{
    // -- Given.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());
    auto otherTag = TrackTag::withValueInCollection("Test:OtherTag", this->collection().asReference());
    auto maybeParentTag = childrenTag.maybeParentTag();
    EXPECT_TRUE(maybeParentTag.isValid());

    // -- When.
    childrenTag.deleteFromCollection();

    // -- Then.
    EXPECT_FALSE(maybeParentTag->maybeParentTag().isValid());
    auto childrenTags = maybeParentTag->childrenTags();
    EXPECT_EQ(1u, childrenTags.length());
    EXPECT_TRUE(childrenTags[0] == otherTag);
    EXPECT_EQ(0u, childrenTag.childrenTags().length());
    EXPECT_TRUE(childrenTag.isDeletedOrNotInAnyCollection());
    EXPECT_FALSE(otherTag.isDeletedOrNotInAnyCollection());
    EXPECT_FALSE(maybeParentTag->isDeletedOrNotInAnyCollection());
}

TEST_F(TrackTagTests, value_AChildrenTgags_ReturnsTheCorrectValue)
{
    // -- Given.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());

    // -- When.
    auto value = childrenTag.value();

    // -- Then.
    EXPECT_EQ(value, "Test:Tag"_String);
}

TEST_F(TrackTagTests, propertyDescription_AChildrenTgags_ReturnsTheCorrectValue)
{
    // -- Given.
    auto childrenTag = TrackTag::withValueInCollection("Test:Tag", this->collection().asReference());

    // -- When.
    auto value = childrenTag.propertyDescription();

    // -- Then.
    EXPECT_EQ(value, "Tag"_String);
}

} } }
