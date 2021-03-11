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
#include <Base/Types.hpp>
#include <list>
#include <memory>
#include <type_traits>
#include <QString>
#include <QStringList>
#include <QList>

#pragma once

namespace NxA::RekordBuddy {

enum class GroupMatchMethod { Any, All, None };

class AbstractGroupItem : public AbstractPredicateItem
{
protected:
    std::list<std::unique_ptr<AbstractPredicateItem>> value;
    GroupMatchMethod method;

    virtual std::list<std::unique_ptr<AbstractPredicateItem>>&& getValues() = 0;

public:
    AbstractGroupItem(ChoiceData, std::list<std::unique_ptr<AbstractPredicateItem>>&&, GroupMatchMethod);

    NotNull<AbstractPredicateItem*> addPredicate(std::unique_ptr<AbstractPredicateItem> item);
    std::unique_ptr<AbstractPredicateItem> removeFirstPredicate();
    std::unique_ptr<AbstractPredicateItem> removePredicate(NotNull<AbstractPredicateItem*>);
    count predicateCount();
    NotNull<AbstractPredicateItem*> switchPredicate(NotNull<AbstractPredicateItem*> oldItem, const ChoiceData& newDescription);
    NotNull<AbstractPredicateItem*> getValue(integer64 index);

    virtual std::unique_ptr<AbstractPredicateItem> createDefaultElement(ChoiceData index) const = 0;
    virtual QList<ChoiceData> getPossibleRules() const = 0;

    inline GroupMatchMethod getMethod() const
    {
        return this->method;
    }
    inline void setMethod(GroupMatchMethod m)
    {
        this->method = m;
    }
};
}
