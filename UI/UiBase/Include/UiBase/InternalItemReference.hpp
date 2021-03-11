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

#include <Base/String.hpp>

namespace NxA { namespace RekordBuddy {

class InternalItemReference final
{
    // -- Private Instance Variables
    const void* p_pointerToInternalItem;

public:
    // -- Constructors & Destructors
    InternalItemReference() = delete;
    InternalItemReference(const void* pointerToInternalItem) : p_pointerToInternalItem{ pointerToInternalItem } { }

    // -- Operators
    inline bool operator==(const InternalItemReference& other) const noexcept
    {
        return this->p_pointerToInternalItem == other.p_pointerToInternalItem;
    }
    inline bool operator!=(const InternalItemReference& other) const noexcept
    {
        return this->p_pointerToInternalItem != other.p_pointerToInternalItem;
    }
    inline bool operator<(const InternalItemReference& other) const noexcept
    {
        return this->p_pointerToInternalItem < other.p_pointerToInternalItem;
    }

    // -- Instance Methods
#if defined(DEBUG)
    String asString()
    {
        return String::stringWithFormat("%p", this->p_pointerToInternalItem);
    }
#endif
};

} }
