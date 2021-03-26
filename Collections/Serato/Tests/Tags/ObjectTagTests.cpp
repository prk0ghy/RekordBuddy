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

#include "ObjectTagTests.hpp"

using namespace testing;

namespace NxA { namespace Serato { namespace CollectionImplementation {

// -- Class Variables

byte ObjectTagTests::objectTagTestsTestData[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
Blob ObjectTagTests::objectTagTestsTestBlob{ Blob::withMemoryAndSize(ObjectTagTests::objectTagTestsTestData, sizeof(ObjectTagTests::objectTagTestsTestData)) };
String ObjectTagTests::objectTagTestsTestText{ "Some Text" };
String ObjectTagTests::objectTagTestsTestVersion{ "Cool Version" };

// -- Instance Methods

TEST_F(ObjectTagTests, TagWithIdentifierAndValue_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    tags.append(TextTag::tagWithIdentifierAndValue('ttst', ObjectTagTests::objectTagTestsTestText));
    tags.append(UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF));
    tags.append(UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF));
    tags.append(VersionTag::tagWithIdentifierAndValue('vtst', ObjectTagTests::objectTagTestsTestVersion));

    // -- When.
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));

    // -- Then.
    EXPECT_EQ('otst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(7u, tag.numberOfSubTags());

    auto maybeBlobTag = tag.maybeSubTagForIdentifier('atst');
    EXPECT_TRUE(maybeBlobTag.isValid());
    auto& blobTag = static_cast<BlobTag&>(*maybeBlobTag);
    EXPECT_EQ('atst', static_cast<int>(blobTag.identifier()));
    EXPECT_EQ(sizeof(ObjectTagTests::objectTagTestsTestData), blobTag.value().size());
    EXPECT_EQ(0, memcmp(ObjectTagTests::objectTagTestsTestData, blobTag.value().data().get(), sizeof(ObjectTagTests::objectTagTestsTestData)));

    auto maybeBooleanTag = tag.maybeSubTagForIdentifier('btst');
    EXPECT_TRUE(maybeBooleanTag.isValid());
    auto& booleanTag = static_cast<BooleanTag&>(*maybeBooleanTag);
    EXPECT_EQ('btst', static_cast<int>(booleanTag.identifier()));
    EXPECT_EQ(true, booleanTag.value());

    auto maybePathTag = tag.maybeSubTagForIdentifier('ptst');
    EXPECT_TRUE(maybePathTag.isValid());
    auto& pathTag = static_cast<PathTag&>(*maybePathTag);
    EXPECT_EQ('ptst', static_cast<int>(pathTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestPath().asEncodedString().asUTF8(), pathTag.value().asEncodedString().asUTF8());

    auto maybeTextTag = tag.maybeSubTagForIdentifier('ttst');
    EXPECT_TRUE(maybeTextTag.isValid());
    auto& textTag = static_cast<TextTag&>(*maybeTextTag);
    EXPECT_EQ('ttst', static_cast<int>(textTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestText.asUTF8(), textTag.value().asUTF8());

    auto maybeUInteger16Tag = tag.maybeSubTagForIdentifier('stst');
    EXPECT_TRUE(maybeUInteger16Tag.isValid());
    auto& uinteger16Tag = static_cast<UInteger16Tag&>(*maybeUInteger16Tag);
    EXPECT_EQ('stst', static_cast<int>(uinteger16Tag.identifier()));
    EXPECT_EQ(0xBEEF, uinteger16Tag.value());

    auto maybeUInteger32Tag = tag.maybeSubTagForIdentifier('utst');
    EXPECT_TRUE(maybeUInteger32Tag.isValid());
    auto& uinteger32Tag = static_cast<UInteger32Tag&>(*maybeUInteger32Tag);
    EXPECT_EQ('utst', static_cast<int>(uinteger32Tag.identifier()));
    EXPECT_EQ(0xDEADBEEF, uinteger32Tag.value());

    auto maybeVersionTag = tag.maybeSubTagForIdentifier('vtst');
    EXPECT_TRUE(maybeVersionTag.isValid());
    auto& versionTag = static_cast<VersionTag&>(*maybeVersionTag);
    EXPECT_EQ('vtst', static_cast<int>(versionTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestVersion.asUTF8(), versionTag.value().asUTF8());
}

TEST_F(ObjectTagTests, TagWithIdentifierAndValue_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));

    // -- When.
    // -- Then.
    EXPECT_THROW(ObjectTag::tagWithIdentifierAndValue('stst', std::move(tags)), NxA::AssertionFailed);
}

TEST_F(ObjectTagTests, TagFromAnEmptyArray_ThrowsException)
{
    // -- Given.
    Array<Tag> tags;

    // -- When.
    // -- Then.
    EXPECT_THROW(ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags)), NxA::AssertionFailed);
}

