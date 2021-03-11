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

#include <UiMainWindow/RuleGroupWidget.hpp>
#include <UiMainWindow/RuleWidget.hpp>

#include <Base/Base.hpp>

#include <qglobal.h>
#include <qabstractbutton.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>

#include "UiMainWindow/moc_RuleGroupWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

RuleGroupWidget::RuleGroupWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent) : ItemRuleWidget{withItem, parent}
{
    this->setupUi();
    RuleGroupWidget::loadFromCurrentItem();
    connect(this->addButton, &QPushButton::clicked, this, &RuleGroupWidget::clickAddRule);
}

NotNull<AbstractGroupItem*> RuleGroupWidget::getGroup()
{
    return this->getItem();
}

void RuleGroupWidget::loadFromCurrentItem()
{
    switch (this->getItem()->getMethod()) {
        case GroupMatchMethod::All: {
            this->anyAllComboBox->setCurrentIndex(0);
            break;
        }
        case GroupMatchMethod::Any: {
            this->anyAllComboBox->setCurrentIndex(1);
            break;
        }
        default:
            NXA_ALOG("Not implemented");
    }
    QLayoutItem* w = nullptr;
    while ((w = this->ruleLayout->takeAt(0)) != nullptr) {
        ruleDeleted(dynamic_cast<RuleWidget*>(w->widget()));
    }

    auto item = this->getItem();
    for (count index = 0; index < item->predicateCount(); ++index) {
        this->addRule(item->removeFirstPredicate());
    }
}

void RuleGroupWidget::saveToRules()
{
    this->getItem()->setMethod(static_cast<GroupMatchMethod>(this->anyAllComboBox->currentData().value<integer64>()));
    auto number = this->ruleLayout->count();
    for (int i = 0; i < number; ++i) {
        auto w = this->ruleLayout->itemAt(i);
        auto rule = dynamic_cast<RuleWidget*>(w->widget());
        if (rule != nullptr) {
            rule->saveToRules();
        }
    }
}

void RuleGroupWidget::setupUi()
{
    verticalLayout = new QVBoxLayout{this};
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    verticalLayout->setMargin(4);
    verticalLayout->setSpacing(0);
    this->setLayout(verticalLayout);

    horizontalLayout = new QHBoxLayout;
    horizontalLayout->setObjectName("horizontalLayout");
    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setSpacing(4);
    horizontalLayout->setMargin(0);

    matchLabel = new QLabel{this};
    matchLabel->setObjectName("matchLabel");
    matchLabel->setText(QObject::tr("Match"));

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(matchLabel->sizePolicy().hasHeightForWidth());

    matchLabel->setSizePolicy(sizePolicy);
    matchLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    horizontalLayout->addWidget(matchLabel);

    anyAllComboBox = new QComboBox{this};
    anyAllComboBox->setObjectName("anyAllComboBox");
    anyAllComboBox->addItem(QObject::tr("All"), static_cast<integer64>(GroupMatchMethod::All));
    anyAllComboBox->addItem(QObject::tr("Any"), static_cast<integer64>(GroupMatchMethod::Any));

    horizontalLayout->addWidget(anyAllComboBox);

    ofTheRulesLabel = new QLabel{this};
    ofTheRulesLabel->setText(QObject::tr("of the rules:"));
    ofTheRulesLabel->setObjectName("ofTheRulesLabel");

    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(ofTheRulesLabel->sizePolicy().hasHeightForWidth());
    ofTheRulesLabel->setSizePolicy(sizePolicy1);

    horizontalLayout->addWidget(ofTheRulesLabel);

    horizontalSpacer = new QSpacerItem{20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum};
    horizontalLayout->addItem(horizontalSpacer);

    addButton = new QPushButton{this};
    addButton->setText(QObject::tr("New Rule"));
    horizontalLayout->addWidget(addButton);

    verticalLayout->addLayout(horizontalLayout);

    this->ruleContainer = new QWidget{this};
    this->ruleLayout = new QVBoxLayout{this->ruleContainer};
    this->ruleLayout->setObjectName("ruleLayout");
    this->ruleLayout->setSizeConstraint(QLayout::SetMinimumSize);
    this->ruleLayout->setMargin(4);
    this->ruleLayout->setSpacing(0);
    this->ruleContainer->setObjectName("RuleGroup");
    this->ruleContainer->setLayout(ruleLayout);
    verticalLayout->addWidget(this->ruleContainer);
}

void RuleGroupWidget::ruleSwitched(RuleWidget* switchingRuleWidget, const ChoiceData& newRule)
{
    // we must change the underlying data, get the pointer to the new element at that position and update the widget
    auto newPredicate = this->getItem()->switchPredicate(switchingRuleWidget->getRule(), newRule);
    switchingRuleWidget->setPredicate(newPredicate);
}

void RuleGroupWidget::ruleDeleted(const RuleWidget* deletedWidget)
{
    NXA_ASSERT_NOT_NULL(deletedWidget);
    disconnect(deletedWidget, &RuleWidget::ruleDeleted, this, &RuleGroupWidget::ruleDeleted);
    disconnect(deletedWidget, &RuleWidget::ruleSwitched, this, &RuleGroupWidget::ruleSwitched);
    this->getItem()->removePredicate(deletedWidget->getRule());
    delete deletedWidget;
}

void RuleGroupWidget::addRule(std::unique_ptr<AbstractPredicateItem> rule)
{
    auto widget = new RuleWidget{this->getItem(), rule.get(), this->ruleContainer};
    connect(widget, &RuleWidget::ruleDeleted, this, &RuleGroupWidget::ruleDeleted);
    connect(widget, &RuleWidget::ruleSwitched, this, &RuleGroupWidget::ruleSwitched);
    this->getItem()->addPredicate(std::move(rule));
    this->ruleLayout->addWidget(widget);
}

void RuleGroupWidget::clickAddRule()
{
    this->addRule(this->getItem()->createDefaultElement(ChoiceData{}));
}
