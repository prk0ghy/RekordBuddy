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

#include <UiMainWindow/RuleWidget.hpp>
#include <UiMainWindow/StringMatchWidget.hpp>
#include <UiMainWindow/DateMatchWidget.hpp>
#include <UiMainWindow/EnumerationMatchWidget.hpp>
#include <UiMainWindow/NumberMatchWidget.hpp>
#include <UiMainWindow/RuleGroupWidget.hpp>
#include <UiMainWindow/TagMatchWidget.hpp>
#include <AbstractViewCollection/Predicate/AbstractDateItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractEnumerationItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractNumberItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractStringItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractTagItem.hpp>

#include <Base/Base.hpp>

#include <qgraphicseffect.h>
#include <qabstractbutton.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qvariant.h>
#include <qwidget.h>

#include "UiMainWindow/moc_RuleWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

RuleWidget::RuleWidget(NotNull<AbstractGroupItem*> inGroup, NotNull<AbstractPredicateItem*> withItem, QWidget* parent)
    : QFrame{parent}, rule{withItem}, ruleListModel{inGroup}
{
    this->setupUi();
}

NotNull<AbstractPredicateItem*> RuleWidget::getRule() const
{
    return this->rule;
}

void RuleWidget::setPredicate(NotNull<AbstractPredicateItem*> predicate)
{
    delete this->childWidget;
    delete this->subchildWidget;

    auto choiceData = predicate->getChoiceData();
    this->rule = predicate;

    switch (choiceData.rule) {
        case PredicateRule::TagMatch: {
            this->childWidget = new TagMatchWidget{predicate, this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
        case PredicateRule::NameMatch: {
            this->childWidget = new StringMatchWidget{predicate, this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
        case PredicateRule::DateMatch: {
            this->childWidget = new DateMatchWidget{predicate, this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
        case PredicateRule::NumberMatch: {
            this->childWidget = new NumberMatchWidget{predicate, this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
        case PredicateRule::EnumerationMatch: {
            auto enumItem = predicate.maybeAs<AbstractEnumerationItem*>();
            NXA_ASSERT_TRUE(enumItem.isValid());
            auto choices = (*enumItem)->getChoices();
            this->childWidget = new EnumerationMatchWidget{choices, *enumItem, this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
        case PredicateRule::ChildRules: {
            this->childWidget = new BaseRuleWidget{this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = new RuleGroupWidget{predicate, this};
            break;
        }
        default: {
            this->childWidget = new BaseRuleWidget{this};
            horizontalLayout->insertWidget(1, this->childWidget);
            this->subchildWidget = nullptr;
            break;
        }
    }

    this->childWidget->setContentsMargins(4, 0, 0, 0);

    if (this->subchildWidget != nullptr) {
        verticalLayout->insertWidget(1, this->subchildWidget);
    }
}

void RuleWidget::switchRule(int currentRule)
{
    auto data = this->ruleListModel.getChoice(currentRule);
    this->ruleSwitched(this, data);
}

void RuleWidget::saveToRules()
{
    if (this->subchildWidget != nullptr) {
        this->subchildWidget->saveToRules();
        return;
    }

    this->childWidget->saveToRules();
}

void RuleWidget::setupUi()
{
    this->setObjectName("RuleFrame");
    auto* effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(6);
    effect->setOffset(3, 3);
    effect->setColor(QColor{0, 0, 0, 30});
    this->setGraphicsEffect(effect);

    verticalLayout = new QVBoxLayout{this};
    verticalLayout->setMargin(0);
    verticalLayout->setSpacing(0);

    horizontalLayout = new QHBoxLayout;
    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(4);
    verticalLayout->addLayout(horizontalLayout);

    selectorCombo = new QComboBox{this};
    selectorCombo->setObjectName("selectorCombo");
    selectorCombo->setModel(&ruleListModel);
    selectorCombo->setContentsMargins(4, 0, 0, 0);

    selectorCombo->setMinimumWidth(150);
    selectorCombo->setMaximumWidth(150);
    horizontalLayout->addWidget(selectorCombo);

    this->setPredicate(this->rule);

    deleteAction = new QPushButton{this};
    deleteAction->setObjectName("deleteAction");
    deleteAction->setText("✕"); // todo: pick better icon
    deleteAction->setMinimumWidth(23);
    deleteAction->setMinimumHeight(23);
    deleteAction->setMaximumWidth(23);
    deleteAction->setMaximumHeight(23);
    deleteAction->setContentsMargins(8, 8, 8, 8);

    horizontalLayout->addWidget(deleteAction);

    horizontalLayout->setAlignment(selectorCombo, Qt::AlignVCenter);
    horizontalLayout->setAlignment(childWidget, Qt::AlignVCenter);
    horizontalLayout->setAlignment(deleteAction, Qt::AlignVCenter);

    connect(deleteAction, &QPushButton::clicked, this, &RuleWidget::deleteClicked);
    connect(selectorCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RuleWidget::switchRule);
}

void RuleWidget::deleteClicked()
{
    this->ruleDeleted(this);
}
