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

#include <UiMainWindow/TagMatchWidget.hpp>
#include <Widgets/TagWidget.hpp>

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qobjectdefs.h>

#include "UiMainWindow/moc_TagMatchWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

TagMatchWidget::TagMatchWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : ItemRuleWidget{withItem, parent}
{
    this->setupUi();
}

void TagMatchWidget::loadFromCurrentItem()
{
    switch (this->getItem()->getMethod()) {
        case TagMatchMethod::Tagged: {
            this->matchMethod->setCurrentIndex(0);
            break;
        }
        case TagMatchMethod::NotTagged: {
            this->matchMethod->setCurrentIndex(1);
            break;
        }
    }

    // TODO: Collection does not support multiple tags here, either limit the query length, or add list support?
    this->matchContents->setTags({this->getItem()->getValue()});
}

void TagMatchWidget::saveToRules()
{
    this->getItem()->setMethod(static_cast<TagMatchMethod>(this->matchMethod->currentData().value<integer64>()));
    // TODO: Collection does not support multiple tags here, either limit the query length, or add list support? this will crash
    this->getItem()->setValue(this->matchContents->tags().first());
}

void TagMatchWidget::setupUi()
{
    this->matchMethod = new QComboBox{this};
    this->matchMethod->setObjectName(QStringLiteral("matchMethod"));
    this->matchMethod->addItem(tr("Include"), static_cast<integer64>(TagMatchMethod::Tagged));
    this->matchMethod->addItem(tr("Exclude"), static_cast<integer64>(TagMatchMethod::NotTagged));

    this->matchContents = new TagWidget{this};
    this->matchContents->setObjectName(QStringLiteral("matchContents"));

    this->horizontalLayout = new QHBoxLayout{};
    this->horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    this->horizontalLayout->setMargin(8);
    this->horizontalLayout->setSpacing(8);

    this->setLayout(this->horizontalLayout);

    this->horizontalLayout->addWidget(this->matchMethod);
    this->horizontalLayout->addWidget(this->matchContents);

    this->horizontalLayout->setAlignment(this->matchMethod, Qt::AlignVCenter);
    this->horizontalLayout->setAlignment(this->matchContents, Qt::AlignVCenter);
}
