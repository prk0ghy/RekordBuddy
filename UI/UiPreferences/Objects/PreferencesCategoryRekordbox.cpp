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

#include <UiPreferences/PreferencesCategoryRekordbox.hpp>
#include <UiModel/Preferences/PreferencesModuleManager.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include "ui_PreferencesRekordboxPanel.h"
#include "UiPreferences/moc_PreferencesCategoryRekordbox.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constants

#define NXA_OLD_NUMBER_OF_REKORDBOX_HOTCUES_PREFERENCES_KEY                            "NxAUserPreferencesNumberOfRekordboxHotcues"
#define NXA_OLD_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY  "NxAUserPreferencesNumberOfRekordboxGridMarkersBeforeIgnoringThem"
#define NXA_OLD_DUPLICATE_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY                       "NxAUserPreferencesDuplicateSeratoHotCuesAsMemoryCues"

// -- Class Methods

void PreferencesCategoryRekordbox::registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    userPreferences->setDefaultIntegerForKey(8, String{ NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY });
    userPreferences->setDefaultIntegerForKey(4096, String{ NXA_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY });
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_DUPLICATE_REKORDBOX_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
    userPreferences->setDefaultBooleanForKey(false, String{ NXA_ALLOW_REKORDBOX_VIDEO_TRACKS_IN_PLAYLISTS_PREFERENCES_KEY });
}

// -- Instance Methods

PreferencesCategoryRekordbox::~PreferencesCategoryRekordbox()
{
    delete this->p_preferencesPanelContents;
}

void PreferencesCategoryRekordbox::p_handleNumberHotcuesChanged(int newIndex)
{
    QVariant data = this->p_preferencesPanelContents->comboBoxNumberOfHotcues->itemData(newIndex, Qt::UserRole);
    if (data.isValid()) {
        this->p_userPreferences->setIntegerForKey(data.toInt(), String{ NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY });
        // --TODO: Does this need to be ported over: [NxACueMarker setAllMarkersWithHotCuesInContext:appDelegate.uiPersistentContext asModifiedOn:[NxATime now]];
    }
}

void PreferencesCategoryRekordbox::p_handleGridMarkersIgnoredChanged(int newMarkersIgnored)
{
    this->p_userPreferences->setIntegerForKey(this->p_preferencesPanelContents->spinBoxGridMarkers->value(),
                                              String{ NXA_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY });
}

void PreferencesCategoryRekordbox::p_handleDuplicateHotcuesFlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked),
                                              String{ NXA_DUPLICATE_REKORDBOX_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
}

void PreferencesCategoryRekordbox::p_handleAllowVideoTracksInPlaylistsFlagChanged(int newCheckState)
{
    this->p_userPreferences->setBooleanForKey((newCheckState == Qt::Checked),
                                              String{ NXA_ALLOW_REKORDBOX_VIDEO_TRACKS_IN_PLAYLISTS_PREFERENCES_KEY });
}

// -- Overidden Instance Methods

NxA::String PreferencesCategoryRekordbox::CategoryName() const
{
    return NxA::String(QObject::tr("rekordbox").toUtf8().constData());
}

