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

#include <Widgets/ConfirmationMessageBox.hpp>

#include <UiBase/Types.hpp>

#include "ui_ConfirmationMessageBox.h"

#include "Widgets/moc_ConfirmationMessageBox.cpp"

#include <QAbstractButton>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

ConfirmationMessageBox::ConfirmationMessageBox(QWidget* parent) : QDialog{parent}, ui{new Ui::ConfirmationMessageBox}
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    this->ui->setupUi(this);
}

ConfirmationMessageBox::~ConfirmationMessageBox()
{
    delete ui;
}

// -- Instance Methods

void ConfirmationMessageBox::setText(const QString& title, const QString& prompt, const QString& detail)
{
    this->setWindowTitle("");

    ui->titleLabel->setText(title);
    ui->textPrompt->setText(prompt);
    ui->textDetail->setText(detail);
}

void ConfirmationMessageBox::setButtonLayout(QDialogButtonBox::StandardButtons buttons)
{
    ui->buttonBox->setStandardButtons(buttons);
}

void ConfirmationMessageBox::setButtonLayout(const String& firstLabel, const String& secondLabel)
{
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);

    auto buttons = ui->buttonBox->buttons();
    NXA_ASSERT_TRUE(buttons.length() == 2);

    buttons[0]->setText(fromString(secondLabel));
    buttons[1]->setText(fromString(firstLabel));
}
