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

nxa_project(Widgets)

init_qt()

add_library(Widgets STATIC
            Objects/FlowLayout.cpp
            Objects/TagChildWidget.cpp
            Objects/TrackListDetailsView.cpp
            Objects/ArtworkTrackViewDelegate.cpp
            Objects/DetailsTrackViewDelegate.cpp
            Objects/SidebarView.cpp
            Objects/PlayerWidget.cpp
            Objects/PlayerWidget.ui
            Objects/ConfirmationMessageBox.cpp
            Objects/ConfirmationMessageBox.ui
            Objects/ProgressWidget.cpp
            Objects/ProgressWidget.ui
            Objects/ColorEditor.ui
            Objects/ColorEditor.cpp
            Objects/RatingEditor.ui
            Objects/RatingEditor.cpp
            Objects/ClickableColor.cpp
            Objects/DurationViewer.cpp
            Objects/ImageBrowser.ui
            Objects/ImageBrowser.cpp
            Objects/TagWidget.cpp)

target_include_directories(Widgets
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects)
target_link_libraries(Widgets PUBLIC UiModel UiBase CommonCollection WorkQueue Qt5::Multimedia Qt5::Widgets ${EXTRA_QT_REQUIREMENTS})

nxa_project_end()
