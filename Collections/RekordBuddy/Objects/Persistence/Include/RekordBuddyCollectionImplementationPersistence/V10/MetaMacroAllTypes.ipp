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
#undef ALL_TYPES
#define ALL_TYPES(F)     F(AllTracksPlaylist,Playlist) \
    F(Artist,PersistentObject) \
    F(BooleanProperty,IntegerProperty) \
    F(Crate,PersistentObject) \
    F(CrateClone,Crate) \
    F(CueMarker,Marker) \
    F(DateProperty,StringProperty) \
    F(DecimalProperty,PersistentObject) \
    F(FilterPlaylist,Playlist) \
    F(Folder,Crate) \
    F(GridMarker,Marker) \
    F(IntegerProperty,PersistentObject) \
    F(LogEntry,PersistentObject) \
    F(LoopMarker,CueMarker) \
    F(Marker,PersistentObject) \
    F(MarkerImportOffset,PersistentObject) \
    F(OtherTracksPlaylist,Playlist) \
    F(Playlist,Crate) \
    F(RootFolder,Folder) \
    F(SmartPlaylist,FilterPlaylist) \
    F(SortingCriteria,PersistentObject) \
    F(SortingPlaylist,FilterPlaylist) \
    F(StringProperty,PersistentObject) \
    F(TimeProperty,PersistentObject) \
    F(Track,PersistentObject) \
    F(TrackAlbum,StringProperty) \
    F(TrackBeatGridLockedFlag,BooleanProperty) \
    F(TrackBitDepth,IntegerProperty) \
    F(TrackBitRate,IntegerProperty) \
    F(TrackBpm,DecimalProperty) \
    F(TrackColor,IntegerProperty) \
    F(TrackComments,StringProperty) \
    F(TrackDateAdded,DateProperty) \
    F(TrackDateReleased,DateProperty) \
    F(TrackDiscNumber,IntegerProperty) \
    F(TrackDisplayCache,PersistentObject) \
    F(TrackFileSize,IntegerProperty) \
    F(TrackFileType,IntegerProperty) \
    F(TrackGenre,StringProperty) \
    F(TrackGrouping,StringProperty) \
    F(TrackKey,IntegerProperty) \
    F(TrackLength,DecimalProperty) \
    F(TrackMixName,StringProperty) \
    F(TrackModificationTime,TimeProperty) \
    F(TrackNumber,IntegerProperty) \
    F(TrackPlayCount,IntegerProperty) \
    F(TrackRating,IntegerProperty) \
    F(TrackRecordLabel,StringProperty) \
    F(TrackSampleRate,IntegerProperty) \
    F(TrackTag,StringProperty) \
    F(TrackTitle,StringProperty) 
#undef ALL_M2M_TYPES
#define ALL_M2M_TYPES(F)     F(Artist,artists,Track,tracksCreditedAsArtist) \
    F(Artist,producers,Track,tracksCreditedAsProducer) \
    F(Artist,remixers,Track,tracksCreditedAsRemixer) \
    F(DecimalProperty,decimalProperties,Track,parentTracks) \
    F(IntegerProperty,integerProperties,Track,parentTracks) \
    F(Playlist,parentPlaylists,Track,tracks) \
    F(StringProperty,stringProperties,Track,parentTracks) \
    F(TimeProperty,timeProperties,Track,parentTracks) 
