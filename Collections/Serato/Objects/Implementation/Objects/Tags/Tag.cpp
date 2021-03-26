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

#include <SeratoCollectionImplementation/Tags/Tag.hpp>
#include "Tags/Internal/TagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            Tag

#include <Base/ObjectDefinition.ipp>

// -- Class Methods

uint32_t Tag::identifierForTagAt(NotNull<const byte*> tagAddress)
{
    const Internal::TagStruct* tagStructPtr = reinterpret_cast<const Internal::TagStruct*>(tagAddress.get());
    uint32_t identifier = Platform::bigEndianUInteger32ValueAt(tagStructPtr->identifier);
    return identifier;
}

NotNull<const byte*> Tag::nextTagAfterTagAtWhichHasDataSize(NotNull<const byte*> tagAddress, count dataSize)
{
    return tagAddress.get() + dataSize + sizeof(Internal::TagStruct);
}

NotNull<const byte*> Tag::nextTagAfterTagAt(NotNull<const byte*> tagAddress)
{
    return Tag::nextTagAfterTagAtWhichHasDataSize(tagAddress, Tag::dataSizeForTagAt(tagAddress));
}

count Tag::dataSizeForTagAt(NotNull<const byte*> tagAddress)
{
    const Internal::TagStruct* tagStructPtr = reinterpret_cast<const Internal::TagStruct*>(tagAddress.get());
    unsigned long dataSize = Platform::bigEndianUInteger32ValueAt(tagStructPtr->length);
    return dataSize;
}

String Tag::descriptionForIdentifier(uinteger32 identifier)
{
    return String::stringWithFormat("%c%c%c%c",
                                    (identifier >> 24) & 0xff,
                                    (identifier >> 16) & 0xff,
                                    (identifier >> 16) & 0xff,
                                    (identifier) & 0xff
                                    );
}

// -- Instance Methods

uinteger32 Tag::identifier() const
{
    return nxa_const_internal->identifier;
}

void Tag::addTo(MutableBlob& destination) const
{
    nxa_const_internal->addTo(destination);
}
