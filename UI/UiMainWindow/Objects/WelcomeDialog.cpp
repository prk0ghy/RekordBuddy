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

#include <UiMainWindow/WelcomeDialog.hpp>
#include <UiMainWindow/MainWindow.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include "ui_WelcomeDialog.h"
#include "UiMainWindow/moc_WelcomeDialog.cpp"

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Constants

#define NXA_DO_NOT_SHOW_WELCOME_DIALOG_BELOW_PREFERENCES_KEY                    "DoNotShowWelcomeDialogsBelow"
#define NXA_CURRENT_WELCOME_DIALOG_INDEX                                        1

// -- Class Methods

boolean WelcomeDialog::shouldBeDisplayed()
{
    auto maybeCurrentDialogIndex = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume())->maybeIntegerForKey(String{ NXA_DO_NOT_SHOW_WELCOME_DIALOG_BELOW_PREFERENCES_KEY });
    if (maybeCurrentDialogIndex.isValid() && (*maybeCurrentDialogIndex >= NXA_CURRENT_WELCOME_DIALOG_INDEX)) {
        return false;
    }

    return true;
}

// -- Constructors & Destructors

WelcomeDialog::WelcomeDialog(QWidget* parent) : QDialog{ parent, Qt::FramelessWindowHint },
                                                p_ui{ new Ui::WelcomeDialog }
{
    this->p_ui->setupUi(this);

    connect(this->p_ui->okButton, &QAbstractButton::clicked, this, &WelcomeDialog::clickOk);
    connect(this->p_ui->doNotShowAgain, &QAbstractButton::clicked, this, &WelcomeDialog::doNotShowUpdated);

    auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());
    if (userPreferences->maybeIntegerForKey(String{ NXA_DO_NOT_SHOW_WELCOME_DIALOG_BELOW_PREFERENCES_KEY }).valueOr(0) >= NXA_CURRENT_WELCOME_DIALOG_INDEX) {
        this->p_ui->doNotShowAgain->setChecked(true);
    }
}

WelcomeDialog::~WelcomeDialog()
{
    delete this->p_ui;
}

// -- Instance Methods

void WelcomeDialog::clickOk()
{
    this->close();

    if (this->p_maybeCallback.isValid()) {
        (*this->p_maybeCallback)();
    }
}

void WelcomeDialog::doNotShowUpdated()
{
    auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());

    if (this->p_ui->doNotShowAgain->isChecked()) {
        userPreferences->setIntegerForKey(NXA_CURRENT_WELCOME_DIALOG_INDEX, String{ NXA_DO_NOT_SHOW_WELCOME_DIALOG_BELOW_PREFERENCES_KEY });
    }
    else {
        userPreferences->setIntegerForKey(nothing, String{ NXA_DO_NOT_SHOW_WELCOME_DIALOG_BELOW_PREFERENCES_KEY });
    }
}
