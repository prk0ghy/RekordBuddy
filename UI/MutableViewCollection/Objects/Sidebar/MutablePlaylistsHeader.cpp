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

#include <MutableViewCollection/Sidebar/MutablePlaylistsHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableFolderItem.hpp>

#include <CommonCollection/Collection.hpp>

class QIcon;

namespace NxA::RekordBuddy {

class SidebarItem;

}

using namespace NxA;
using namespace RekordBuddy;

MutablePlaylistsHeader::MutablePlaylistsHeader(NotNull<SidebarModel*> model,
                                               SidebarItem* parent,
                                               WorkQueue& withQueue,
                                               NotNull<Common::MutableFolder*> withRootFolder) : MutableFolderItem{ model,
                                                                                                                    parent,
                                                                                                                    withQueue,
                                                                                                                    std::move(withRootFolder) }
{

}

QString MutablePlaylistsHeader::getName() const
{
    return QObject::tr("PLAYLISTS");
}

boolean MutablePlaylistsHeader::canBeDeleted() const
{
    return false;
}

boolean MutablePlaylistsHeader::canBeCloned() const
{
    return false;
}

boolean MutablePlaylistsHeader::canBeRenamed() const
{
    return false;
}

boolean MutablePlaylistsHeader::canAddOrganization() const
{
    return false;
}

boolean MutablePlaylistsHeader::isSelectable() const
{
    return false;
}

Optional<QIcon> MutablePlaylistsHeader::maybeIcon() const
{
    return {};
}

boolean MutablePlaylistsHeader::canReceiveCrates() const
{
    return true;
}

boolean MutablePlaylistsHeader::canBeDragged() const
{
    return false;
}
