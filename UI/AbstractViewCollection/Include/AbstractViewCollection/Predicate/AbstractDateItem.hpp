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
#include <QDate>

#pragma once

namespace NxA::RekordBuddy {

enum class DateMatchMethod { Is, IsNot, Before, After };

class AbstractDateItem : public AbstractPredicateItem
{
    QDate date;
    DateMatchMethod method;

public:
    AbstractDateItem(ChoiceData, QDate, DateMatchMethod);

    inline QDate getValue() const
    {
        return this->date;
    }
    inline void setValue(QDate d)
    {
        this->date = d;
    }
    inline DateMatchMethod getMethod() const
    {
        return this->method;
    }
    inline void setMethod(DateMatchMethod m)
    {
        this->method = m;
    }
};
}
