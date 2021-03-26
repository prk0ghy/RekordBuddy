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

#include <SeratoCollectionImplementation/Tags/BooleanTag.hpp>

#include "Tags/Internal/BooleanTagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            BooleanTag
#define NXA_OBJECT_BASE_CLASS                       Tag

#include <Base/ObjectDefinition.ipp>

// -- Constructors/Destructors

BooleanTag::BooleanTag() : Tag{ std::make_shared<Internal>() } { }

// -- Factory Methods

BooleanTag BooleanTag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    auto tagData = Internal::dataForTagAt(tagAddress);
    auto identifier = Tag::identifierForTagAt(tagAddress);
    auto size = Tag::dataSizeForTagAt(tagAddress);
    NXA_ASSERT_TRUE(size == 1);

    return BooleanTag::tagWithIdentifierAndValue(identifier, (*tagData == 1) ? true : false);
}

BooleanTag BooleanTag::tagWithIdentifierAndValue(uinteger32 identifier, boolean value)
{
    NXA_ASSERT_EQ((identifier & 0xFF000000) >> 24, 'b');

    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = identifier;
    newInternal->value = value;

    return BooleanTag{ std::move(newInternal) };
}

// -- Class Methods

boolean BooleanTag::tagIsValidAt(NotNull<const byte*> tagAddress)
{
    return Tag::dataSizeForTagAt(tagAddress) == 1;
}

// -- Instance Methods

boolean BooleanTag::value() const
{
    return nxa_const_internal->value;
}
