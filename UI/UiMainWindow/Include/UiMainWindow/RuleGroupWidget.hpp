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

#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>
#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>
#include <UiMainWindow/BaseRuleWidget.hpp>

#include <Base/Types.hpp>
#include <qframe.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <memory>

class QComboBox;
class QHBoxLayout;
class QLabel;
class QObject;
class QPushButton;
class QSpacerItem;
class QVBoxLayout;
class QWidget;

namespace NxA::RekordBuddy {

class RuleWidget;
class AbstractGroupItem;

class RuleGroupWidget : public ItemRuleWidget<AbstractGroupItem>
{
    Q_OBJECT
    void setupUi();

public:
    RuleGroupWidget(NotNull<AbstractPredicateItem*> withItem, QWidget* parent);
    void loadFromCurrentItem() override;
    void saveToRules() override;
    NotNull<AbstractGroupItem*> getGroup();

protected:
    QSpacerItem* horizontalSpacer;
    QVBoxLayout* verticalLayout;
    QVBoxLayout* ruleLayout;
    QPushButton* addButton;
    QHBoxLayout* horizontalLayout;
    QLabel* matchLabel;
    QWidget* ruleContainer;
    QComboBox* anyAllComboBox;
    QLabel* ofTheRulesLabel;

public slots:
    void ruleDeleted(const RuleWidget* deletedWidget);
    void ruleSwitched(RuleWidget* switchingRuleWidget, const ChoiceData& newRule);

public:
    void clickAddRule();
    void addRule(std::unique_ptr<AbstractPredicateItem> rule);
};
}