void PreferencesCategoryRekordbox::populatePreferencesPanel(QWidget* panel)
{
    this->p_preferencesPanelContents = new Ui::PreferencesRekordbox{};
    this->p_preferencesPanelContents->setupUi(panel);
    void (QComboBox::*comboIndexChangedEvent)(int) = &QComboBox::currentIndexChanged;
    void (QSpinBox::*spinValueChangedEvent)(int) = &QSpinBox::valueChanged;
    {
        this->p_preferencesPanelContents->comboBoxNumberOfHotcues->addItem(QObject::tr("No hotcues"), 0);
        this->p_preferencesPanelContents->comboBoxNumberOfHotcues->addItem(QObject::tr("3 hotcues"), 3);
        this->p_preferencesPanelContents->comboBoxNumberOfHotcues->addItem(QObject::tr("4 hotcues"), 4);
        this->p_preferencesPanelContents->comboBoxNumberOfHotcues->addItem(QObject::tr("8 hotcues"), 8);
        auto maybeValue = this->p_userPreferences->maybeIntegerForKey(String{ NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            integer32 valueIndex = this->p_preferencesPanelContents->comboBoxNumberOfHotcues->findData(*maybeValue);
            if (valueIndex != -1) {
                this->p_preferencesPanelContents->comboBoxNumberOfHotcues->setCurrentIndex(valueIndex);
            }
        }
        QComboBox::connect(this->p_preferencesPanelContents->comboBoxNumberOfHotcues, comboIndexChangedEvent,
                           [=](int newIndex) {
                               this->p_handleNumberHotcuesChanged(newIndex); });
    }
    {
        auto maybeValue = this->p_userPreferences->maybeIntegerForKey(String{ NXA_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY });
        this->p_preferencesPanelContents->spinBoxGridMarkers->setMaximum(999999);
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->spinBoxGridMarkers->setValue(*maybeValue);
        }
        QSpinBox::connect(this->p_preferencesPanelContents->spinBoxGridMarkers, spinValueChangedEvent,
                          [=](int newValue) {
                              this->p_handleGridMarkersIgnoredChanged(newValue); });
    }
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_DUPLICATE_REKORDBOX_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->checkBoxDuplicateHotcues->setChecked(*maybeValue);
        }
        QCheckBox::connect(this->p_preferencesPanelContents->checkBoxDuplicateHotcues, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleDuplicateHotcuesFlagChanged(newCheckState); });
    }
    {
        auto maybeValue = this->p_userPreferences->maybeBooleanForKey(String{ NXA_ALLOW_REKORDBOX_VIDEO_TRACKS_IN_PLAYLISTS_PREFERENCES_KEY });
        if (maybeValue.isValid()) {
            this->p_preferencesPanelContents->allowVideoTracksInPlaylists->setChecked(*maybeValue);
        }
        QCheckBox::connect(this->p_preferencesPanelContents->allowVideoTracksInPlaylists, &QCheckBox::stateChanged,
                           [=](int newCheckState) {
                               this->p_handleAllowVideoTracksInPlaylistsFlagChanged(newCheckState); });
    }
}

boolean PreferencesCategoryRekordbox::hasPreferencesToMigrate() const
{
    return this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_NUMBER_OF_REKORDBOX_HOTCUES_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_DUPLICATE_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
}

void PreferencesCategoryRekordbox::migratePreferenceValues() const
{
    auto maybeNumberOfRekordboxHotcues = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_NUMBER_OF_REKORDBOX_HOTCUES_PREFERENCES_KEY });
    if (maybeNumberOfRekordboxHotcues.isValid()) {
        this->p_userPreferences->setIntegerForKey(*maybeNumberOfRekordboxHotcues,
                                                  String{ NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY });
    }
    auto maybeNumberOfRekordboxGridMarkersBeforeIgnoringThem = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY });
    if (maybeNumberOfRekordboxGridMarkersBeforeIgnoringThem.isValid()) {
        this->p_userPreferences->setIntegerForKey(*maybeNumberOfRekordboxGridMarkersBeforeIgnoringThem,
                                           String{ NXA_NUMBER_OF_REKORDBOX_GRID_MARKERS_BEFORE_IGNORING_THEM_PREFERENCES_KEY });
    }
    auto maybeDuplicateHotCuesAsMemoryCues = this->p_userPreferences->maybeOSBooleanForKey(String{ NXA_OLD_DUPLICATE_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
    if (maybeDuplicateHotCuesAsMemoryCues.isValid()) {
        this->p_userPreferences->setBooleanForKey(*maybeDuplicateHotCuesAsMemoryCues,
                                           String{ NXA_DUPLICATE_REKORDBOX_HOTCUES_AS_MEMORY_CUES_PREFERENCES_KEY });
    }
}

boolean PreferencesCategoryRekordbox::appClashesWithAppNamed(const String& name)
{
    // -- We don't really interface directly wiht rekordbox at this time, even the XML
    // -- file we write to needs to be refreshed from within rekordbox so this is safe.
    return false;
}
