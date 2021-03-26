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

#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>
#include "Tags/Internal/TagInternal.hpp"

#include <Base/Base.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

#include <Base/ResetObjectForwardDeclarations.ipp>

// -- Forward Declarations
#define NXA_OBJECT_CLASS                UInteger32Tag
#define NXA_OBJECT_INTERNAL_BASE_CLASS  TagInternal

// -- Public Interface
class UInteger32TagInternal : public NXA_OBJECT_INTERNAL_BASE_CLASS
{
    #include <Base/ObjectInternal.ipp>

public:
    // -- Constructors & Destructors
    UInteger32TagInternal() : value(0) { }
    UInteger32TagInternal(const UInteger32TagInternal&) = default;
    UInteger32TagInternal(UInteger32TagInternal&&) = default;
    virtual ~UInteger32TagInternal() = default;

    // -- Instance Variables
    uinteger32 value;

    // -- Operators
    bool operator==(const UInteger32TagInternal& other) const
    {
        if (!this->TagInternal::operator==(other)) {
            return false;
        }

        return this->value == other.value;
    }
    bool operator<(const UInteger32TagInternal& other) const
    {
        if (!this->TagInternal::operator<(other)) {
            return false;
        }

        return this->value < other.value;
    }

    // -- Instance Methods
    virtual void addTo(MutableBlob& destination) const override
    {
        count dataSize = 4;
        auto memoryRepresentation = MutableBlob::withCapacity(Internal::memoryNeededForTagHeader() + dataSize);

        auto tagAddress = memoryRepresentation.data();
        Internal::setIdentifierForTagAt(this->identifier, tagAddress);
        Internal::setDataSizeForTagAt(dataSize, tagAddress);
        Platform::writeBigEndianUInteger32ValueAt(this->value, Internal::dataForTagAt(tagAddress));

        destination.append(std::move(memoryRepresentation));
    }
};

} } }
