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
#pragma once
#include <memory>
#include <RekordBuddyCollectionImplementationPersistence/V10/Artist/PersistentArtist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentAllTracksPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrateClone.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentFilterPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentFolder.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentOtherTracksPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentRootFolder.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSmartPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentSortingPlaylist.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentDecimalProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentTrackBpm.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/DecimalProperty/PersistentTrackLength.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentBooleanProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentIntegerProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackBeatGridLockedFlag.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackBitDepth.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackBitRate.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackColor.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackDiscNumber.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackFileSize.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackFileType.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackKey.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackNumber.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackPlayCount.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackRating.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackSampleRate.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/LogEntry/PersistentLogEntry.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentCueMarker.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentGridMarker.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentLoopMarker.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/MarkerImportOffset/PersistentMarkerImportOffset.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/SortingCriteria/PersistentSortingCriteria.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentDateProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackAlbum.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackComments.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackDateAdded.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackDateReleased.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackGenre.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackGrouping.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackMixName.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackRecordLabel.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackTag.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackTitle.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TimeProperty/PersistentTimeProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TimeProperty/PersistentTrackModificationTime.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/TrackDisplayCache/PersistentTrackDisplayCache.hpp>
template <typename F>
void NxA::RekordBuddy::CollectionImplementation::V10::RBSchema::applyToTypes(F callable)
{
    ALL_TYPES(CALL_ON_TYPE)
}
