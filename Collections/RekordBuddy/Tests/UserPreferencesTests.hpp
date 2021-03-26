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

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <Base/FilePath.hpp>
#include <Base/Test.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class UserPreferencesTests : public NxA::Test
{
public:
    // -- Public Class Methods
    static void setUpTestUserPreferencesForVolumeAndTestClass(const Volume& volume, NxA::Test& testClass)
    {
        // -- Creating a new receipt needs to use the current time and not out test time because
        // -- validation code for the receipt uses the current time.
        auto currentTestTime = testClass.testCurrentTime();
        testClass.clearTestCurrentTime();
        testClass.setTestCurrentTime(currentTestTime);

        // -- Testing frameworks need a consistent testing context so we set our own preferences for this test.
        auto userPreferences = RekordBuddy::UserPreferences::forVolume(volume);
        userPreferences->clearAll();

        userPreferences->setStringForKey(", "_String, String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
        userPreferences->setStringForKey(", "_String, String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
        userPreferences->setStringForKey(", "_String, String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
    }
};

} }
