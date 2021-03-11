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

#include <Base/NotNull.hpp>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qwidget.h>
#include <memory>

class QObject;
class QPushButton;
class QSpacerItem;
class QVBoxLayout;

namespace NxA::RekordBuddy {

class RuleGroupWidget;
class AbstractGroupItem;

class PredicateWidget : public QWidget
{
    Q_OBJECT
    void setupUi(NotNull<AbstractGroupItem*> item);

public:
    explicit PredicateWidget(NotNull<AbstractGroupItem*> withItem, QWidget* parent = nullptr);
    ~PredicateWidget() override = default;

    void saveToRules();
    NotNull<AbstractGroupItem*> getPredicatesAsGroup();

protected:
    RuleGroupWidget* rules;
    QVBoxLayout* verticalLayout;
    QWidget* verticalLayoutWidget;
    QVBoxLayout* ruleLayout;
    QSpacerItem* verticalSpacer;
    QPushButton* addButton;
};
}
