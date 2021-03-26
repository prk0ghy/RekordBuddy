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

#include <SeratoCollectionImplementation/Tags/UInteger16Tag.hpp>
#include "Tags/Internal/TagInternal.hpp"

#include <Base/Base.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

#include <Base/ResetObjectForwardDeclarations.ipp>

// -- Forward Declarations
#define NXA_OBJECT_CLASS                UInteger16Tag
#define NXA_OBJECT_INTERNAL_BASE_CLASS  TagInternal

// -- Internal Interface
class UInteger16TagInternal : public NXA_OBJECT_INTERNAL_BASE_CLASS
{
    #include <Base/ObjectInternal.ipp>

public:
    // -- Constructors & Destructors
    UInteger16TagInternal() : value(0) { }
    UInteger16TagInternal(const UInteger16TagInternal&) = default;
    UInteger16TagInternal(UInteger16TagInternal&) = default;
    UInteger16TagInternal(UInteger16TagInternal&&) = default;
    virtual ~UInteger16TagInternal() = default;

    // -- Instance Variables
    uinteger16 value;

    // -- Operators
    bool operator==(const UInteger16TagInternal& other) const
    {
        if (!this->TagInternal::operator==(other)) {
            return false;
        }

        return this->value == other.value;
    }
    bool operator<(const UInteger16TagInternal& other) const
    {
        if (!this->TagInternal::operator<(other)) {
            return false;
        }

        return this->value < other.value;
    }

    // -- Instance Methods
    virtual void addTo(MutableBlob& destination) const override
    {
        count dataSize = 2;
        auto memoryRepresentation = MutableBlob::withCapacity(UInteger16TagInternal::memoryNeededForTagHeader() + dataSize);

        auto tagAddress = memoryRepresentation.data();
        UInteger16TagInternal::setIdentifierForTagAt(this->identifier, tagAddress);
        UInteger16TagInternal::setDataSizeForTagAt(dataSize, tagAddress);
        Platform::writeBigEndianUInteger16ValueAt(this->value, UInteger16TagInternal::dataForTagAt(tagAddress));

        destination.append(std::move(memoryRepresentation));
    }
};

} } }
