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

#include <UiMainWindow/DateMatchWidget.hpp>
#include <AbstractViewCollection/Predicate/AbstractDateItem.hpp>

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlayout.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include "UiMainWindow/moc_DateMatchWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

DateMatchWidget::DateMatchWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : ItemRuleWidget{withItem, parent}
{
    this->setupUi();
    DateMatchWidget::loadFromCurrentItem();
}

void DateMatchWidget::loadFromCurrentItem()
{
    switch (this->getItem()->getMethod()) {
        case DateMatchMethod::Is: {
            this->matchMethod->setCurrentIndex(0);
            break;
        }
        case DateMatchMethod::IsNot: {
            this->matchMethod->setCurrentIndex(1);
            break;
        }
        case DateMatchMethod::Before: {
            this->matchMethod->setCurrentIndex(2);
            break;
        }
        case DateMatchMethod::After: {
            this->matchMethod->setCurrentIndex(3);
            break;
        }
    }
    this->matchContents->setDate(this->getItem()->getValue());
}

void DateMatchWidget::saveToRules()
{
    auto date = this->matchContents->date();
    this->getItem()->setMethod(static_cast<DateMatchMethod>(this->matchMethod->currentData().value<integer64>()));
    this->getItem()->setValue(date);
}

void DateMatchWidget::setupUi()
{
    matchMethod = new QComboBox{this};
    matchMethod->setObjectName(QStringLiteral("matchMethod"));
    matchMethod->addItem(QObject::tr("is"), static_cast<integer64>(DateMatchMethod::Is));
    matchMethod->addItem(QObject::tr("is not"), static_cast<integer64>(DateMatchMethod::IsNot));
    matchMethod->addItem(QObject::tr("before"), static_cast<integer64>(DateMatchMethod::Before));
    matchMethod->addItem(QObject::tr("after"), static_cast<integer64>(DateMatchMethod::After));

    matchContents = new QDateEdit{this};
    matchContents->setCalendarPopup(true);
    matchContents->setObjectName(QStringLiteral("matchContents"));

    horizontalLayout = new QHBoxLayout{this};
    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setMargin(8);
    horizontalLayout->setSpacing(8);

    this->setLayout(horizontalLayout);

    this->matchMethod->setContentsMargins(4, 0, 0, 0);
    this->matchContents->setContentsMargins(4, 0, 0, 0);

    horizontalLayout->addWidget(matchMethod);
    horizontalLayout->addWidget(matchContents);
}
