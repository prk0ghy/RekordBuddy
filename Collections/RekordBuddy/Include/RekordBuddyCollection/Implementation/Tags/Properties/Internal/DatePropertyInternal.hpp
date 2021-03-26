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

#include <RekordBuddyCollection/Implementation/Tags/Properties/DateProperty.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/StringPropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

namespace NxA {

// -- Operator Specialization
    
inline constexpr bool operator==(const RekordBuddy::CollectionImplementation::PersistentDateProperty& first,
                                 const RekordBuddy::CollectionImplementation::PersistentDateProperty& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const RekordBuddy::CollectionImplementation::PersistentDateProperty& first,
                                const RekordBuddy::CollectionImplementation::PersistentDateProperty& second) noexcept
{
    return first.objectID() < second.objectID();
}

namespace RekordBuddy { namespace CollectionImplementation {

// -- Private Functions

template<typename T>
Optional<std::shared_ptr<T>> p_maybeDatePropertyWithValueInCollection(const String& value, Collection& collection)
{
    auto date = Date::maybeDateWithStringUsingSeparator(value, '-');
    if (!date.isValid()) {
        return nothing;
    }

    auto newTag = StringPropertyInternal::stringPropertyWithValueInCollection<T>(date->asStringSeparatedWith('-'), collection);

    auto maybeDay = date->maybeDay();
    newTag->setDay(maybeDay.isValid() ? *maybeDay : 0);

    auto maybeMonth = date->maybeMonth();
    newTag->setMonth(maybeMonth.isValid() ? *maybeMonth : 0);

    newTag->setYear(date->year());

    return newTag;
}

inline Date p_dateFromPersistentDateProperty(const PersistentDateProperty& property)
{
    auto month = property.month();
    auto day = property.day();

    Optional<Date> maybeDate;
    if (!month || !day) {
        maybeDate = Date::maybeDateWithYear(property.year());
    }
    else {
        maybeDate = Date::maybeDateWithYearMonthDay(property.year(), month, day);
    }

    NXA_ASSERT_TRUE(maybeDate.isValid());
    return *maybeDate;
}

inline void p_deleteDatePropertyFromCollection(PersistentDateProperty& property)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    p_deleteStringPropertyFromCollection(property);
}

} } }
