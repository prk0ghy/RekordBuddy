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
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>

#include "../../Objects/Implementation/Objects/Tags/Internal/TextTagInternal.hpp"

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class PathTagTests : public NxA::Test
{
public:
    // -- Class Methods
    static FilePath pathTagTestsPath()
    {
        return NXA_FILEPATH("Some/Cool/path.mp3");
    }
};

TEST_F(PathTagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    // -- When.
    auto tag = PathTag::tagWithIdentifierAndValue('ptst', this->pathTagTestsPath());

    // -- Then.
    EXPECT_EQ('ptst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(this->pathTagTestsPath().asEncodedString().asUTF8(), tag.value().asEncodedString().asUTF8());
}

TEST_F(PathTagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    // -- When.
    // -- Then.
    EXPECT_THROW(PathTag::tagWithIdentifierAndValue('stst', this->pathTagTestsPath()), NxA::AssertionFailed);
}

TEST_F(PathTagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = {
        0x70, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F,
        0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68,
        0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x33
    };

    // -- When.
    auto tag = PathTag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('ptst', static_cast<int>(tag.identifier()));
    EXPECT_STREQ(this->pathTagTestsPath().asEncodedString().asUTF8(), tag.value().asEncodedString().asUTF8());
}

TEST_F(PathTagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = {
        0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F,
        0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68,
        0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x33
    };

    // -- When.
    EXPECT_THROW(PathTag::tagWithMemoryAt(data), NxA::AssertionFailed);
}

TEST_F(PathTagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    auto tag = PathTag::tagWithIdentifierAndValue('ptst', this->pathTagTestsPath());
    auto otherTag = PathTag::tagWithIdentifierAndValue('ptst', FilePath("Some/Cool/path.mp3"));

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(PathTagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    auto tag = PathTag::tagWithIdentifierAndValue('ptst', this->pathTagTestsPath());

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(PathTagTests, OperatorEqual_TwoUnEqualTags_Returnsfalse)
{
    // -- Given.
    auto tag = PathTag::tagWithIdentifierAndValue('ptst', this->pathTagTestsPath());
    auto otherTag = PathTag::tagWithIdentifierAndValue('ptst', FilePath("Some/Other/path.mp3"));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(PathTagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{
    // -- Given.
    auto tag = PathTag::tagWithIdentifierAndValue('ptst', this->pathTagTestsPath());
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = {
        0x70, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F,
        0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68,
        0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x33
    };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

TEST_F(PathTagTests, PathTagAndTextTagWithSameValue_OperatorEqual_ReturnsFalse)
{
    // -- Given.
    auto pathTag = PathTag::tagWithIdentifierAndValue('ptst', NXA_FILEPATH("Some/Cool/path.mp3"));
    auto newInternal = std::make_shared<TextTagInternal>();
    newInternal->identifier = 'ptst';
    newInternal->value = "Some/Cool/path.mp3";

    auto textTag = TextTagInternal::toWrapperClass(std::move(newInternal));

    // -- When.
    // -- Then.
    EXPECT_FALSE(textTag == pathTag);
}

} } }