TEST_F(ObjectTagTests, TagWithMemoryAt_TagCreated_TagHasCorrectValue)
{
    // -- Given.
    constexpr byte data[] = {
        0x6F, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x9D, 0x61, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x01, 0x70,
        0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F, 0x00, 0x43, 0x00, 0x6F,
        0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68, 0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00,
        0x33, 0x73, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0xBE, 0xEF, 0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x12, 0x00, 0x53,
        0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x75, 0x74, 0x73, 0x74, 0x00,
        0x00, 0x00, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x76, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x18, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F,
        0x00, 0x6C, 0x00, 0x20, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E
    };

    // -- When.
    auto tag = ObjectTag::tagWithMemoryAt(data);

    // -- Then.
    EXPECT_EQ('otst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(7u, tag.numberOfSubTags());

    auto maybeBlobTag = tag.maybeSubTagForIdentifier('atst');
    EXPECT_TRUE(maybeBlobTag.isValid());
    auto& blobTag = static_cast<BlobTag&>(*maybeBlobTag);
    EXPECT_EQ('atst', static_cast<int>(blobTag.identifier()));
    EXPECT_EQ(sizeof(ObjectTagTests::objectTagTestsTestData), blobTag.value().size());
    EXPECT_EQ(0, memcmp(ObjectTagTests::objectTagTestsTestData, blobTag.value().data().get(), sizeof(ObjectTagTests::objectTagTestsTestData)));

    auto maybeBooleanTag = tag.maybeSubTagForIdentifier('btst');
    EXPECT_TRUE(maybeBooleanTag.isValid());
    auto& booleanTag = static_cast<BooleanTag&>(*maybeBooleanTag);
    EXPECT_EQ('btst', static_cast<int>(booleanTag.identifier()));
    EXPECT_EQ(true, booleanTag.value());

    auto maybePathTag = tag.maybeSubTagForIdentifier('ptst');
    EXPECT_TRUE(maybePathTag.isValid());
    auto& pathTag = static_cast<PathTag&>(*maybePathTag);
    EXPECT_EQ('ptst', static_cast<int>(pathTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestPath().asEncodedString().asUTF8(), pathTag.value().asEncodedString().asUTF8());

    auto maybeTextTag = tag.maybeSubTagForIdentifier('ttst');
    EXPECT_TRUE(maybeTextTag.isValid());
    auto& textTag = static_cast<TextTag&>(*maybeTextTag);
    EXPECT_EQ('ttst', static_cast<int>(textTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestText.asUTF8(), textTag.value().asUTF8());

    auto maybeUInteger16Tag = tag.maybeSubTagForIdentifier('stst');
    EXPECT_TRUE(maybeUInteger16Tag.isValid());
    auto& uinteger16Tag = static_cast<UInteger16Tag&>(*maybeUInteger16Tag);
    EXPECT_EQ('stst', static_cast<int>(uinteger16Tag.identifier()));
    EXPECT_EQ(0xBEEF, uinteger16Tag.value());

    auto maybeUInteger32Tag = tag.maybeSubTagForIdentifier('utst');
    EXPECT_TRUE(maybeUInteger32Tag.isValid());
    auto& uinteger32Tag = static_cast<UInteger32Tag&>(*maybeUInteger32Tag);
    EXPECT_EQ('utst', static_cast<int>(uinteger32Tag.identifier()));
    EXPECT_EQ(0xDEADBEEF, uinteger32Tag.value());

    auto maybeVersionTag = tag.maybeSubTagForIdentifier('vtst');
    EXPECT_TRUE(maybeVersionTag.isValid());
    auto& versionTag = static_cast<VersionTag&>(*maybeVersionTag);
    EXPECT_EQ('vtst', static_cast<int>(versionTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestVersion.asUTF8(), versionTag.value().asUTF8());
}

TEST_F(ObjectTagTests, TagWithMemoryAt_IncorrectIdentifier_ThrowsException)
{
    // -- Given.
    constexpr byte data[] = {
        0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F,
        0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68,
        0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x33
    };

    // -- When.
    // -- Then.
    EXPECT_THROW(ObjectTag::tagWithMemoryAt(data), AssertionFailed);
}

TEST_F(ObjectTagTests, OperatorEqual_TwoEqualTags_ReturnsTrue)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));
    MutableArray<Tag> otherTags;
    otherTags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    otherTags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    otherTags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto otherTag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(otherTags));

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == otherTag);
}

