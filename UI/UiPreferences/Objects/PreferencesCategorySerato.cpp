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

#include <UiPreferences/PreferencesCategorySerato.hpp>
#include <UiModel/Preferences/PreferencesModuleManager.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include "ui_PreferencesSeratoPanel.h"
#include "UiPreferences/moc_PreferencesCategorySerato.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constants

#define NXA_OLD_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY           "ConvertMemoryCuesToHotCuesOnExport"

// -- Class Methods

void PreferencesCategorySerato::registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_IGNORE_PLAYLISTS_THAT_ARE_ALSO_FOLDERS_ON_IMPORT_PREFERENCES_KEY });
}

// -- Instance Methods

void PreferencesCategorySerato::p_handleConvertCuesFlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked), String{ NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
}

void PreferencesCategorySerato::p_handleIgnorePlaylistsChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked), String{ NXA_IGNORE_PLAYLISTS_THAT_ARE_ALSO_FOLDERS_ON_IMPORT_PREFERENCES_KEY });
}

// -- Overidden Instance Methods

PreferencesCategorySerato::~PreferencesCategorySerato()
{
    delete this->p_preferencesPanelContents;
}

NxA::String PreferencesCategorySerato::CategoryName() const
{
    return NxA::String(QObject::tr("Serato").toUtf8().constData());
}

void PreferencesCategorySerato::populatePreferencesPanel(QWidget* panel)
{
    this->p_preferencesPanelContents = new Ui::PreferencesSerato{};
    this->p_preferencesPanelContents->setupUi(panel);
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxConvertCues->setChecked(*maybeValue);
        }

        maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_IGNORE_PLAYLISTS_THAT_ARE_ALSO_FOLDERS_ON_IMPORT_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxIgnorePlaylists->setChecked(*maybeValue);
        }

        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxConvertCues, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleConvertCuesFlagChanged(newCheckState); });
        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxIgnorePlaylists, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleIgnorePlaylistsChanged(newCheckState); });
    }
}

boolean PreferencesCategorySerato::hasPreferencesToMigrate() const
{
    return this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY }).isValid();
}

void PreferencesCategorySerato::migratePreferenceValues() const
{
    auto maybeMemoryCuesFlag = this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
    if (maybeMemoryCuesFlag.isValid()) {
        this->p_userPreferences->setBooleanForKey(*maybeMemoryCuesFlag, String{ NXA_CONVERT_MEMORY_CUES_TO_SERATO_HOT_CUES_ON_EXPORT_PREFERENCES_KEY });
    }
}

boolean PreferencesCategorySerato::appClashesWithAppNamed(const String& name)
{
    return (name == "serato dj"_String) || (name == "serato dj pro"_String);
}
