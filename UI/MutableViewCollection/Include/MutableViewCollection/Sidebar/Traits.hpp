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

#include <MutableViewCollection/Sidebar/MutableFolderItem.hpp>
#include <MutableViewCollection/Sidebar/MutablePlaylistItem.hpp>
#include <MutableViewCollection/Sidebar/MutableSmartPlaylistItem.hpp>

#pragma once

namespace NxA { namespace Common {

class Folder;
class Playlist;
class SmartPlaylist;

}

namespace RekordBuddy {

template <typename T>
    struct CrateTrait;

template <>
    struct CrateTrait<NotNull<Common::MutableFolder*>>
    {
        using MutableType = MutableFolderItem;
    };

template <>
    struct CrateTrait<NotNull<Common::MutablePlaylist*>>
    {
        using MutableType = MutablePlaylistItem;
    };

template <>
    struct CrateTrait<NotNull<Common::MutableSmartPlaylist*>>
    {
        using MutableType = MutableSmartPlaylistItem;
    };

template <typename T>
    struct CrateTrait<const T&>
    {
        using MutableType = typename CrateTrait<T>::MutableType;
    };

template <typename T>
    struct CrateTrait<T&>
    {
        using MutableType = typename CrateTrait<T>::MutableType;
    };

template <typename T>
    struct CrateTrait<T&&>
    {
        using MutableType = typename CrateTrait<T>::MutableType;
    };

template <typename Parent>
    struct CrateVisitor
    {
        Parent* p_parent;
        WorkQueue& p_queue;
        NotNull<SidebarModel*> p_model;

        template <typename T>
            Shared<AbstractCrateItem> operator()(T&& crate)
            {
                return Shared<typename CrateTrait<decltype(crate)>::MutableType>::with(this->p_model, this->p_parent, this->p_queue, std::move(crate));
            }
    };

} }
