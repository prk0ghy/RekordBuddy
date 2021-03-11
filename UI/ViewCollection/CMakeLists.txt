#
#  Rekord Buddy - The future proof music collection tool made by DJs for DJs.
#  Copyright (C) 2020-2021 Didier Malenfant (didier@rekordbuddy.org)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.13)
nxa_project(ViewCollection)

init_qt()
add_library(ViewCollection STATIC
            Objects/Predicate/TagItem.cpp
            Objects/Predicate/StringItem.cpp
            Objects/Predicate/DateItem.cpp
            Objects/Predicate/EnumerationItem.cpp
            Objects/Predicate/GroupItem.cpp
            Objects/Predicate/NumberItem.cpp
            Objects/Sidebar/AllTracksHeader.cpp
            Objects/Sidebar/PlaylistsHeader.cpp
            Objects/Sidebar/TagsHeader.cpp
            Objects/Sidebar/KeysHeader.cpp
            Objects/Sidebar/ArtistsHeader.cpp
            Objects/Sidebar/ArtistItem.cpp
            Objects/Sidebar/KeyItem.cpp
            Objects/Sidebar/TrackTagItem.cpp
            Objects/Sidebar/AllTracksItem.cpp
            Objects/Sidebar/FolderItem.cpp
            Objects/Sidebar/PlaylistItem.cpp
            Objects/Sidebar/SmartPlaylistItem.cpp
            Objects/Sidebar/ViewCollection.cpp
            Objects/TrackList/TrackItem.cpp
            Objects/TrackList/TrackListNode.cpp
            )

target_include_directories(ViewCollection
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects)
target_link_libraries(ViewCollection PUBLIC Qt5::Widgets ${EXTRA_QT_REQUIREMENTS} Collections UiModel UiBase AbstractViewCollection RekordBuddyCollection WorkQueue)

nxa_project_end()
