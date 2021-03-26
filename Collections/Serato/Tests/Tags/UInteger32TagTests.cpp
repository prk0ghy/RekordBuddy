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

#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class UInteger32TagTests : public NxA::Test
{

};

TEST_F(UInteger32TagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);

    // -- Then.
    EXPECT_EQ('utst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(0xDEADBEEF, tag.value());
}

TEST_F(UInteger32TagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(UInteger32Tag::tagWithIdentifierAndValue('stst', 0xDEADBEEF), NxA::AssertionFailed);
}

TEST_F(UInteger32TagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = { 0x75, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x04, 0x54, 0xCA, 0x84, 0xAB };

    // -- When.
    auto tag = UInteger32Tag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('utst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(0x54CA84ABu, tag.value());
}

TEST_F(UInteger32TagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = { 0x12, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x04, 0x54, 0xCA, 0x84, 0xAB };

    // -- When.
    EXPECT_THROW(UInteger32Tag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(UInteger32TagTests, TagWithMemoryAt_TagCreatedFromDataWithIncorrectSize_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = { 0x75, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0x54, 0xCA };

    // -- When.
    // -- Then.
    EXPECT_THROW(UInteger32Tag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(UInteger32TagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);
    auto otherTag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(UInteger32TagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(UInteger32TagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);
    auto otherTag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEFF);

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(UInteger32TagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{
    // -- Given.
    auto tag = UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = { 0x75, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x04, 0xDE, 0xAD, 0xBE, 0xEF };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
