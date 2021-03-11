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

#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>
#include <Base/Utilities.hpp>

using namespace NxA;
using namespace RekordBuddy;

AbstractGroupItem::AbstractGroupItem(ChoiceData data, std::list<std::unique_ptr<AbstractPredicateItem>>&& withValue, GroupMatchMethod withMethod)
    : AbstractPredicateItem{ std::move(data) }, value{ std::move(withValue) }, method{ withMethod }
{
}

count AbstractGroupItem::predicateCount()
{
    return this->value.size();
}

NotNull<AbstractPredicateItem*> AbstractGroupItem::getValue(integer64 index)
{
    auto elementIter = this->value.begin();
    while (index-- > 0) {
        ++elementIter;
    }
    return elementIter->get();
}

std::list<std::unique_ptr<AbstractPredicateItem>>&& AbstractGroupItem::getValues()
{
    return std::move(this->value);
}

NotNull<AbstractPredicateItem*> AbstractGroupItem::addPredicate(std::unique_ptr<AbstractPredicateItem> item)
{
    auto pointer = item.get();
    this->value.push_back(std::move(item));
    return pointer;
}

std::unique_ptr<AbstractPredicateItem> AbstractGroupItem::removePredicate(NotNull<AbstractPredicateItem*> item)
{
    for (auto&& predicate : this->value) {
        if (predicate.get() == item.get()) {
            auto predicateToReturn = std::move(predicate);
            this->value.remove(predicateToReturn);
            return predicateToReturn;
        }
    }
    return nullptr;
}

NotNull<AbstractPredicateItem*> AbstractGroupItem::switchPredicate(NotNull<AbstractPredicateItem*> oldItem, const ChoiceData& newDescription)
{
    for (auto&& predicate : this->value) {
        if (predicate.get() == oldItem.get()) {
            predicate = this->createDefaultElement(newDescription);
            return predicate.get();
        }
    }
    return this->addPredicate(this->createDefaultElement(newDescription));
}

std::unique_ptr<AbstractPredicateItem> AbstractGroupItem::removeFirstPredicate()
{
    auto front = std::move(this->value.front());
    this->value.pop_front();
    return front;
}
