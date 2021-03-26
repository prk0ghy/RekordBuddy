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

#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

static String testVersionTagTestsVersion("Cool Version");

class VersionTagTests : public NxA::Test
{

};

TEST_F(VersionTagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = VersionTag::tagWithIdentifierAndValue('vtst', testVersionTagTestsVersion);

    // -- Then.
    EXPECT_EQ('vtst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(testVersionTagTestsVersion.asUTF8(), tag.value().asUTF8());
}

TEST_F(VersionTagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(VersionTag::tagWithIdentifierAndValue('stst', testVersionTagTestsVersion), NxA::AssertionFailed);
}

TEST_F(VersionTagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = {
        0x76, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x18, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C,
        0x00, 0x20, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E
    };

    // -- When.
    auto tag = VersionTag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('vtst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(testVersionTagTestsVersion.asUTF8(), tag.value().asUTF8());
}

TEST_F(VersionTagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = {
        0x66, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x18, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C,
        0x00, 0x20, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E
    };

    // -- When.
    EXPECT_THROW(VersionTag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(VersionTagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = VersionTag::tagWithIdentifierAndValue('vtst', testVersionTagTestsVersion);
    auto otherTag = VersionTag::tagWithIdentifierAndValue('vtst', String("Cool Version"));

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(VersionTagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = VersionTag::tagWithIdentifierAndValue('vtst', testVersionTagTestsVersion);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(VersionTagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = VersionTag::tagWithIdentifierAndValue('vtst', testVersionTagTestsVersion);
    auto otherTag = VersionTag::tagWithIdentifierAndValue('vtst', String("Some Other Version"));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(VersionTagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{
    // -- Given.
    auto tag = VersionTag::tagWithIdentifierAndValue('vtst', testVersionTagTestsVersion);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = {
        0x76, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x18, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C,
        0x00, 0x20, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E
    };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
