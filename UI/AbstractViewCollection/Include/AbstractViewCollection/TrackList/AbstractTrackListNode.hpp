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

#include <UiBase/Types.hpp>

#include <Base/Array.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>
#include <Base/Variant.hpp>

#include <QAbstractListModel>

#include <memory>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class AbstractTrackItem;
class AbstractCrateItem;

// -- Public Interface
class AbstractTrackListNode
{
public:
    // -- Constructors & Destructors
    virtual ~AbstractTrackListNode() = default;

    // -- Instance Methods
    virtual Optional<NotNull<const AbstractTrackListNode*>> maybeChildNode(count) const = 0;
    virtual Optional<NotNull<AbstractTrackListNode*>> maybeChildNode(count) = 0;
    virtual Optional<NotNull<const AbstractTrackListNode*>> maybeParent() const = 0;
    virtual Optional<NotNull<AbstractTrackListNode*>> maybeParent() = 0;
    virtual boolean hasParentItem() const = 0;
    virtual count childCount() const = 0;
    virtual Pointer<const AbstractTrackListNode> getChildAtIndex(count) const
    {
        NXA_ALOG("This item has no children.");
    }
    virtual Pointer<AbstractTrackListNode> getChildAtIndex(count)
    {
        NXA_ALOG("This item has no children.");
    }
    virtual Optional<NotNull<const AbstractCrateItem*>> maybeCrateItem() const = 0;
    virtual Optional<NotNull<AbstractCrateItem*>> maybeCrateItem() = 0;
    virtual Optional<NotNull<const AbstractTrackItem*>> maybeTrackItem() const = 0;
    virtual Optional<NotNull<AbstractTrackItem*>> maybeTrackItem() = 0;
    virtual count getRowInParent() const = 0;
    virtual QString getSectionName() const = 0;
    virtual void refreshTrackList() = 0;
    virtual Optional<count> getTrackIndex(NotNull<AbstractTrackItem*>) const = 0;
    virtual boolean canUserOrderTracks() const
    {
        return false;
    }
    virtual void moveTracksAtIndicesToIndex(Array<count>, count)
    {
        NXA_ALOG("Can't reorder this.");
    }
};

} }
