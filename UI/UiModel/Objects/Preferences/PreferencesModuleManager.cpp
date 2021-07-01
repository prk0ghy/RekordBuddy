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

#include <UiBase/Types.hpp>

#include <UiModel/Preferences/PreferencesModuleManager.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <Base/Base.hpp>

namespace NxA::RekordBuddy {

// -- Static Variables

using PreferenceList = std::vector<std::unique_ptr<PreferencesCategoryBase>>;
static PreferenceList preferenceModuleList;

// -- Interface

void PreferencesModuleManager::loadPreferencesModule(std::unique_ptr<PreferencesCategoryBase> module)
{
    preferenceModuleList.push_back(std::move(module));
}

count PreferencesModuleManager::numberAvailableModules()
{
    return *maybeNarrowCast<integer32>(preferenceModuleList.size());
}

PreferencesCategoryBase* PreferencesModuleManager::getModuleInterface(integer32 index)
{
    if (index >= 0 && index < *maybeNarrowCast<integer32>(preferenceModuleList.size())) {
        return preferenceModuleList[*maybeNarrowCast<std::size_t>(index)].get();
    }

    return nullptr;
}

void PreferencesModuleManager::saveUserPreferencesChanges()
{
    auto& musicFolderVolume = Volume::musicFolderVolume();
    auto musicFolderVolumePreferences = RekordBuddy::UserPreferences::forVolume(musicFolderVolume);
    UserPreferences::saveToVolume(musicFolderVolumePreferences, musicFolderVolume);
}

}
