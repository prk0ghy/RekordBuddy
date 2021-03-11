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

#include <Base/NotNull.hpp>
#include <Base/Threading.hpp>

#include "qdialog.h"

#include <QDialogButtonBox>

namespace Ui {

class AlertMessageBox;

}

namespace NxA { namespace RekordBuddy {

class AlertMessageBox : public QDialog
{
    NotNull<Ui::AlertMessageBox*> ui;

public:
    // -- Constructors & Destructors
    AlertMessageBox(QWidget* = nullptr);
    ~AlertMessageBox() override;

    // -- Instance Methods
    void setText(const QString& title, const QString& description, const QString& suggestion);
    void setButtonLayout(const QString& dismissText);
    void setButtonLayout(const QString& acceptText, const QString& cancelText);
    void setMoreInformation(const QString& moreInformationText);

    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }

private:
    void eventMoreInformationClicked();
};

} }
