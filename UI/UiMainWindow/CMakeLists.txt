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

nxa_project(UiMainWindow)

init_qt()

add_library(UiMainWindow STATIC
            Objects/AboutDialog.cpp
            Objects/AboutDialog.ui
            Objects/BetaSoftwareWarning.cpp
            Objects/BetaSoftwareWarning.ui
            Objects/DateMatchWidget.cpp
            Objects/EnumerationMatchWidget.cpp
            Objects/MainWindow.cpp
            Objects/MainWindow.ui
            Objects/MultiValueEditor.cpp
            Objects/MultiValueEditor.ui
            Objects/NewCollectionDialog.cpp
            Objects/NewCollectionDialog.ui
            Objects/NumberMatchWidget.cpp
            Objects/PredicateEditorDialog.cpp
            Objects/PredicateWidget.cpp
            Objects/RuleGroupWidget.cpp
            Objects/RuleWidget.cpp
            Objects/StringMatchWidget.cpp
            Objects/TagMatchWidget.cpp
            Objects/TrackEditDialog.cpp
            Objects/TrackEditDialog.ui
            Objects/WelcomeDialog.cpp
            Objects/WelcomeDialog.ui)

target_include_directories(UiMainWindow
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(UiMainWindow PUBLIC UiPreferences UiBase Collections MutableViewCollection UiUpdater Widgets Qt5::Multimedia Qt5::Widgets ${EXTRA_QT_REQUIREMENTS})

nxa_depends_on_build_defines(UiMainWindow RkbBuildDefines)

nxa_project_end()
