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

set(SOURCE_FILES
    Objects/V10/V10Schema.cpp
    Objects/V10/Artist/PersistentArtist.cpp
    Objects/V10/Crate/PersistentAllTracksPlaylist.cpp
    Objects/V10/Crate/PersistentCrate.cpp
    Objects/V10/Crate/PersistentCrateClone.cpp
    Objects/V10/Crate/PersistentFilterPlaylist.cpp
    Objects/V10/Crate/PersistentFolder.cpp
    Objects/V10/Crate/PersistentOtherTracksPlaylist.cpp
    Objects/V10/Crate/PersistentPlaylist.cpp
    Objects/V10/Crate/PersistentRootFolder.cpp
    Objects/V10/Crate/PersistentSmartPlaylist.cpp
    Objects/V10/Crate/PersistentSortingPlaylist.cpp
    Objects/V10/DecimalProperty/PersistentDecimalProperty.cpp
    Objects/V10/DecimalProperty/PersistentTrackBpm.cpp
    Objects/V10/DecimalProperty/PersistentTrackLength.cpp
    Objects/V10/IntegerProperty/PersistentBooleanProperty.cpp
    Objects/V10/IntegerProperty/PersistentIntegerProperty.cpp
    Objects/V10/IntegerProperty/PersistentTrackBeatGridLockedFlag.cpp
    Objects/V10/IntegerProperty/PersistentTrackBitDepth.cpp
    Objects/V10/IntegerProperty/PersistentTrackBitRate.cpp
    Objects/V10/IntegerProperty/PersistentTrackColor.cpp
    Objects/V10/IntegerProperty/PersistentTrackDiscNumber.cpp
    Objects/V10/IntegerProperty/PersistentTrackFileSize.cpp
    Objects/V10/IntegerProperty/PersistentTrackFileType.cpp
    Objects/V10/IntegerProperty/PersistentTrackKey.cpp
    Objects/V10/IntegerProperty/PersistentTrackNumber.cpp
    Objects/V10/IntegerProperty/PersistentTrackPlayCount.cpp
    Objects/V10/IntegerProperty/PersistentTrackRating.cpp
    Objects/V10/IntegerProperty/PersistentTrackSampleRate.cpp
    Objects/V10/LogEntry/PersistentLogEntry.cpp
    Objects/V10/Marker/PersistentCueMarker.cpp
    Objects/V10/Marker/PersistentGridMarker.cpp
    Objects/V10/Marker/PersistentLoopMarker.cpp
    Objects/V10/Marker/PersistentMarker.cpp
    Objects/V10/MarkerImportOffset/PersistentMarkerImportOffset.cpp
    Objects/V10/SortingCriteria/PersistentSortingCriteria.cpp
    Objects/V10/StringProperty/PersistentDateProperty.cpp
    Objects/V10/StringProperty/PersistentStringProperty.cpp
    Objects/V10/StringProperty/PersistentTrackAlbum.cpp
    Objects/V10/StringProperty/PersistentTrackComments.cpp
    Objects/V10/StringProperty/PersistentTrackDateAdded.cpp
    Objects/V10/StringProperty/PersistentTrackDateReleased.cpp
    Objects/V10/StringProperty/PersistentTrackGenre.cpp
    Objects/V10/StringProperty/PersistentTrackGrouping.cpp
    Objects/V10/StringProperty/PersistentTrackMixName.cpp
    Objects/V10/StringProperty/PersistentTrackRecordLabel.cpp
    Objects/V10/StringProperty/PersistentTrackTag.cpp
    Objects/V10/StringProperty/PersistentTrackTitle.cpp
    Objects/V10/TimeProperty/PersistentTimeProperty.cpp
    Objects/V10/TimeProperty/PersistentTrackModificationTime.cpp
    Objects/V10/Track/PersistentTrack.cpp
    Objects/V10/TrackDisplayCache/PersistentTrackDisplayCache.cpp)

nxa_project(RKBImpPersistence)

add_library(RKBImpPersistence STATIC ${SOURCE_FILES})

target_include_directories(RKBImpPersistence PUBLIC Include Common)
target_link_libraries(RKBImpPersistence PUBLIC Base Persistence)

nxa_project_end()
