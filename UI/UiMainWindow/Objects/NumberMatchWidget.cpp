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

#include <UiMainWindow/NumberMatchWidget.hpp>

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qobjectdefs.h>
#include <qspinbox.h>
#include <qstring.h>

#include "UiMainWindow/moc_NumberMatchWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

NumberMatchWidget::NumberMatchWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : ItemRuleWidget{withItem, parent}
{
    this->setupUi();
    NumberMatchWidget::loadFromCurrentItem();
}

void NumberMatchWidget::loadFromCurrentItem()
{
    switch (this->getItem()->getValue()) {
        case static_cast<integer64>(NumberMatchMethod::Equal): {
            this->matchMethod->setCurrentIndex(0);
            break;
        }
        case static_cast<integer64>(NumberMatchMethod::LessThan): {
            this->matchMethod->setCurrentIndex(1);
            break;
        }
        case static_cast<integer64>(NumberMatchMethod::LessThanOrEqual): {
            this->matchMethod->setCurrentIndex(2);
            break;
        }
        case static_cast<integer64>(NumberMatchMethod::GreaterThan): {
            this->matchMethod->setCurrentIndex(3);
            break;
        }
        case static_cast<integer64>(NumberMatchMethod::GreaterThanOrEqual): {
            this->matchMethod->setCurrentIndex(4);
            break;
        }
    }
    this->matchContents->setValue(this->getItem()->getValue());
}

void NumberMatchWidget::saveToRules()
{
    this->getItem()->setMethod(static_cast<NumberMatchMethod>(this->matchMethod->currentData().value<integer64>()));
    this->getItem()->setValue(this->matchContents->value());
}

void NumberMatchWidget::setupUi()
{
    matchMethod = new QComboBox{this};
    matchMethod->setObjectName(QStringLiteral("matchMethod"));
    matchMethod->addItem(tr("Is Equal To"), static_cast<integer64>(NumberMatchMethod::Equal));
    matchMethod->addItem(tr("Less Than"), static_cast<integer64>(NumberMatchMethod::LessThan));
    matchMethod->addItem(tr("Less Than or Equal"), static_cast<integer64>(NumberMatchMethod::LessThanOrEqual));
    matchMethod->addItem(tr("Greater Than"), static_cast<integer64>(NumberMatchMethod::GreaterThan));
    matchMethod->addItem(tr("Greater Than or Equal"), static_cast<integer64>(NumberMatchMethod::GreaterThanOrEqual));

    matchContents = new QSpinBox{this};
    matchContents->setObjectName(QStringLiteral("matchContents"));

    horizontalLayout = new QHBoxLayout{this};
    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setMargin(8);
    horizontalLayout->setSpacing(8);

    this->setLayout(this->horizontalLayout);

    this->matchMethod->setContentsMargins(4, 0, 0, 0);
    this->matchContents->setContentsMargins(4, 0, 0, 0);

    horizontalLayout->addWidget(this->matchMethod);
    horizontalLayout->addWidget(this->matchContents);
}
