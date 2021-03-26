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

#include <SeratoCollectionImplementation/Tags/UInteger16Tag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class UInteger16TagTests : public NxA::Test
{

};

TEST_F(UInteger16TagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);

    // -- Then.
    EXPECT_EQ('stst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(0xBEEF, tag.value());
}

TEST_F(UInteger16TagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(UInteger16Tag::tagWithIdentifierAndValue('vtst', 0xBEEF), NxA::AssertionFailed);
}

TEST_F(UInteger16TagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = { 0x73, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0x54, 0xCA };

    // -- When.
    auto tag = UInteger16Tag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('stst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(0x54CA, tag.value());
}

TEST_F(UInteger16TagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = { 0x12, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0x54, 0xCA };

    // -- When.
    EXPECT_THROW(UInteger16Tag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(UInteger16TagTests, TagWithMemoryAt_TagCreatedFromDataWithIncorrectSize_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = { 0x73, 0x74, 0x73, 0x74, 0x00, 0x00, 0x02, 0x02, 0x54, 0xCA };

    // -- When.
    // -- Then.
    EXPECT_THROW(UInteger16Tag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(UInteger16TagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);
    auto otherTag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);

    // -- When.
    // -- Then.
    EXPECT_EQ(tag, otherTag);
}

TEST_F(UInteger16TagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);

    // -- When.
    // -- Then.
    EXPECT_EQ(tag, tag);
}

TEST_F(UInteger16TagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);
    auto otherTag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xDEAD);

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(UInteger16TagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{
    // -- Given.
    auto tag = UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF);
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = { 0x73, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0xBE, 0xEF };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
