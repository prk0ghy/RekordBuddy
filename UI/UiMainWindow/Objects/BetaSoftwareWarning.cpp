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

#include <UiMainWindow/BetaSoftwareWarning.hpp>
#include <UiMainWindow/MainWindow.hpp>

#include "ui_BetaSoftwareWarning.h"
#include "UiMainWindow/moc_BetaSoftwareWarning.cpp"

using namespace NxA;
using namespace NxA::RekordBuddy;

BetaSoftwareWarning::BetaSoftwareWarning(QWidget* parent) : QDialog{ parent, Qt::FramelessWindowHint },
                                                            p_ui{ new Ui::BetaSoftwareWarning }
{
    this->p_ui->setupUi(this);

    connect(this->p_ui->okGo, &QAbstractButton::clicked, this, &BetaSoftwareWarning::clickOk);
    connect(this->p_ui->notReadyButton, &QAbstractButton::clicked, this, &BetaSoftwareWarning::clickNo);
}

BetaSoftwareWarning::~BetaSoftwareWarning()
{
    delete this->p_ui;
}

void BetaSoftwareWarning::clickOk()
{
    this->close();

    if (this->p_maybeCallback.isValid()) {
        (*this->p_maybeCallback)();
    }
}

void BetaSoftwareWarning::clickNo()
{
    exit(0);
}
