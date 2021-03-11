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
nxa_project(MutableViewCollection)

init_qt()
add_library(MutableViewCollection STATIC
            Objects/Predicate/MutableDateItem.cpp
            Objects/Predicate/MutableEnumerationItem.cpp
            Objects/Predicate/MutableGroupItem.cpp
            Objects/Predicate/MutableNumberItem.cpp
            Objects/Predicate/MutableStringItem.cpp
            Objects/Predicate/MutableTagItem.cpp
            Objects/Sidebar/MutableCollectionList.cpp
            Objects/Sidebar/MutableAllTracksHeader.cpp
            Objects/Sidebar/MutableAllTracksItem.cpp
            Objects/Sidebar/MutableArtistItem.cpp
            Objects/Sidebar/MutableArtistsHeader.cpp
            Objects/Sidebar/MutableFolderItem.cpp
            Objects/Sidebar/MutableKeyItem.cpp
            Objects/Sidebar/MutableKeysHeader.cpp
            Objects/Sidebar/MutablePlaylistItem.cpp
            Objects/Sidebar/MutableSmartPlaylistItem.cpp
            Objects/Sidebar/MutablePlaylistsHeader.cpp
            Objects/Sidebar/MutableTagsHeader.cpp
            Objects/Sidebar/MutableTrackTagItem.cpp
            Objects/Sidebar/MutableViewCollection.cpp
            Objects/TrackList/MutableTrackItem.cpp
            Objects/TrackList/MutableTrackListNode.cpp
            )

target_include_directories(MutableViewCollection
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects)
target_link_libraries(MutableViewCollection PUBLIC Qt5::Widgets ${EXTRA_QT_REQUIREMENTS} Collections UiModel UiBase AbstractViewCollection RekordBuddyCollection ViewCollection WorkQueue)

nxa_project_end()
