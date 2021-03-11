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

#include <UiMainWindow/PredicateWidget.hpp>
#include <UiMainWindow/RuleGroupWidget.hpp>

#include <qboxlayout.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>

#include "UiMainWindow/moc_PredicateWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

PredicateWidget::PredicateWidget(NotNull<AbstractGroupItem*> withItem, QWidget* parent) : QWidget{parent}
{
    this->setupUi(withItem);
}

void PredicateWidget::setupUi(NotNull<AbstractGroupItem*> item)
{
    verticalLayout = new QVBoxLayout{this};
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setMargin(0);
    verticalLayout->setSpacing(0);
    verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);

    rules = new RuleGroupWidget{item, this};
    verticalLayout->addWidget(rules);

    verticalSpacer = new QSpacerItem{20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding};
    verticalLayout->addItem(verticalSpacer);

    auto horizontal = new QVBoxLayout{};
    horizontal->setObjectName(QStringLiteral("horizontalLayout"));
    auto horizontalSpace = new QSpacerItem{20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum};

    horizontal->addItem(horizontalSpace);
    verticalLayout->addItem(horizontal);
}

void PredicateWidget::saveToRules()
{
    this->rules->saveToRules();
}

NotNull<AbstractGroupItem*> PredicateWidget::getPredicatesAsGroup()
{
    return this->rules->getGroup();
}
