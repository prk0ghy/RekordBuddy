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

#include <SeratoCollectionImplementation/Tags/TagFactory.hpp>
#include <SeratoCollectionImplementation/Tags/BooleanTag.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>
#include <SeratoCollectionImplementation/Tags/TextTag.hpp>
#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>
#include <SeratoCollectionImplementation/Tags/BlobTag.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/Tags/UInteger16Tag.hpp>
#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>
#include <SeratoCollectionImplementation/Tags/UnknownTag.hpp>

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

// -- Class Methods

Tag TagFactory::tagForTagAt(NotNull<const byte*> tagAddress)
{
    uinteger32 identifier = Tag::identifierForTagAt(tagAddress);
    char typeIdentifier = (identifier >> 24) & 0xff;

    switch (typeIdentifier) {
        case 'b': {
            if (BooleanTag::tagIsValidAt(tagAddress)) {
                return BooleanTag::tagWithMemoryAt(tagAddress);
            }
            break;
        }
        case 's': {
            return UInteger16Tag::tagWithMemoryAt(tagAddress);
        }
        case 'u': {
            return UInteger32Tag::tagWithMemoryAt(tagAddress);
        }
        case 'v': {
            return VersionTag::tagWithMemoryAt(tagAddress);
        }
        case 't': {
            if (TextTag::tagIsValidAt(tagAddress)) {
                return TextTag::tagWithMemoryAt(tagAddress);
            }
            break;
        }
        case 'p': {
            return PathTag::tagWithMemoryAt(tagAddress);
        }
        case 'a': {
            return BlobTag::tagWithMemoryAt(tagAddress);
        }
        case 'o': {
            return ObjectTag::tagWithMemoryAt(tagAddress);
        }
    }

    return UnknownTag::tagWithMemoryAt(tagAddress);
}

Array<Tag> TagFactory::parseTagsAt(NotNull<const byte*> firstTagAddress, count sizeFromFirstTag)
{
    auto tagAddress = firstTagAddress;
    auto endOfTagsAddress = tagAddress + sizeFromFirstTag;

    MutableArray<Tag> newTags;

    while (tagAddress < endOfTagsAddress) {
        auto size = Tag::dataSizeForTagAt(tagAddress);
        auto nextTag = Tag::nextTagAfterTagAtWhichHasDataSize(tagAddress, size);
        if (!size || (nextTag > endOfTagsAddress)) {
            break;
        }

        newTags.append(TagFactory::tagForTagAt(tagAddress));

        tagAddress = nextTag;
    }

    return std::move(newTags);
}
