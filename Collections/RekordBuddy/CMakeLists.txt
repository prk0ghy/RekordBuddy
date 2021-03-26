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

if(${NXA_CMAKE_PLATFORM} STREQUAL "macOS" OR ${NXA_CMAKE_PLATFORM} STREQUAL "Windows")
    set(SOURCE_FILES
        Objects/Implementation/Crates/Internal/CrateCloneInternal.cpp
        Objects/Implementation/Crates/Internal/CrateInternal.cpp
        Objects/Implementation/Crates/Internal/FilterPlaylist.cpp
        Objects/Implementation/Crates/Internal/OtherTracksPlaylist.cpp
        Objects/Implementation/Crates/Internal/Playlist.cpp
        Objects/Implementation/Crates/Internal/SmartPlaylist.cpp
        Objects/Implementation/Crates/Internal/SortingPlaylist.cpp
        Objects/Implementation/Crates/AllTracksPlaylist.cpp
        Objects/Implementation/Crates/Crate.cpp
        Objects/Implementation/Crates/CrateClone.cpp
        Objects/Implementation/Crates/FilterPlaylist.cpp
        Objects/Implementation/Crates/Folder.cpp
        Objects/Implementation/Crates/OtherTracksPlaylist.cpp
        Objects/Implementation/Crates/Playlist.cpp
        Objects/Implementation/Crates/RootFolder.cpp
        Objects/Implementation/Crates/SmartPlaylist.cpp
        Objects/Implementation/Crates/SortingCriteria.cpp
        Objects/Implementation/Crates/SortingPlaylist.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackAlbum.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackBeatGridLockedFlag.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackBitDepth.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackBitRate.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackBpm.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackColor.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackComments.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackDateAdded.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackDateReleased.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackFileSize.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/AudioFileType.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackGenre.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackGrouping.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackKey.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackLength.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackMixName.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackModificationTime.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackNumber.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackDiscNumber.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackPlayCount.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackRating.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackRecordLabel.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackSampleRate.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackTag.cpp
        Objects/Implementation/Tags/Properties/TrackProperties/TrackTitle.cpp
        Objects/Implementation/Tags/Properties/BooleanProperty.cpp
        Objects/Implementation/Tags/Properties/DateProperty.cpp
        Objects/Implementation/Tags/Properties/DecimalProperty.cpp
        Objects/Implementation/Tags/Properties/IntegerProperty.cpp
        Objects/Implementation/Tags/Properties/StringProperty.cpp
        Objects/Implementation/Tags/Properties/TimeProperty.cpp
        Objects/Implementation/Tags/Tag.cpp
        Objects/Implementation/Tracks/Property.cpp
        Objects/Implementation/Tracks/Track.cpp
        Objects/Implementation/Tracks/TrackDisplayCache.cpp
        Objects/Implementation/Tracks/Track${NXA_CMAKE_PLATFORM_PATH_POSTFIX}
        Objects/Implementation/Artist.cpp
        Objects/Implementation/Collection.cpp
        Objects/Crates/FilterPlaylist.cpp
        Objects/Crates/Folder.cpp
        Objects/Crates/FolderClone.cpp
        Objects/Crates/OtherTracksPlaylist.cpp
        Objects/Crates/Playlist.cpp
        Objects/Crates/PlaylistClone.cpp
        Objects/Crates/SmartPlaylist.cpp
        Objects/Crates/SmartPlaylistClone.cpp
        Objects/Crates/SortingPlaylist.cpp
        Objects/Tracks/Tag.cpp
        Objects/Tracks/Track.cpp
        Objects/Collection.cpp
        Objects/RekordBuddy.cpp
        Objects/UserPreferences.cpp)

    # -- This project is the regular library.
    nxa_project(RekordBuddyCollection)

    add_library(RekordBuddyCollection STATIC
                ${SOURCE_FILES})

    target_include_directories(RekordBuddyCollection PUBLIC Include)
    target_link_libraries(RekordBuddyCollection PUBLIC Base RKBImpPersistence CommonCollection)

    nxa_depends_on_build_defines(RekordBuddyCollection RkbBuildDefines)

    add_subdirectory(Objects/Persistence)

    if(NXA_CMAKE_BUILD_FOR_TESTING)
        add_subdirectory(Tests)
    endif()

    nxa_project_end()
endif()
