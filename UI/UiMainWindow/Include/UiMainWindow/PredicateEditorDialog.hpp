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

#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>

#include <UiMainWindow/PredicateWidget.hpp>

#include <Base/Threading.hpp>

#include <qboxlayout.h>
#include <qdialog.h>
#include <qobjectdefs.h>
#include <qstring.h>

class QObject;
class QWidget;
class QDialogButtonBox;

namespace NxA::RekordBuddy {

class AbstractCrateItem;

class PredicateEditorDialog : public QDialog
{
    Q_OBJECT
    AbstractCrateItem* smartPlaylist;
    PredicateWidget* editor;
    QVBoxLayout* mainLayout;
    QDialogButtonBox* buttonBox;
    std::unique_ptr<AbstractGroupItem> item;

public:
    PredicateEditorDialog(AbstractCrateItem* withSmartPlaylist, std::unique_ptr<AbstractGroupItem> withItem, QWidget* parent);
    void saveToSmartPlaylist();

public slots:
    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }

signals:
    void smartPlaylistUpdated(QString);

private slots:
    void acceptChanges();
    void rejectChanges();
};
}
