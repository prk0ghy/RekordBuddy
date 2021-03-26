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

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentDateProperty.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentDateProperty;

template class std::shared_ptr<PersistentDateProperty>;

template class NxA::Array<std::shared_ptr<PersistentDateProperty>>;
void PersistentDateProperty::bind(SourceBinder& parentBinder)
{
    PersistentStringProperty::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::DateProperty,
                      RBSchema::Type::StringProperty,
                      ATTRIBUTE(PersistentDateProperty::dayAttribute, day, Unindexed),
                      ATTRIBUTE(PersistentDateProperty::monthAttribute, month, Unindexed),
                      ATTRIBUTE(PersistentDateProperty::yearAttribute, year, Unindexed));
}
void PersistentDateProperty::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentStringProperty::faultObject();
}
void PersistentDateProperty::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentStringProperty::deleteObject();
}

// -- day
integer16 PersistentDateProperty::day() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return dayAttribute;
}
void PersistentDateProperty::setDay(integer16 day)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, dayAttribute, day);
}



// -- month
integer16 PersistentDateProperty::month() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return monthAttribute;
}
void PersistentDateProperty::setMonth(integer16 month)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, monthAttribute, month);
}



// -- year
integer16 PersistentDateProperty::year() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return yearAttribute;
}
void PersistentDateProperty::setYear(integer16 year)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, yearAttribute, year);
}


