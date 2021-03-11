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

#include <ViewCollection/Sidebar/PlaylistsHeader.hpp>
#include <ViewCollection/Sidebar/FolderItem.hpp>

#include <CommonCollection/Collection.hpp>

class QIcon;

namespace NxA::RekordBuddy {

class SidebarItem;

}

using namespace NxA;
using namespace RekordBuddy;

PlaylistsHeader::PlaylistsHeader(NotNull<SidebarModel*> model,
                                 SidebarItem* parent,
                                 WorkQueue& withQueue,
                                 NotNull<const Common::Folder*> withRootFolder) : FolderItem{ model,
                                                                                              parent,
                                                                                              withQueue,
                                                                                              std::move(withRootFolder) }
{

}

QString PlaylistsHeader::getName() const
{
    return QObject::tr("PLAYLISTS");
}

boolean PlaylistsHeader::canBeDeleted() const
{
    return false;
}

boolean PlaylistsHeader::canBeCloned() const
{
    return false;
}

boolean PlaylistsHeader::canBeRenamed() const
{
    return false;
}

boolean PlaylistsHeader::canAddOrganization() const
{
    return false;
}

boolean PlaylistsHeader::isSelectable() const
{
    return false;
}

Optional<QIcon> PlaylistsHeader::maybeIcon() const
{
    return { };
}
