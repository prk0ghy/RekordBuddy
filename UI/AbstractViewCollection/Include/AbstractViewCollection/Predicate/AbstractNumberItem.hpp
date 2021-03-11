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

#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>

#pragma once

namespace NxA::RekordBuddy {

enum class NumberMatchMethod { Equal, NotEqual, LessThan, LessThanOrEqual, GreaterThan, GreaterThanOrEqual };

class AbstractNumberItem : public AbstractPredicateItem
{
    integer64 value;
    NumberMatchMethod method;

public:
    AbstractNumberItem(ChoiceData, integer64, NumberMatchMethod);

    inline integer64 getValue() const
    {
        return this->value;
    }
    inline NumberMatchMethod getMethod() const
    {
        return this->method;
    }
    inline void setMethod(NumberMatchMethod m)
    {
        this->method = m;
    }
    inline void setValue(integer64 d)
    {
        this->value = d;
    }
};
}
