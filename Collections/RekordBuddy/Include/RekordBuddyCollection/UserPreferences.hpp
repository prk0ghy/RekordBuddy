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

#include <Base/NotNull.hpp>
#include <Base/UserPreferences.hpp>
#include <Base/Volume.hpp>

// -- Interface

namespace NxA { namespace RekordBuddy {

// -- Constants
#define NXA_MUSICAL_NOTATION_PREFERENCES_KEY                                           "MusicalNotation"
#define NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY                                          "ArtistsSeparator"
#define NXA_GENRES_SEPARATOR_PREFERENCES_KEY                                           "GenresSeparator"
#define NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY                                     "KeysSeparator"

#define NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY          "MaximumNumberOfRekordboxHotcuesToExport"
#define NXA_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY      "MaximumNumberOfRekordboxGridMarkersToImport"
#define NXA_DUPLICATE_REKORDBOX_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY                 "DuplicateRekordoxHotCuesAsMemoryCuesOnExport"
#define NXA_ALLOW_REKORDBOX_VIDEO_TRACKS_IN_PLAYLISTS_PREFERENCES_KEY                  "AllowRekordoxVideoTracksInPlaylists"

#define NXA_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY                       "UseTraktorKeyInsteadOfKeyTextOnImport"
#define NXA_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY          "UseComments2FieldAsGroupingOnImportExport"
#define NXA_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY                "MergeGridMarkersAndHotCuesForTraktorOnExport"

#define NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY           "ConvertMemoryCuesToHotCuesOnExport"
#define NXA_IGNORE_PLAYLISTS_THAT_ARE_ALSO_FOLDERS_ON_IMPORT_PREFERENCES_KEY           "IgnorePLaylistsThatAreAlsoFoldersOnExport"

#define NXA_USER_RANDOM_UNIQUE_ID_PREFERENCES_KEY                                      "UserRandomUniqueID"

// -- Public Interface
class UserPreferences
{
public:
    // -- Class Methods
    static NotNull<NxA::UserPreferences*> forVolume(const Volume&);
    static boolean existsOnVolume(const Volume&);
    static boolean saveToVolume(NotNull<NxA::UserPreferences*>, const Volume&);

    static void registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*>);
};

} }
