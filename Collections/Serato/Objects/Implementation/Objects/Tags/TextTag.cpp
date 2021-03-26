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

#include <SeratoCollectionImplementation/Tags/TextTag.hpp>
#include "Tags/Internal/TextTagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            TextTag
#define NXA_OBJECT_BASE_CLASS                       Tag

#include <Base/ObjectDefinition.ipp>

// -- Constructors/Destructors

TextTag::TextTag() : Tag{ std::make_shared<Internal>() } { }

// -- Factory Methods

TextTag TextTag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    count size = Tag::dataSizeForTagAt(tagAddress);
    auto text = size ? String::stringWithUTF16AtAndSizeInBytes(Internal::dataForTagAt(tagAddress), size) : String();

    return TextTag::tagWithIdentifierAndValue(Tag::identifierForTagAt(tagAddress), text);
}

TextTag TextTag::tagWithIdentifierAndValue(uinteger32 identifier, const String& value)
{
    NXA_ASSERT_EQ((identifier & 0xFF000000) >> 24, 't');

    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = identifier;

    if (value.hasNonPrintableCharacters()) {
        newInternal->value = String::stringByFilteringNonPrintableCharactersIn(value);
    }
    else {
        newInternal->value = value;
    }

    return TextTag{ std::move(newInternal) };
}

// -- Class Methods

boolean TextTag::tagIsValidAt(NotNull<const byte*> tagAddress)
{
    return Tag::dataSizeForTagAt(tagAddress) > 1;
}

// -- Instance Methods

String TextTag::value() const
{
    return nxa_const_internal->value;
}
