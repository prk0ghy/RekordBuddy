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

nxa_project(AbstractViewCollection)

init_qt()

# --Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

add_library(AbstractViewCollection STATIC
    Objects/Predicate/AbstractPredicateItem.cpp
    Objects/Predicate/AbstractDateItem.cpp
    Objects/Predicate/AbstractEnumerationItem.cpp
    Objects/Predicate/AbstractGroupItem.cpp
    Objects/Predicate/AbstractNumberItem.cpp
    Objects/Predicate/AbstractStringItem.cpp
    Objects/Predicate/AbstractTagItem.cpp
    Objects/Sidebar/SidebarItem.cpp
    Objects/Sidebar/AbstractCrateItem.cpp
    Objects/Sidebar/AbstractCollectionList.cpp
    Objects/Sidebar/AbstractCollection.cpp
    Objects/TrackList/AbstractTrackItem.cpp
)

target_include_directories(AbstractViewCollection
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects)
target_link_libraries(AbstractViewCollection PUBLIC Qt5::Widgets ${EXTRA_QT_REQUIREMENTS} Base UiBase UiModel WorkQueue UiMainWindow)

nxa_project_end()
