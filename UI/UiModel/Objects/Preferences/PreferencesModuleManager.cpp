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

// -- Constants

#define NXA_DID_MIGRATE_PREFERENCES_PREFERENCES_KEY                    "DidMigratePreferences"

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

void PreferencesModuleManager::migrateUserPreferencesValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    auto maybeDidMigratePreferences = userPreferences->maybeBooleanForKey(String{ NXA_DID_MIGRATE_PREFERENCES_PREFERENCES_KEY });
    if (maybeDidMigratePreferences.isValid() && *maybeDidMigratePreferences) {
        return;
    }

#if defined(NXA_PLATFORM_MACOS)
    if (!UserPreferences::existsOnVolume(Volume::musicFolderVolume())) {
        // -- Preferences file doesn't exist, if there are preferences to migrate, import from NSUserPreferences
        boolean hasPreferencesToMigrate = false;

        for (auto&& module : preferenceModuleList) {
            if (module->hasPreferencesToMigrate()) {
                hasPreferencesToMigrate = true;
                break;
            }
        }

        if (hasPreferencesToMigrate && ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Attempt preference import from old version?"),
                                                                                        QObject::tr("No preferences exist yet. Do you want to import from the older version installed on this system?"),
                                                                                        QObject::tr("If you don't import, preferences will be set to their default values."))) {
            for (auto&& prefModule : preferenceModuleList) {
                prefModule->migratePreferenceValues();
            }

            userPreferences->setBooleanForKey(true, String{ NXA_DID_MIGRATE_PREFERENCES_PREFERENCES_KEY });
        }
    }
#endif
}

void PreferencesModuleManager::saveUserPreferencesChanges()
{
    auto& musicFolderVolume = Volume::musicFolderVolume();
    auto musicFolderVolumePreferences = RekordBuddy::UserPreferences::forVolume(musicFolderVolume);
    UserPreferences::saveToVolume(musicFolderVolumePreferences, musicFolderVolume);
}

}
