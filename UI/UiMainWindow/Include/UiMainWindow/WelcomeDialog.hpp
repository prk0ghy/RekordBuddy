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

#include <Base/Threading.hpp>
#include <Base/Types.hpp>

#include <QDialog>

namespace Ui {

// -- Forward Declarations
class WelcomeDialog;

}

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class WelcomeDialog : public QDialog
{
    Q_OBJECT

    // -- Private Instance variables
    Ui::WelcomeDialog* p_ui;
    Optional<std::function<void()>> p_maybeCallback;

    // -- Private Instance Methods
private slots:
    void clickOk();
    void doNotShowUpdated();

public:
    // -- Class Methods
    static boolean shouldBeDisplayed();

    // -- Constructors & Destructors
    explicit WelcomeDialog(QWidget* parent);
    ~WelcomeDialog();

    // -- Instance Methods
    void showThenCallWhenClosed(std::function<void()> callback)
    {
        this->p_maybeCallback = std::move(callback);
        this->show();
    }
};

} }
