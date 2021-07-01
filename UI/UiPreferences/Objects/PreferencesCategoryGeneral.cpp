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

#include <UiPreferences/PreferencesCategoryGeneral.hpp>
#include <UiModel/Preferences/PreferencesModuleManager.hpp>
#include <UiBase/Types.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

#include "ui_PreferencesGeneralPanel.h"
#include "UiPreferences/moc_PreferencesCategoryGeneral.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constants and Types

#define NXA_OLD_MUSICAL_NOTATION_PREFERENCES_KEY            "NxAUserPreferencesMusicalNotation"
#define NXA_OLD_ARTISTS_SEPARATOR_PREFERENCES_KEY           "NxAUserPreferencesArtistsSeparator"
#define NXA_OLD_GENRES_SEPARATOR_PREFERENCES_KEY            "NxAUserPreferencesGenresSeparator"
#define NXA_OLD_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY      "NxAUserPreferencesKeysSeparator"

// -- Static Variables

static Array<String> p_availableSeparators = {
        ","_String,
        ", "_String,
        "/"_String,
        " / "_String,
        " "_String
};

// -- Class Methods

void PreferencesCategoryGeneral::registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*> userPreferences)
{
    RekordBuddy::UserPreferences::registerDefaultPreferenceValuesIn(userPreferences);
}

// -- Instance Methods

void PreferencesCategoryGeneral::p_handleMusicalNotationChanged(int newIndex)
{
    QVariant data = this->p_preferencesPanelContents->comboBoxKeyNotation->itemData(newIndex, Qt::UserRole);
    if (data.isValid()) {
        auto valueAsInteger = data.toInt();
        auto value = static_cast<Common::MusicalKey::Notation>(valueAsInteger);
        if (value <= Common::MusicalKey::Notation::Maximum) {
            Common::MusicalKey::setDefaultNotation(value);
            this->p_userPreferences->setIntegerForKey(valueAsInteger, String{ NXA_MUSICAL_NOTATION_PREFERENCES_KEY });
        }
    }
}

