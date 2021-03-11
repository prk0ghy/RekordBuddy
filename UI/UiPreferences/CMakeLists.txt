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

nxa_project(UiPreferences)

init_qt()
add_library(UiPreferences STATIC
    Objects/PreferencesCategoryGeneral.cpp
    Objects/PreferencesCategoryRekordbox.cpp
    Objects/PreferencesCategorySerato.cpp
    Objects/PreferencesCategoryTraktor.cpp
    Objects/PreferencesGeneralPanel.ui
    Objects/PreferencesRekordboxPanel.ui
    Objects/PreferencesSeratoPanel.ui
    Objects/PreferencesTraktorPanel.ui
    Objects/PreferencesWindow.cpp
    Objects/PreferencesWindow.ui
    )

target_include_directories(UiPreferences
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

target_link_libraries(UiPreferences PUBLIC UiModel)

nxa_project_end()
