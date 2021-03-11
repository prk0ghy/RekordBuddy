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

#include <Base/Base.hpp>
#include <qframe.h>
#include <type_traits>
#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>

namespace NxA::RekordBuddy {

class BaseRuleWidget : public QFrame
{
public:
    BaseRuleWidget(QWidget* parent) : QFrame{parent} {}
    ~BaseRuleWidget() override = default;
    virtual void saveToRules()
    {
        NXA_ALOG("Must implement in derived type"); // -- and can't be abstract
    }
};

template <typename T, typename>
class ItemRuleWidget; // -- purposefully not defined, error here means not convertable

template <typename T = AbstractPredicateItem, typename = std::enable_if_t<std::is_convertible<T*, AbstractPredicateItem*>::value>>
class ItemRuleWidget : public BaseRuleWidget
{
    using ItemType = T;

public:
    ItemRuleWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : BaseRuleWidget{parent}, item{*withItem.maybeAs<T*>()} {}

    inline void setItem(NotNull<T*> withItem)
    {
        this->item = withItem;
        this->loadFromCurrentItem();
    }
    inline NotNull<T*> getItem()
    {
        return this->item;
    }

    virtual void loadFromCurrentItem() = 0;

private:
    NotNull<T*> item;
};
}
