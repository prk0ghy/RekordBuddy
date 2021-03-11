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

#include <QWidget>

#include <Base/Base.hpp>

namespace NxA {

// -- Forward Declarations
class UserPreferences;

namespace RekordBuddy {

class PreferencesCategoryBase
{
protected:
    // -- Protected Instance Variables
    NotNull<NxA::UserPreferences*> p_userPreferences;

public:
    // -- Constructors & Destructors
    PreferencesCategoryBase() = delete;
    PreferencesCategoryBase(NotNull<NxA::UserPreferences*> userPreferences) : p_userPreferences{ std::move(userPreferences) } { }
    virtual ~PreferencesCategoryBase() = default;

    // -- Instance Methods
    virtual NxA::String CategoryName() const = 0;
    virtual void migratePreferenceValues() const = 0;
    virtual boolean hasPreferencesToMigrate() const = 0;
    virtual void populatePreferencesPanel(QWidget* panel) = 0;
    virtual void eventWindowOpened()
    {

    }

    virtual boolean appClashesWithAppNamed(const String&) = 0;
};

} }
