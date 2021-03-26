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

#pragma once

#include <SeratoCollectionImplementation/Tags/Tag.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                                    Tag
#define NXA_INTERNAL_OBJECT_IS_PURE_VIRTUAL
#define NXA_INTERNAL_OBJECT_DOES_NOT_NEED_EQUAL_OPERATOR

// -- Internal Interface
class TagInternal
{
    #include <Base/ObjectInternal.ipp>

public:
    // -- Types
    struct TagStruct {
        byte identifier[4];
        byte length[4];
        byte data[0];
    };

    // -- Constructors & Destructors
    TagInternal() = default;
    virtual ~TagInternal() = default;

    // -- Class Methods
    static void setIdentifierForTagAt(uinteger32 identifier, NotNull<byte*> tagAddress)
    {
        auto tagStructPtr = reinterpret_cast<TagStruct*>(tagAddress.get());
        Platform::writeBigEndianUInteger32ValueAt(identifier, tagStructPtr->identifier);
    }

    static void setDataSizeForTagAt(count dataSize, NotNull<byte*> tagAddress)
    {
        auto tagStructPtr = reinterpret_cast<TagStruct*>(tagAddress.get());
        Platform::writeBigEndianUInteger32ValueAt(uinteger32(dataSize), tagStructPtr->length);
    }

    static count memoryNeededForTagHeader()
    {
        return sizeof(TagStruct);
    }

    static NotNull<const byte*> dataForTagAt(NotNull<const byte*> tagAddress)
    {
        auto tagStructPtr = reinterpret_cast<const TagStruct*>(tagAddress.get());
        return tagStructPtr->data;
    }

    static NotNull<byte*> dataForTagAt(NotNull<byte*> tagAddress)
    {
        auto tagStructPtr = reinterpret_cast<TagStruct*>(tagAddress.get());
        return reinterpret_cast<byte*>(tagStructPtr->data);
    }

    // -- Instance Variables
    uinteger32 identifier{};

    // -- Operators
    inline virtual bool operator==(const TagInternal& other) const noexcept
    {
        return this->identifier == other.identifier;
    }
    inline virtual bool operator<(const TagInternal& other) const noexcept
    {
        return this->identifier < other.identifier;
    }

    // -- Instance Methods
    virtual void addTo(MutableBlob& destination) const = 0;
};

} } }
