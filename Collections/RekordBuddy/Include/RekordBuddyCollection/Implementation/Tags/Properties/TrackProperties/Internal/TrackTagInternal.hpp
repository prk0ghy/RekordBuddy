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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTag.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/StringPropertyInternal.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

inline static void p_deleteTrackTagFromCollection(PersistentTrackTag& trackTag)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    Optional<TrackTag> maybeParentTagToDelete;

    if (trackTag.maybeParentTagObjectID().isValid()) {
        auto maybeParentTag = trackTag.maybeParentTag();
        NXA_ASSERT_TRUE(maybeParentTag.isValid());

        auto parentTag = TrackTag(std::move(*maybeParentTag));
        if (parentTag.childrenTags().length() == 1) {
            maybeParentTagToDelete = parentTag;
        }
    }

    auto childTagCopy = trackTag.childrenTags().objects();

    p_deleteStringPropertyFromCollection(trackTag);

    if (maybeParentTagToDelete.isValid()) {
        maybeParentTagToDelete->deleteFromCollection();
    }

    for (auto&& persistentChildTag : childTagCopy) {
        TrackTag(std::move(persistentChildTag)).deleteFromCollection();
    }
}

inline static String p_sortingValueForTrackTag(const PersistentTrackTag& trackTag)
{
    // -- The result of this method is stored in user data and should not be modified.
    auto components = trackTag.stringValue().splitBySeparator(':');
    NXA_ASSERT_TRUE(components.length() <= 2);

    // -- This should realy be all the components without the first one if we were to really have tags with more then 2 parts.
    return components.lastObject();
}

} } }
