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

#include <ViewCollection/Sidebar/FolderItem.hpp>
#include <ViewCollection/Sidebar/PlaylistItem.hpp>
#include <ViewCollection/Sidebar/SmartPlaylistItem.hpp>

#pragma once

namespace NxA::Common {

class Folder;
class Playlist;
class SmartPlaylist;
class MutableFolder;
class MutablePlaylist;
class MutableSmartPlaylist;

}

namespace NxA::RekordBuddy {

template <typename T>
    struct CrateTrait;

template <>
    struct CrateTrait<Common::Folder>
    {
        using Type = FolderItem;
    };

template <>
    struct CrateTrait<Common::Playlist>
    {
        using Type = PlaylistItem;
    };

template <>
    struct CrateTrait<Common::SmartPlaylist>
    {
        using Type = SmartPlaylistItem;
    };

template <>
    struct CrateTrait<Common::MutableFolder>
    {
        using Type = FolderItem;
    };

template <>
    struct CrateTrait<Common::MutablePlaylist>
    {
        using Type = PlaylistItem;
    };

template <>
    struct CrateTrait<Common::MutableSmartPlaylist>
    {
        using Type = SmartPlaylistItem;
    };

template <typename T>
    struct CrateTrait<NotNull<T*>>
    {
        using Type = typename CrateTrait<T>::Type;
    };

template <typename T>
    struct CrateTrait<NotNull<const T*>>
    {
        using Type = typename CrateTrait<T>::Type;
    };

template <typename T>
    struct CrateTrait<const T&>
    {
        using Type = typename CrateTrait<T>::Type;
    };

template <typename T>
    struct CrateTrait<T&>
    {
        using Type = typename CrateTrait<T>::Type;
    };

template <typename T>
    struct CrateTrait<T&&>
    {
        using Type = typename CrateTrait<T>::Type;
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
                return Shared<typename CrateTrait<decltype(crate)>::Type>::with(this->p_model, this->p_parent, this->p_queue, std::move(crate));
            }
    };

}
