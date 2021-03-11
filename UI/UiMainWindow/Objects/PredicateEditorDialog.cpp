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

#include <UiMainWindow/PredicateEditorDialog.hpp>
#include <UiMainWindow/PredicateWidget.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>
#include <QDialogButtonBox>
#include "UiMainWindow/moc_PredicateEditorDialog.cpp"

class QWidget;

using namespace NxA;
using namespace RekordBuddy;

PredicateEditorDialog::PredicateEditorDialog(AbstractCrateItem* withSmartPlaylist, std::unique_ptr<AbstractGroupItem> withItem, QWidget* parent)
    : QDialog{parent}, smartPlaylist{withSmartPlaylist}, item{std::move(withItem)}
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());
    NXA_ASSERT_NOT_NULL(smartPlaylist);

    this->mainLayout = new QVBoxLayout{this};
    this->buttonBox = new QDialogButtonBox{this};
    this->mainLayout->setMargin(12);
    this->mainLayout->setSpacing(0);
    this->setWindowModality(Qt::WindowModal);
    this->editor = new PredicateWidget{item.get(), this};
    this->editor->setObjectName("MainPredicateWidget");
    this->resize(600, 400);

    this->buttonBox->setObjectName("predicateButtonBox");
    QSizePolicy sizePolicy{QSizePolicy::Expanding, QSizePolicy::Fixed};
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->buttonBox->sizePolicy().hasHeightForWidth());
    this->buttonBox->setSizePolicy(sizePolicy);
    this->buttonBox->setOrientation(Qt::Horizontal);
    this->buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    this->buttonBox->setCenterButtons(true);

    this->setLayout(this->mainLayout);
    mainLayout->addWidget(this->editor);
    mainLayout->addWidget(this->buttonBox);
    connect(this->buttonBox, &QDialogButtonBox::accepted, this, &PredicateEditorDialog::acceptChanges);
    connect(this->buttonBox, &QDialogButtonBox::rejected, this, &PredicateEditorDialog::rejectChanges);
}

void PredicateEditorDialog::rejectChanges()
{
    this->close();
}

void PredicateEditorDialog::acceptChanges()
{
    this->saveToSmartPlaylist();
    this->close();
}

void PredicateEditorDialog::saveToSmartPlaylist()
{
    this->editor->saveToRules();
    auto predicateGroup = this->editor->getPredicatesAsGroup();
    this->smartPlaylist->setPredicateGroup(predicateGroup);
    // TODO: Could we get the new name from Collection somehow? just generate it here?
    emit smartPlaylistUpdated("TODO: Name Based on Query");
}
