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

#include <UiPreferences/PreferencesWindow.hpp>
#include <UiModel/Preferences/PreferencesModuleManager.hpp>
#include "ui_PreferencesWindow.h"
#include "UiPreferences/moc_PreferencesWindow.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors
PreferencesWindow::PreferencesWindow(QWidget* parent) : QDialog{ parent },
                                                        p_ui{ new Ui::PreferencesWindow }
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    this->p_ui->setupUi(this);
    this->p_ui->categoryList->setModel(&this->p_categoryListModel);

    connect(this->p_ui->categoryList, &QAbstractItemView::clicked, this, &PreferencesWindow::selectPreferencesCategory);

    while (auto child = this->p_ui->optionsPanel->findChild<QWidget*>()) {
        delete child;
    }

    count pageIndex = 0;
    PreferencesCategoryBase* preferenceModule = nullptr;
    while ((preferenceModule = RekordBuddy::PreferencesModuleManager::getModuleInterface(pageIndex++)) != nullptr) {
        auto container = new QWidget{ this->p_ui->optionsPanel };
        this->p_ui->optionsPanel->addWidget(container);
        preferenceModule->populatePreferencesPanel(container);
    }
}

PreferencesWindow::~PreferencesWindow()
{
    delete this->p_ui;
}

// -- Instance Methods

void PreferencesWindow::showAndCallOnClose(std::function<void()>&& func)
{
    this->p_maybeCallAfterClose = { std::move(func) };

    this->show();
}

void PreferencesWindow::selectPreferencesCategory(const QModelIndex& index)
{
    this->p_ui->optionsPanel->setCurrentIndex(index.row());
}

// -- Overridden Instance Methods

void PreferencesWindow::showEvent(QShowEvent*)
{
    count pageIndex= 0;
    PreferencesCategoryBase* preferenceModule = nullptr;
    while ((preferenceModule = RekordBuddy::PreferencesModuleManager::getModuleInterface(pageIndex++)) != nullptr) {
        preferenceModule->eventWindowOpened();
    }
}

void PreferencesWindow::closeEvent(QCloseEvent*)
{
    if (this->p_maybeCallAfterClose.isValid()) {
        (*this->p_maybeCallAfterClose)();
    }

    this->p_maybeCallAfterClose = nothing;
}
