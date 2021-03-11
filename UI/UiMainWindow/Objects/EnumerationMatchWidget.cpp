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

#include <UiMainWindow/EnumerationMatchWidget.hpp>

#include <qglobal.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <qvariant.h>

#include "UiMainWindow/moc_EnumerationMatchWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

EnumerationMatchWidget::EnumerationMatchWidget(QStringList withChoices, NotNull<AbstractPredicateItem*> withItem, QWidget* parent)
    : ItemRuleWidget{withItem, parent}, choices{std::move(withChoices)}
{
    this->setupUi();
    this->loadFromCurrentItem();
}

QStringList EnumerationMatchWidget::getChoices()
{
    return this->choices;
}

void EnumerationMatchWidget::loadFromCurrentItem()
{
    auto value = this->getItem()->getValue();
    this->matchMethod->setCurrentText(value);
}

void EnumerationMatchWidget::saveToRules()
{
    this->getItem()->setValue(this->matchMethod->currentText());
}

void EnumerationMatchWidget::setupUi()
{
    horizontalLayout = new QHBoxLayout{this};

    this->matchIs = new QLabel{this};

    QSizePolicy sizePolicy{QSizePolicy::Minimum, QSizePolicy::Minimum};
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->matchIs->sizePolicy().hasHeightForWidth());

    this->matchIs->setSizePolicy(sizePolicy);
    this->matchIs->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    matchMethod = new QComboBox{this};

    for (const auto& choice : this->choices) {
        matchMethod->addItem(choice);
    }

    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setMargin(8);
    horizontalLayout->setSpacing(8);

    this->setLayout(horizontalLayout);

    this->matchMethod->setContentsMargins(4, 0, 0, 0);

    horizontalLayout->addWidget(this->matchIs);
    horizontalLayout->addWidget(matchMethod);
}
