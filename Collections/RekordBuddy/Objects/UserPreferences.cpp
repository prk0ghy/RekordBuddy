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

#include <RekordBuddyCollection/UserPreferences.hpp>
#include <RekordBuddyCollection/RekordBuddy.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

#include <Base/FilePath.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Local Functions

static FilePath p_applicationUserPreferencesFilePathOnVolume(const Volume& volume)
{
    auto applicationDataDirectoryPath = rekordBuddyFolderPathOnVolume(volume);
    return FilePath::filePathByJoiningPaths(applicationDataDirectoryPath, NXA_FILEPATH("userpref.db"));
}

// -- Class Methods

NotNull<NxA::UserPreferences*> RekordBuddy::UserPreferences::forVolume(const Volume& volume)
{
    return NxA::UserPreferences::fromFile(p_applicationUserPreferencesFilePathOnVolume(volume));
}

boolean RekordBuddy::UserPreferences::existsOnVolume(const Volume& volume)
{
    return File::existsAt(p_applicationUserPreferencesFilePathOnVolume(volume));
}

boolean RekordBuddy::UserPreferences::saveToVolume(NotNull<NxA::UserPreferences*> preferences, const Volume& volume)
{
    return preferences->saveToFile(p_applicationUserPreferencesFilePathOnVolume(volume));
}

void RekordBuddy::UserPreferences::registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    userPreferences->setDefaultIntegerForKey(int(Common::MusicalKey::Notation::Standard), String{ NXA_MUSICAL_NOTATION_PREFERENCES_KEY });
    userPreferences->setDefaultStringForKey(", "_String, String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    userPreferences->setDefaultStringForKey("/ "_String, String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
    userPreferences->setDefaultStringForKey("/"_String, String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
}
