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

#include <SeratoCollectionImplementation/Tags/TextTag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

static String testTagTestText("Some Text");

class TextTagTests : public NxA::Test
{

};

TEST_F(TextTagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = TextTag::tagWithIdentifierAndValue('ttst', testTagTestText);

    // -- Then.
    EXPECT_EQ('ttst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(testTagTestText.asUTF8(), tag.value().asUTF8());
}

TEST_F(TextTagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(TextTag::tagWithIdentifierAndValue('stst', testTagTestText), NxA::AssertionFailed);
}

TEST_F(TextTagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = {
        0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x12, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65,
        0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74
    };

    // -- When.
    auto tag = TextTag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('ttst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(testTagTestText.asUTF8(), tag.value().asUTF8());
}

TEST_F(TextTagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = {
        0x14, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x12, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65,
        0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74
    };

    // -- When.
    EXPECT_THROW(TextTag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(TextTagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = TextTag::tagWithIdentifierAndValue('ttst', testTagTestText);
    auto otherTag = TextTag::tagWithIdentifierAndValue('ttst', String("Some Text"));

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(TextTagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = TextTag::tagWithIdentifierAndValue('ttst', testTagTestText);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(TextTagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = TextTag::tagWithIdentifierAndValue('ttst', testTagTestText);
    auto otherTag = TextTag::tagWithIdentifierAndValue('ttst', String("Some Other Text"));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(TextTagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{
    // -- Given.
    auto tag = TextTag::tagWithIdentifierAndValue('ttst', testTagTestText);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = {
        0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x12, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65,
        0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74
    };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
