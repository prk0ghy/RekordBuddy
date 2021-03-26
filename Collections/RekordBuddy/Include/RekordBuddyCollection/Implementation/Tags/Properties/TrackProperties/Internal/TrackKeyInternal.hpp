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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackKey.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/IntegerPropertyInternal.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

inline static String p_sortingValueForTrackKey(const PersistentTrackKey& trackKey)
{
    Common::MusicalKey::Notation notation = Common::MusicalKey::defaultNotation();
    if (notation == Common::MusicalKey::Notation::Camelot) {
        notation = Common::MusicalKey::Notation::CamelotLeadingZero;
    }
    else if (notation == Common::MusicalKey::Notation::OpenKey) {
        notation = Common::MusicalKey::Notation::OpenKeyLeadingZero;
    }

    return Common::MusicalKey::stringValueForKeyUsingNotation(static_cast<Common::MusicalKey::Value>(trackKey.integerValue()), notation);
}

inline static String p_stringValueForTrackKey(const PersistentTrackKey& trackKey)
{
    return Common::MusicalKey::stringValueForKey(static_cast<Common::MusicalKey::Value>(trackKey.integerValue()));
}

} } }