void PreferencesCategoryGeneral::p_handleArtistSeparatorChanged(int newIndex)
{
    QVariant data = this->p_preferencesPanelContents->comboBoxArtistSeparator->itemData(newIndex, Qt::UserRole);
    if (data.isValid()) {
        this->p_userPreferences->setStringForKey(fromString(data.toString()),
                                                 String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    }
}

void PreferencesCategoryGeneral::p_handleGenresSeparatorChanged(int newIndex)
{
    QVariant data = this->p_preferencesPanelContents->comboBoxGenreSeparator->itemData(newIndex, Qt::UserRole);
    if (data.isValid()) {
        this->p_userPreferences->setStringForKey(fromString(data.toString()),
                                                 String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
    }
}

void PreferencesCategoryGeneral::p_handleKeysSeparatorChanged(int newIndex)
{
    QVariant data = this->p_preferencesPanelContents->comboBoxKeySeparator->itemData(newIndex, Qt::UserRole);
    if (data.isValid()) {
        this->p_userPreferences->setStringForKey(fromString(data.toString()),
                                                 String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
    }
}

// -- Overidden Instance Methods

NxA::String PreferencesCategoryGeneral::CategoryName() const
{
    return NxA::String(QObject::tr("Rekord Buddy").toUtf8().constData());
}

PreferencesCategoryGeneral::PreferencesCategoryGeneral(NotNull<NxA::UserPreferences*> userPreferences) : PreferencesCategoryBase{ std::move(userPreferences) }
{
    auto maybeKeyNotation = this->p_userPreferences->maybeIntegerForKey(String{ NXA_MUSICAL_NOTATION_PREFERENCES_KEY });
    if (maybeKeyNotation.isValid()) {
        Common::MusicalKey::setDefaultNotation(static_cast<Common::MusicalKey::Notation>(*maybeKeyNotation));
    }
}

PreferencesCategoryGeneral::~PreferencesCategoryGeneral() {
    delete this->p_preferencesPanelContents;
}

void PreferencesCategoryGeneral::populatePreferencesPanel(QWidget* panel)
{
    this->p_preferencesPanelContents = new Ui::PreferencesGeneral{};
    this->p_preferencesPanelContents->setupUi(panel);

    void (QComboBox::*signalPointer)(int) = &QComboBox::currentIndexChanged;
    {
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr("Musical"), int(Common::MusicalKey::Notation::Standard));
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr("All Sharps"), int(Common::MusicalKey::Notation::StandardAllSharps));
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr("Camelot"), int(Common::MusicalKey::Notation::Camelot));
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr("Camelot (leading 0)"),
                                                                       int(Common::MusicalKey::Notation::CamelotLeadingZero));
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr("Open Key"), int(Common::MusicalKey::Notation::OpenKey));
        this->p_preferencesPanelContents->comboBoxKeyNotation->addItem(QObject::tr(("Open Key (leading 0)")),
                                                                       int(Common::MusicalKey::Notation::OpenKeyLeadingZero));
        auto maybeNotationChoice = this->p_userPreferences->maybeIntegerForKey(String{ NXA_MUSICAL_NOTATION_PREFERENCES_KEY });
        if (maybeNotationChoice.isValid()) {
            integer32 notationChoiceIndex = this->p_preferencesPanelContents->comboBoxKeyNotation->findData(*maybeNotationChoice);
            if (notationChoiceIndex != -1) {
                this->p_preferencesPanelContents->comboBoxKeyNotation->setCurrentIndex(notationChoiceIndex);
            }
        }
        QComboBox::connect(this->p_preferencesPanelContents->comboBoxKeyNotation, signalPointer,
                           [=](int newIndex) {
                               this->p_handleMusicalNotationChanged(newIndex);
                           });
    }

    {
        for (auto&& separator : p_availableSeparators) {
            auto itemText = fromString(String{ "Artist1" }.stringByAppending(separator).stringByAppending("Artist2"));
            this->p_preferencesPanelContents->comboBoxArtistSeparator->addItem(itemText, fromString(separator));
        }
        auto maybeArtistsSeparator = this->p_userPreferences->maybeStringForKey(String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
        if (maybeArtistsSeparator.isValid()) {
            integer32 artistsSeparatorIndex = this->p_preferencesPanelContents->comboBoxArtistSeparator->findData(fromString(*maybeArtistsSeparator));
            if (artistsSeparatorIndex != -1) {
                this->p_preferencesPanelContents->comboBoxArtistSeparator->setCurrentIndex(artistsSeparatorIndex);
            }
        }
        QComboBox::connect(this->p_preferencesPanelContents->comboBoxArtistSeparator, signalPointer,
                           [=](int newIndex) {
                               this->p_handleArtistSeparatorChanged(newIndex);
                           });
    }

    {
        for (auto&& separator : p_availableSeparators) {
            auto itemText = fromString(String{ "Genre1" }.stringByAppending(separator).stringByAppending("Genre2"));
            this->p_preferencesPanelContents->comboBoxGenreSeparator->addItem(itemText, fromString(separator));
        }
        auto maybeGenresSeparator = this->p_userPreferences->maybeStringForKey(String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
        if (maybeGenresSeparator.isValid()) {
            integer32 genresSeparatorIndex = this->p_preferencesPanelContents->comboBoxGenreSeparator->findData(fromString(*maybeGenresSeparator));
            if (genresSeparatorIndex != -1) {
                this->p_preferencesPanelContents->comboBoxGenreSeparator->setCurrentIndex(genresSeparatorIndex);
            }
        }
        QComboBox::connect(this->p_preferencesPanelContents->comboBoxGenreSeparator, signalPointer,
                           [=](int newIndex) {
                               this->p_handleGenresSeparatorChanged(newIndex);
                           });
    }

    {
        for (auto&& separator : p_availableSeparators) {
            auto itemText = fromString(String{ "Key1" }.stringByAppending(separator).stringByAppending("Key2"));
            this->p_preferencesPanelContents->comboBoxKeySeparator->addItem(itemText, fromString(separator));
        }
        auto maybeKeysSeparator = this->p_userPreferences->maybeStringForKey(String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
        if (maybeKeysSeparator.isValid()) {
            integer32 musicalKeysSeparatorIndex = this->p_preferencesPanelContents->comboBoxKeySeparator->findData(fromString(*maybeKeysSeparator));
            if (musicalKeysSeparatorIndex != -1) {
                this->p_preferencesPanelContents->comboBoxKeySeparator->setCurrentIndex(musicalKeysSeparatorIndex);
            }
        }
        QComboBox::connect(this->p_preferencesPanelContents->comboBoxKeySeparator, signalPointer,
                           [=](int newIndex) {
                               this->p_handleKeysSeparatorChanged(newIndex);
                           });
    }
}

void PreferencesCategoryGeneral::eventWindowOpened()
{

}

boolean PreferencesCategoryGeneral::hasPreferencesToMigrate() const
{
    return this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_MUSICAL_NOTATION_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_ARTISTS_SEPARATOR_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_GENRES_SEPARATOR_PREFERENCES_KEY }).isValid() ||
           this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY }).isValid();
}

void PreferencesCategoryGeneral::migratePreferenceValues() const
{
    static Array<String> oldAvailableSeparators = {
            ","_String,
            ", "_String,
            "/"_String,
            " / "_String,
            " "_String
    };

    auto maybeValue = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_MUSICAL_NOTATION_PREFERENCES_KEY });
    if (maybeValue.isValid()) {
        this->p_userPreferences->setIntegerForKey(*maybeValue, String{ NXA_MUSICAL_NOTATION_PREFERENCES_KEY });
    }
    maybeValue = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    if (maybeValue.isValid()) {
        this->p_userPreferences->setStringForKey(oldAvailableSeparators[*maybeValue],
                                                 String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    }
    maybeValue = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_GENRES_SEPARATOR_PREFERENCES_KEY });
    if (maybeValue.isValid()) {
        this->p_userPreferences->setStringForKey(oldAvailableSeparators[*maybeValue],
                                                 String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
    }
    maybeValue = this->p_userPreferences->maybeOSIntegerForKey(String{ NXA_OLD_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
    if (maybeValue.isValid()) {
        this->p_userPreferences->setStringForKey(oldAvailableSeparators[*maybeValue],
                                                 String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
    }
}

boolean PreferencesCategoryGeneral::appClashesWithAppNamed(const String& name)
{
    static boolean foundOneRekordBuddy = false;

    // -- Here we just want to make sure only one copy of Rekord Buddy is running.
    if ((name == "rekord buddy 2"_String) || (name == "rekord buddy"_String)) {
        if (foundOneRekordBuddy) {
            return true;
        }

        foundOneRekordBuddy = true;
    }

    return false;
}
