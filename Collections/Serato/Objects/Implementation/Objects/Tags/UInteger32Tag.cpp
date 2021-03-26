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

#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>
#include "Tags/Internal/UInteger32TagInternal.hpp"

#include <Base/Base.hpp>

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            UInteger32Tag
#define NXA_OBJECT_BASE_CLASS                       Tag

#include <Base/ObjectDefinition.ipp>

// -- Constructors/Destructors

UInteger32Tag::UInteger32Tag() : Tag{ std::make_shared<Internal>() } { }

// -- Factory Methods

UInteger32Tag UInteger32Tag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    auto tagData = Internal::dataForTagAt(tagAddress);
    NXA_ASSERT_EQ(Tag::dataSizeForTagAt(tagAddress), 4);

    return UInteger32Tag::tagWithIdentifierAndValue(Tag::identifierForTagAt(tagAddress),
                                                    Platform::bigEndianUInteger32ValueAt(tagData));
}

UInteger32Tag UInteger32Tag::tagWithIdentifierAndValue(uinteger32 identifier, uinteger32 value)
{
    NXA_ASSERT_EQ((identifier & 0xFF000000) >> 24, 'u');

    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = identifier;
    newInternal->value = value;

    return UInteger32Tag{ std::move(newInternal) };
}

// -- Instance Methods

uinteger32 UInteger32Tag::value() const
{
    return nxa_const_internal->value;
}
