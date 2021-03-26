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

#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>
#include "Tags/Internal/TextTagInternal.hpp"

#include <Base/Base.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                VersionTag
#define NXA_OBJECT_INTERNAL_BASE_CLASS  TagInternal

// -- Internal Interface
class VersionTagInternal : public TextTagInternal
{
    #include <Base/ObjectInternal.ipp>

public:
    // -- Constructors & Destructors
    VersionTagInternal() = default;
    VersionTagInternal(const VersionTagInternal&) = default;
    VersionTagInternal(VersionTagInternal&&) = default;
    virtual ~VersionTagInternal() = default;

    // -- Operators
    bool operator==(const VersionTagInternal& other) const
    {
        return this->TextTagInternal::operator==(other);
    }
    bool operator<(const VersionTagInternal& other) const
    {
        return this->TextTagInternal::operator<(other);
    }
};

} } }
