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

#include <SeratoCollectionImplementation/Tags/BooleanTag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class BooleanTagTests : public NxA::Test
{

};

TEST_F(BooleanTagTests, TagWithIdentifierAndTrueValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', true);

    // -- Then.
    EXPECT_EQ('btst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(true, tag.value());
}

TEST_F(BooleanTagTests, TagWithIdentifierAndFalseValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', false);

    // -- Then.
    EXPECT_EQ('btst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(false, tag.value());
}

TEST_F(BooleanTagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(BooleanTag::tagWithIdentifierAndValue('stst', true), NxA::AssertionFailed);
}

TEST_F(BooleanTagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = { 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x01 };

    // -- When.
    auto tag = BooleanTag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('btst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(true, tag.value());
}

TEST_F(BooleanTagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = { 0x64, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x01 };

    // -- When.
    EXPECT_THROW(BooleanTag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(BooleanTagTests, TagWithMemoryAt_TagCreatedFromDataWithIncorrectSize_ReturnsAnUnknownTag)
{
    // -- Given.
    constexpr byte data[] = { 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0x01 };

    // -- When.
    // -- Then.
    EXPECT_THROW(BooleanTag::tagWithMemoryAt(data), AssertionFailed);
}

TEST_F(BooleanTagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', true);
    auto otherTag = BooleanTag::tagWithIdentifierAndValue('btst', true);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(BooleanTagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', true);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(BooleanTagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', true);
    auto otherTag = BooleanTag::tagWithIdentifierAndValue('btst', false);

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(BooleanTagTests, AddTo_TagWithTrueValue_WritesCorrectData)
{
    // -- Given.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', true);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = { 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x01 };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

TEST_F(BooleanTagTests, AddTo_TagWithFaleValue_WritesCorrectData)
{
    // -- Given.
    auto tag = BooleanTag::tagWithIdentifierAndValue('btst', false);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = { 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x00 };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
