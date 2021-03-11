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

#include <UiBase/AlertMessageBox.hpp>

#include "ui_AlertMessageBox.h"

#include "UiBase/moc_AlertMessageBox.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors
AlertMessageBox::AlertMessageBox(QWidget* parent) : QDialog{parent}, ui{new Ui::AlertMessageBox}
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    ui->setupUi(this);
    ui->icon->setPixmap(QPixmap(QStringLiteral(":/Icons/Warnings and Errors/Alert Icon/icon_512x512.png")));
    ui->textMoreInformation->setVisible(false);
    ui->buttonMoreInformation->setArrowType(Qt::RightArrow);
    ui->buttonMoreInformation->setVisible(false);
    ui->labelMoreInformation->setVisible(false);
    connect(ui->buttonMoreInformation, &QToolButton::clicked, this, &AlertMessageBox::eventMoreInformationClicked);
}

// -- Instance Methods
void AlertMessageBox::setText(const QString& title, const QString& description, const QString& suggestion)
{
    this->setWindowTitle(title);
    ui->textDescription->setText(description);
    ui->textRecoverySuggestion->setText(suggestion);
}

void AlertMessageBox::setButtonLayout(const QString& dismissText)
{
    ui->buttonBox->clear();
    ui->buttonBox->addButton(dismissText, QDialogButtonBox::AcceptRole);
}

void AlertMessageBox::setButtonLayout(const QString& acceptText, const QString& cancelText)
{
    ui->buttonBox->clear();
    ui->buttonBox->addButton(acceptText, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(cancelText, QDialogButtonBox::RejectRole);
}

void AlertMessageBox::setMoreInformation(const QString& moreInformationText)
{
    ui->buttonMoreInformation->setVisible(true);
    ui->labelMoreInformation->setVisible(true);
    ui->textMoreInformation->setPlainText(moreInformationText);
}

void AlertMessageBox::eventMoreInformationClicked()
{
    // Expand or collapse to show the more-information text field in the messagebox.
    if (ui->textMoreInformation->isVisible()) {
        QSize newWindowSize = this->size() + QSize(0, -ui->textMoreInformation->size().height());
        QPoint newButtonPosition = ui->buttonBox->pos() + QPoint(0, -ui->textMoreInformation->size().height());
        this->setMaximumSize(newWindowSize);
        this->setMinimumSize(newWindowSize);
        this->resize(newWindowSize);
        ui->buttonBox->move(newButtonPosition);
        ui->textMoreInformation->setVisible(false);
        ui->buttonMoreInformation->setArrowType(Qt::RightArrow);
    }
    else {
        QSize newWindowSize = this->size() + QSize(0, ui->textMoreInformation->size().height());
        QPoint newButtonPosition = ui->buttonBox->pos() + QPoint(0, ui->textMoreInformation->size().height());
        this->setMaximumSize(newWindowSize);
        this->setMinimumSize(newWindowSize);
        this->resize(newWindowSize);
        ui->buttonBox->move(newButtonPosition);
        ui->textMoreInformation->setVisible(true);
        ui->buttonMoreInformation->setArrowType(Qt::DownArrow);
    }
}

AlertMessageBox::~AlertMessageBox()
{
    delete ui.get();
}
