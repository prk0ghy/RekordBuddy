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

#include <Base/Test.hpp>

#include <SeratoCollectionImplementation/TrackEntry.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>
#include <SeratoCollectionImplementation/Tags/CrateV1Tags.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class TrackEntryTests : public NxA::Test
{

};

TEST_F(TrackEntryTests, MaybeRelativePathForEntryWithTag_TagWithValidData_ReturnsAValidPath)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(PathTag::tagWithIdentifierAndValue(trackEntryPathTagIdentifier, FilePath("Music/Gigs/02 Money On My Mind (MK Remix).m4a")));
    auto tag = ObjectTag::tagWithIdentifierAndValue(trackEntryTagIdentifier, std::move(tags));

    // -- When.
    auto path = TrackEntry::maybeRelativePathForEntryWithTag(tag);

    // -- Then.
    EXPECT_TRUE(path.isValid());
    EXPECT_STREQ("Music/Gigs/02 Money On My Mind (MK Remix).m4a", path->asEncodedString().asUTF8());
}

TEST_F(TrackEntryTests, MaybeRelativePathForEntryWithTag_TagWithNoPathTag_ReturnsNothing)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(PathTag::tagWithIdentifierAndValue('poot', FilePath("Music/Gigs/02 Money On My Mind (MK Remix).m4a")));
    auto tag = ObjectTag::tagWithIdentifierAndValue(trackEntryTagIdentifier, std::move(tags));

    // -- When.
    auto path = TrackEntry::maybeRelativePathForEntryWithTag(tag);

    // -- Then.
    EXPECT_FALSE(path.isValid());
}

TEST_F(TrackEntryTests, TagForRelativeFilePath_AFilePath_ReturnsTheCorrectTag)
{
    // -- Given.
    auto filePath = FilePath("Users/didier/Music/Gigs/02 Money On My Mind (MK Remix).m4a");

    // -- When.
    auto tag = TrackEntry::tagForRelativeFilePath(filePath);

    // -- Then.
    auto& objectTag = static_cast<const ObjectTag&>(tag);
    EXPECT_EQ(trackEntryTagIdentifier, objectTag.identifier());
    EXPECT_EQ(1u, objectTag.numberOfSubTags());
    auto maybeTag = objectTag.maybeSubTagForIdentifier(trackEntryPathTagIdentifier);
    EXPECT_TRUE(maybeTag.isValid());
    auto pathTag = static_cast<PathTag&>(*maybeTag);
    EXPECT_EQ(trackEntryPathTagIdentifier, pathTag.identifier());
    EXPECT_NE(filePath.asEncodedString().asUTF8()[0], '/');
    EXPECT_STREQ(filePath.asEncodedString().asUTF8(), pathTag.value().asEncodedString().asUTF8());
}

} } }
