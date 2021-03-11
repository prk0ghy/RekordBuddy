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

#include <UiMainWindow/NewCollectionDialog.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>
#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/RekordBuddy.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <UiBase/Types.hpp>

#include <Base/FilePath.hpp>

#include "ui_NewCollectionDialog.h"

#include "UiMainWindow/moc_NewCollectionDialog.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

NewCollectionDialog::NewCollectionDialog(QWidget* parent) : QDialog{ parent },
                                                            p_ui{ new Ui::NewCollectionDialog }
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    this->p_ui->setupUi(this);

    this->setWindowTitle({ "New Collection" });
    this->p_ui->textDescription->setText({ "Create a new Rekord Buddy collection." });
    this->p_ui->textRecoverySuggestion->setText(
            { "The following volumes do not currently contain a Rekord Buddy collection. "
              "Please select the one you'd like to create a new collection on." });

    connect(this->p_ui->buttonBox, &QDialogButtonBox::accepted, this, &NewCollectionDialog::p_createCollection);
}

NewCollectionDialog::~NewCollectionDialog()
{
    delete this->p_ui.get();
}

// -- Instance Methods

void NewCollectionDialog::p_createCollection()
{
    auto& volume = this->p_volumesWithoutRekordBuddyCollection[this->p_ui->volumeList->currentIndex()];
    if (makeSureRekorBuddyCollectionExistsOnVolume(volume)) {
        if (this->p_maybeCallAfterClose.isValid()) {
            (*this->p_maybeCallAfterClose)();
        }

        return;
    }

    ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Error creating Rekord Buddy collection."),
                                                     QObject::tr(""),
                                                     QObject::tr("Volume '%1' might be read-only.").arg(fromString(volume.name())));
}

int NewCollectionDialog::exec()
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    for (auto&& volume : otherVolumesWithNoRekordBuddyCollection()) {
        p_volumesWithoutRekordBuddyCollection.append(volume);

        this->p_ui->volumeList->addItem(fromString(volume.name()),0);
    }

    if (!this->p_ui->volumeList->count()) {
        ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("All available volumes seem to already have a Rekord Buddy Collection."),
                                                         QObject::tr(""),
                                                         QObject::tr(""));

        return 0;
    }

    return this->QDialog::exec();
}

int NewCollectionDialog::execThenCall(std::function<void()>&& callBack)
{
    this->p_maybeCallAfterClose = { std::move(callBack) };

    return this->exec();
}
