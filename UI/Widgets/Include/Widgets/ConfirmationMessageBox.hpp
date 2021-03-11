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

#include <Base/String.hpp>
#include <Base/Threading.hpp>

#include <qdialog.h>
#include <QDialogButtonBox>

namespace Ui {

class ConfirmationMessageBox;

}

namespace NxA::RekordBuddy {

class ConfirmationMessageBox : public QDialog
{
    Ui::ConfirmationMessageBox* ui{ nullptr };

public:
    // -- Class Methods
    static bool promptForConfirmOkCancel(const QString& title, const QString& prompt, const QString& info)
    {
        ConfirmationMessageBox messageBox(nullptr);
        messageBox.setText(title, prompt, info);
        messageBox.setButtonLayout(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        return (messageBox.exec() == QDialog::Accepted);
    }
    static bool promptForConfirmYesNo(const QString& title, const QString& prompt, const QString& info)
    {
        ConfirmationMessageBox messageBox(nullptr);
        messageBox.setText(title, prompt, info);
        messageBox.setButtonLayout(QDialogButtonBox::Yes | QDialogButtonBox::No);

        return (messageBox.exec() == QDialog::Accepted);
    }

    // -- Constructors & Destructors
    ConfirmationMessageBox(QWidget*);
    ~ConfirmationMessageBox() override;

    // -- Instance Methods
    void setText(const QString&, const QString&, const QString&);
    void setButtonLayout(QDialogButtonBox::StandardButtons);
    void setButtonLayout(const String&, const String&);

    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }
};

}
