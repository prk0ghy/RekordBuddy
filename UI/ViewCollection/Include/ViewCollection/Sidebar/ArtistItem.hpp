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

#include <CommonCollection/Artist.hpp>

#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>
#include <memory>

#pragma once

namespace NxA::RekordBuddy {

class AbstractTrackListItem;

class ArtistItem : public SidebarItem
{
    NotNull<Common::Artist*> p_artist;
    mutable Optional<Shared<AbstractTrackListNode>> p_treeCache;

public:
    ArtistItem(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&, NotNull<Common::Artist*>);

    QString getName() const override;
    std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree() override;
    boolean isSelectable() const override;
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::Artist;
    }

    Optional<QIcon> maybeIcon() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
};

}
