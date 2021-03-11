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

#include <WorkQueue/WorkQueue.hpp>

#include <Base/NotNull.hpp>
#include <Base/Threading.hpp>

#include <QTimer>
#include <QDialog>

#include <memory>

namespace Ui {

// -- Forward Declarations
class AboutDialog;

}

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode
    {
        Splash,
        About
    };

    // -- Constructors & Destructors
    AboutDialog(WorkQueue& withWorkQueue, QWidget* parent, Mode mode);
    ~AboutDialog() override;

    // -- Instance Methods
    void dismissWindow();

    void keyPressEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }

private:
    NotNull<Ui::AboutDialog*> p_ui;
    Mode p_usageMode;
};

} }
