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

#include <UiPreferences/PreferencesCategoryTraktor.hpp>
#include <UiModel/Preferences/PreferencesModuleManager.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include "ui_PreferencesTraktorPanel.h"
#include "UiPreferences/moc_PreferencesCategoryTraktor.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constants

#define NXA_OLD_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY                       "UseTraktorKeyInsteadOfKeyText"
#define NXA_OLD_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY          "ImportExportComments2FieldAsGrouping"
#define NXA_OLD_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY                "MergeGridMarkersAndHotCuesForTraktor"

// -- Class Methods

void PreferencesCategoryTraktor::registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY });
    userPreferences->setDefaultBooleanForKey(true, String{ NXA_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY });
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY });
}

// -- Instance Methods

void PreferencesCategoryTraktor::p_handleUseTraktorKeyFlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked),
                                              String{ NXA_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY });
}

void PreferencesCategoryTraktor::p_handleImportComments2FlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked), String{ NXA_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY });
}

void PreferencesCategoryTraktor::p_handleMergeGridMarkersFlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked), String{ NXA_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY });
}

// -- Overidden Instance Methods

PreferencesCategoryTraktor::~PreferencesCategoryTraktor()
{
    delete this->p_preferencesPanelContents;
}

NxA::String PreferencesCategoryTraktor::CategoryName() const
{
    return NxA::String(QObject::tr("Traktor").toUtf8().constData());
}

void PreferencesCategoryTraktor::populatePreferencesPanel(QWidget* panel)
{
    this->p_preferencesPanelContents = new Ui::PreferencesTraktor{ };
    this->p_preferencesPanelContents->setupUi(panel);
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxUseTraktorKey->setChecked(*maybeValue);
        }
        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxUseTraktorKey, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleUseTraktorKeyFlagChanged(newCheckState); });
    }
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxImportComments2->setChecked(*maybeValue);
        }
        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxImportComments2, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleImportComments2FlagChanged(newCheckState); });
    }
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxMergeGridMarkers->setChecked(*maybeValue);
        }
        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxMergeGridMarkers, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleMergeGridMarkersFlagChanged(newCheckState); });
    }
}

boolean PreferencesCategoryTraktor::hasPreferencesToMigrate() const
{
    return this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY }).isValid();
}

void PreferencesCategoryTraktor::migratePreferenceValues() const
{
    auto maybeTraktorKeyInsteadOfKeyText = this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY });
    if (maybeTraktorKeyInsteadOfKeyText.isValid()) {
        this->p_userPreferences->setBooleanForKey(*maybeTraktorKeyInsteadOfKeyText,
                                           String{ NXA_READ_TRAKTOR_KEY_INSTEAD_OF_KEY_TEXT_PREFERENCES_KEY });
    }
    auto maybeMergeGridMarkers = this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY });
    if (maybeMergeGridMarkers.isValid()) {
        this->p_userPreferences->setBooleanForKey(*maybeMergeGridMarkers,
                                           String{ NXA_MERGE_GRID_MARKERS_AND_HOT_CUES_FOR_TRAKTOR_PREFERENCES_KEY });
    }
    auto maybeImportExportComments2 = this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY });
    if (maybeImportExportComments2.isValid()) {
        this->p_userPreferences->setBooleanForKey(*maybeImportExportComments2,
                                           String{ NXA_IMPORT_EXPORT_TRAKTOR_COMMENTS2_FIELD_AS_GROUPING_PREFERENCES_KEY });
    }
}

boolean PreferencesCategoryTraktor::appClashesWithAppNamed(const String& name)
{
    return name == "traktor"_String;
}
