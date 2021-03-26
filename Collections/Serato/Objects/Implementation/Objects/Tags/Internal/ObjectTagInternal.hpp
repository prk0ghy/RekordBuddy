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

#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include "Tags/Internal/TagInternal.hpp"

#include <Base/Base.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                ObjectTag
#define NXA_OBJECT_INTERNAL_BASE_CLASS  TagInternal

// -- Internal Interface
class ObjectTagInternal : public NXA_OBJECT_INTERNAL_BASE_CLASS
{
    #include <Base/ObjectInternal.ipp>

public:
    // -- Constructors & Destructors
    ObjectTagInternal() = default;
    ObjectTagInternal(const ObjectTagInternal&) = default;
    ObjectTagInternal(ObjectTagInternal&&) = default;
    virtual ~ObjectTagInternal() = default;

    // -- Types
    using IdentifierToTagMap = MutableMap<uinteger32, Tag>;

    // -- Instance Variables
    IdentifierToTagMap subTagForIdentifier;

    // -- Operators
    bool operator==(const ObjectTagInternal& other) const noexcept
    {
        if (!this->TagInternal::operator==(other)) {
            return false;
        }

        return this->subTagForIdentifier == other.subTagForIdentifier;
    }
    bool operator<(const ObjectTagInternal& other) const noexcept
    {
        if (!this->TagInternal::operator<(other)) {
            return false;
        }

        return this->subTagForIdentifier < other.subTagForIdentifier;
    }

    // -- Instance Methods
    virtual void addTo(MutableBlob& destination) const override
    {
        MutableBlob subTagsRepresentation;
        for (auto&& identifierAndTag : this->subTagForIdentifier) {
            identifierAndTag.second.addTo(subTagsRepresentation);
        }

        auto sizeNeededForHeader = Internal::memoryNeededForTagHeader();
        auto headerRepresentation = MutableBlob::withCapacity(sizeNeededForHeader);
        auto tagAddress = headerRepresentation.data();
        Internal::setIdentifierForTagAt(this->identifier, tagAddress);
        Internal::setDataSizeForTagAt(subTagsRepresentation.size(), tagAddress);

        destination.append(std::move(headerRepresentation));
        destination.append(std::move(subTagsRepresentation));
    }
};

} } }
