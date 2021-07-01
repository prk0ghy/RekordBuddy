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

#include <UiModel/Preferences/PreferencesCategoryBase.hpp>
#include <Base/Base.hpp>

namespace Ui {

class PreferencesRekordbox;

}

namespace NxA { namespace RekordBuddy {

class PreferencesCategoryRekordbox : public PreferencesCategoryBase
{
protected:
    // -- Protected Instance Variables
    Ui::PreferencesRekordbox* p_preferencesPanelContents = nullptr;

    // -- Protected Instance Methods
    void p_handleNumberHotcuesChanged(int);
    void p_handleGridMarkersIgnoredChanged(int);
    void p_handleDuplicateHotcuesFlagChanged(int);
    void p_handleAllowVideoTracksInPlaylistsFlagChanged(int);

public:
    // -- Class Methods
    static void registerDefaultPreferenceValuesIn(NotNull<NxA::UserPreferences*>);

    // -- Constructors & Destructors
    PreferencesCategoryRekordbox(NotNull<NxA::UserPreferences*> userPreferences) : PreferencesCategoryBase{ std::move(userPreferences) } { }
    ~PreferencesCategoryRekordbox() override;

    // -- Instance Methods
    NxA::String CategoryName() const override;
    void populatePreferencesPanel(QWidget*) override;

    boolean appClashesWithAppNamed(const String&) override;
};

} }
