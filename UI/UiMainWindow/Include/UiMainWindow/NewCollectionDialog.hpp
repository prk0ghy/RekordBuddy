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
#include <Base/Volume.hpp>

#include <QDialogButtonBox>

#include "qdialog.h"

namespace Ui {

class NewCollectionDialog;

}

namespace NxA { namespace RekordBuddy {

class NewCollectionDialog : public QDialog
{
    // -- Private Instance Variables
    NotNull<Ui::NewCollectionDialog*> p_ui;

    MutableArray<Volume> p_volumesWithoutRekordBuddyCollection;

    Optional<std::function<void()>> p_maybeCallAfterClose;

    // -- Private Instance Methods
    void p_createCollection();

public:
    // -- Constructors & Destructors
    NewCollectionDialog(QWidget*);
    ~NewCollectionDialog() override;

    // -- Instance Methods
    int exec() override;
    int execThenCall(std::function<void()>&&);
};

} }
