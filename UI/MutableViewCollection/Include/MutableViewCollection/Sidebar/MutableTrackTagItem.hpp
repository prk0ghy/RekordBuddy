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

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Tracks/Tag.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>
#include <memory>

#pragma once

namespace NxA::RekordBuddy {

class AbstractTrackListItem;

class MutableTrackTagItem : public SidebarItem
{
    NotNull<Common::MutableTag*> p_tag;

    mutable Optional<Array<Unique<Common::MutableTag>>> p_maybeChildren;
    mutable Optional<Shared<AbstractTrackListNode>> p_maybeTreeCache;

protected:
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    MutableTrackTagItem(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&, NotNull<Common::MutableTag*>);

    QString getName() const override;
    std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree() override;
    boolean isSelectable() const override;
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::Tag;
    }

    Optional<QIcon> maybeIcon() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;

    count childCount() const override;
};
}
