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

#include <SeratoCollectionImplementation/Tags/Tag.hpp>

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

class TagTests : public NxA::Test
{

};

TEST_F(TagTests, IdentifierForTagAt_TagDataWithAGivenIdentifier_ReturnsTheTagIdentifier)
{
    // -- Given.
    constexpr byte data[] = { 0x76, 0x72, 0x73, 0x6E, 0x00, 0x00, 0x00, 0x04, 0x54, 0xCA, 0x84, 0xAB };

    // -- When.
    auto identifier = Tag::identifierForTagAt(data);

    // -- Then.
    EXPECT_EQ('vrsn', static_cast<int>(identifier));
}

TEST_F(TagTests, NextTagAfterTagAt_TagDataWithTwoTags_ReturnsAPointerToTheSecondTag)
{
    // -- Given.
    constexpr byte data[] = {
        0x76, 0x72, 0x73, 0x6E, 0x00, 0x00, 0x00, 0x04, 0x54, 0xCA, 0x84, 0xAB,
        0x76, 0x72, 0x73, 0x6E, 0x00, 0x00, 0x00, 0x04, 0x54, 0xCA, 0x84, 0xAB
    };

    // -- When.
    auto nextTag = Tag::nextTagAfterTagAt(data);

    // -- Then.
    EXPECT_EQ(nextTag.get(), data + 12);
}

} } }