TEST_F(ObjectTagTests, OperatorEqual_TwoEqualTagsSameObject_ReturnsTrue)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag == tag);
}

TEST_F(ObjectTagTests, OperatorEqual_TwoUnEqualTagsDifferentNumberOfSubtags_Returnsfalse)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));
    MutableArray<Tag> otherTags;
    otherTags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    otherTags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    auto otherTag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(otherTags));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(ObjectTagTests, OperatorEqual_TwoUnEqualTagsDifferentSubtags_Returnsfalse)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    tags.append(UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));
    MutableArray<Tag> otherTags;
    otherTags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    otherTags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    auto otherTag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(otherTags));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(ObjectTagTests, OperatorEqual_TwoUnEqualTagsDifferentSubtagsValues_Returnsfalse)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));
    MutableArray<Tag> otherTags;
    otherTags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    otherTags.append(BooleanTag::tagWithIdentifierAndValue('btst', false));
    otherTags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto otherTag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(otherTags));

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag == otherTag);
}

TEST_F(ObjectTagTests, SetSubTag_TagWithExistingSubTagForThatIdentifier_SetsTheValueCorrectly)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(BooleanTag::tagWithIdentifierAndValue('btst', true));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));

    // -- When.
    tag.setSubTag(BooleanTag::tagWithIdentifierAndValue('btst', false));

    // -- Then.
    EXPECT_EQ('otst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(3u, tag.numberOfSubTags());

    auto maybeBlobTag = tag.maybeSubTagForIdentifier('atst');
    EXPECT_TRUE(maybeBlobTag.isValid());
    auto& blobTag = static_cast<BlobTag&>(*maybeBlobTag);
    EXPECT_EQ('atst', static_cast<int>(blobTag.identifier()));
    EXPECT_EQ(sizeof(ObjectTagTests::objectTagTestsTestData), blobTag.value().size());
    EXPECT_EQ(0, memcmp(ObjectTagTests::objectTagTestsTestData, blobTag.value().data().get(), sizeof(ObjectTagTests::objectTagTestsTestData)));

    auto maybeBooleanTag = tag.maybeSubTagForIdentifier('btst');
    EXPECT_TRUE(maybeBooleanTag.isValid());
    auto& booleanTag = static_cast<BooleanTag&>(*maybeBooleanTag);
    EXPECT_EQ('btst', static_cast<int>(booleanTag.identifier()));
    EXPECT_EQ(false, booleanTag.value());

    auto maybePathTag = tag.maybeSubTagForIdentifier('ptst');
    EXPECT_TRUE(maybePathTag.isValid());
    auto& pathTag = static_cast<PathTag&>(*maybePathTag);
    EXPECT_EQ('ptst', static_cast<int>(pathTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestPath().asEncodedString().asUTF8(), pathTag.value().asEncodedString().asUTF8());
}

TEST_F(ObjectTagTests, SetSubTag_TagWithSubTagForANewIdentifier_SetsTheValueCorrectly)
{
    // -- Given.
    MutableArray<Tag> tags;
    tags.append(BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob));
    tags.append(PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()));
    auto tag = ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));

    // -- When.
    tag.setSubTag(BooleanTag::tagWithIdentifierAndValue('btst', false));

    // -- Then.
    EXPECT_EQ('otst', static_cast<int>(tag.identifier()));
    EXPECT_EQ(3u, tag.numberOfSubTags());
    EXPECT_TRUE(tag.maybeSubTagForIdentifier('atst').isValid());
    EXPECT_TRUE(tag.maybeSubTagForIdentifier('btst').isValid());
    EXPECT_TRUE(tag.maybeSubTagForIdentifier('ptst').isValid());


    auto maybeBlobTag = tag.maybeSubTagForIdentifier('atst');
    EXPECT_TRUE(maybeBlobTag.isValid());
    auto& blobTag = static_cast<BlobTag&>(*maybeBlobTag);
    EXPECT_EQ('atst', static_cast<int>(blobTag.identifier()));
    EXPECT_EQ(sizeof(ObjectTagTests::objectTagTestsTestData), blobTag.value().size());
    EXPECT_EQ(0, memcmp(ObjectTagTests::objectTagTestsTestData, blobTag.value().data().get(), sizeof(ObjectTagTests::objectTagTestsTestData)));

    auto maybeBooleanTag = tag.maybeSubTagForIdentifier('btst');
    EXPECT_TRUE(maybeBooleanTag.isValid());
    auto& booleanTag = static_cast<BooleanTag&>(*maybeBooleanTag);
    EXPECT_EQ('btst', static_cast<int>(booleanTag.identifier()));
    EXPECT_EQ(false, booleanTag.value());

    auto maybePathTag = tag.maybeSubTagForIdentifier('ptst');
    EXPECT_TRUE(maybePathTag.isValid());
    auto& pathTag = static_cast<PathTag&>(*maybePathTag);
    EXPECT_EQ('ptst', static_cast<int>(pathTag.identifier()));
    EXPECT_STREQ(ObjectTagTests::objectTagTestsTestPath().asEncodedString().asUTF8(), pathTag.value().asEncodedString().asUTF8());
}

