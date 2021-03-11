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

#include <UiModel/PreferencesCategoryModel.hpp>

#include <Base/Optional.hpp>
#include <Base/Threading.hpp>

#include <QDialogButtonBox>

#include <qdialog.h>

namespace Ui {

class PreferencesWindow;

}

namespace NxA { namespace RekordBuddy {

class PreferencesWindow : public QDialog
{
    // -- Instance Variables
    Ui::PreferencesWindow* p_ui{ nullptr };

    PreferencesCategoryModel p_categoryListModel;

    Optional<std::function<void()>> p_maybeCallAfterClose;

    // -- Overridden Instance Methods
    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;

public:
    // -- Constructors & Destructors
    PreferencesWindow(QWidget*);
    ~PreferencesWindow() override;

    // -- Instance Methods
    void showAndCallOnClose(std::function<void()>&&);

    void selectPreferencesCategory(const QModelIndex&);

    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }
};

} }
