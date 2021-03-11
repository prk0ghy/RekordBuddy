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

#include <UiMainWindow/StringMatchWidget.hpp>

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qstring.h>

#include "UiMainWindow/moc_StringMatchWidget.cpp"

class QWidget;

using namespace NxA;
using namespace RekordBuddy;

StringMatchWidget::StringMatchWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : ItemRuleWidget{withItem, parent}
{
    this->setupUi();
    StringMatchWidget::loadFromCurrentItem();
}

void StringMatchWidget::loadFromCurrentItem()
{
    switch (this->getItem()->getMethod()) {
        case StringMatchMethod::Contain: {
            this->matchMethod->setCurrentIndex(0);
            break;
        }
        case StringMatchMethod::DoesNotContain: {
            this->matchMethod->setCurrentIndex(1);
            break;
        }
        case StringMatchMethod::Equal: {
            this->matchMethod->setCurrentIndex(2);
            break;
        }
        case StringMatchMethod::NotEqual: {
            this->matchMethod->setCurrentIndex(3);
            break;
        }
        case StringMatchMethod::StartsWith: {
            this->matchMethod->setCurrentIndex(4);
            break;
        }
        case StringMatchMethod::EndsWith: {
            this->matchMethod->setCurrentIndex(5);
            break;
        }
    }
    this->matchContents->setText(this->getItem()->getValue());
}

void StringMatchWidget::saveToRules()
{
    this->getItem()->setMethod(static_cast<StringMatchMethod>(this->matchMethod->currentData().value<integer64>()));
    this->getItem()->setValue(this->matchContents->text());
}

void StringMatchWidget::setupUi()
{
    matchMethod = new QComboBox{this};
    matchMethod->setObjectName(QStringLiteral("matchMethod"));
    matchMethod->addItem(tr("Contains"), static_cast<integer64>(StringMatchMethod::Contain));
    matchMethod->addItem(tr("Does Not Contain"), static_cast<integer64>(StringMatchMethod::DoesNotContain));
    matchMethod->addItem(tr("Matches"), static_cast<integer64>(StringMatchMethod::Equal));
    matchMethod->addItem(tr("Doesn't Match"), static_cast<integer64>(StringMatchMethod::NotEqual));
    matchMethod->addItem(tr("Starts With"), static_cast<integer64>(StringMatchMethod::StartsWith));
    matchMethod->addItem(tr("Ends With"), static_cast<integer64>(StringMatchMethod::EndsWith));

    matchContents = new QLineEdit{this};
    matchContents->setObjectName(QStringLiteral("matchContents"));

    horizontalLayout = new QHBoxLayout{};
    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setMargin(8);
    horizontalLayout->setSpacing(8);

    this->setLayout(horizontalLayout);

    horizontalLayout->addWidget(matchMethod);
    horizontalLayout->addWidget(matchContents);

    horizontalLayout->setAlignment(matchMethod, Qt::AlignVCenter);
    horizontalLayout->setAlignment(matchContents, Qt::AlignVCenter);
}