TEST_F(ObjectTagTests, AddTo_TagWrittenToBlob_WriteCorrectData)
{

    // -- Given.
    auto tag = ObjectTagTests::testObjectTag();
    MutableBlob destination;

    // -- When.
    tag.addTo(destination);

    // -- Then.
    auto data = destination.data();
    constexpr byte expectedData[] = {
        0x6F, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x9D, 0x61, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x62, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x01, 0x01, 0x70,
        0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x24, 0x00, 0x53, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F, 0x00, 0x43, 0x00, 0x6F,
        0x00, 0x6F, 0x00, 0x6C, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x74, 0x00, 0x68, 0x00, 0x2E, 0x00, 0x6D, 0x00, 0x70, 0x00,
        0x33, 0x73, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0xBE, 0xEF, 0x74, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x12, 0x00, 0x53,
        0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x75, 0x74, 0x73, 0x74, 0x00,
        0x00, 0x00, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x76, 0x74, 0x73, 0x74, 0x00, 0x00, 0x00, 0x18, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F,
        0x00, 0x6C, 0x00, 0x20, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E
    };
    EXPECT_EQ(sizeof(expectedData), destination.size());
    EXPECT_EQ(0, ::memcmp(expectedData, data.get(), sizeof(expectedData)));
}

} } }
