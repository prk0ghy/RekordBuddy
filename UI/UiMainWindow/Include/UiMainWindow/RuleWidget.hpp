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

#include <Base/Types.hpp>
#include <UiMainWindow/BaseRuleWidget.hpp>
#include <UiModel/RuleListModel.hpp>

#include <qframe.h>
#include <qobjectdefs.h>
#include <qstring.h>

class QComboBox;
class QHBoxLayout;
class QObject;
class QPushButton;
class QVBoxLayout;
class QWidget;

namespace NxA::RekordBuddy {

class AbstractGroupItem;
class AbstractPredicateItem;

class RuleWidget : public QFrame
{
    Q_OBJECT

    void setupUi();

    QVBoxLayout* verticalLayout{nullptr};
    QHBoxLayout* horizontalLayout{nullptr};
    QComboBox* selectorCombo{nullptr};
    BaseRuleWidget* childWidget{nullptr};
    BaseRuleWidget* subchildWidget{nullptr};
    QPushButton* deleteAction{nullptr};
    NotNull<AbstractPredicateItem*> rule;
    RuleListModel ruleListModel;

private slots:
    void deleteClicked();
    void switchRule(int currentRule);

public:
    RuleWidget(NotNull<AbstractGroupItem*> inGroup, NotNull<AbstractPredicateItem*> withItem, QWidget* parent);
    virtual NotNull<AbstractPredicateItem*> getRule() const;
    void setPredicate(NotNull<AbstractPredicateItem*> predicate);
    ~RuleWidget() override = default;
    void saveToRules();

signals:
    void ruleDeleted(const RuleWidget* rule);
    void ruleSwitched(RuleWidget* rule, const ChoiceData& newRule);
};
}
