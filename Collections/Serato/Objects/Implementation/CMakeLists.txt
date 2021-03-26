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

nxa_project(SeratoCollectionImplementation)

add_library(SeratoCollectionImplementation STATIC
            Objects/Tags/BlobTag.cpp
            Objects/Tags/BooleanTag.cpp
            Objects/Tags/ObjectTag.cpp
            Objects/Tags/PathTag.cpp
            Objects/Tags/Tag.cpp
            Objects/Tags/TagFactory.cpp
            Objects/Tags/TextTag.cpp
            Objects/Tags/UInteger16Tag.cpp
            Objects/Tags/UInteger32Tag.cpp
            Objects/Tags/UnknownTag.cpp
            Objects/Tags/VersionTag.cpp
            Objects/Crate.cpp
            Objects/Database.cpp
            Objects/Track.cpp
            Objects/TrackEntry.cpp
            )

target_include_directories(SeratoCollectionImplementation
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Objects)
target_link_libraries(SeratoCollectionImplementation PUBLIC Base SeratoCollection TrackFilesCollection)

nxa_project_end()
