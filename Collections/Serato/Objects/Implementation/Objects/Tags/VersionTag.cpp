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

#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>
#include "Tags/Internal/VersionTagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            VersionTag
#define NXA_OBJECT_BASE_CLASS                       TextTag

#include <Base/ObjectDefinition.ipp>

// -- Constructors/Destructors

VersionTag::VersionTag() : TextTag{ std::make_shared<Internal>() } { }

// -- Factory Methods

VersionTag VersionTag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    count size = Tag::dataSizeForTagAt(tagAddress);
    auto text = String::stringWithUTF16(Blob::withMemoryAndSize(Internal::dataForTagAt(tagAddress), size));

    return VersionTag::tagWithIdentifierAndValue(Tag::identifierForTagAt(tagAddress), text);
}

VersionTag VersionTag::tagWithIdentifierAndValue(uinteger32 identifier, const String& value)
{
    NXA_ASSERT_EQ((identifier & 0xFF000000) >> 24, 'v');

    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = identifier;
    newInternal->value = value;

    return VersionTag{ std::move(newInternal) };
}
