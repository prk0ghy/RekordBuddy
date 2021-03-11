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
#include <Base/Types.hpp>
#include <QAbstractListModel>

namespace NxA::RekordBuddy {

class RuleListModel : public QAbstractListModel
{
    QList<ChoiceData> rules;

public:
    RuleListModel() = delete;
    ~RuleListModel() override = default;

    explicit RuleListModel(NotNull<AbstractGroupItem*> withItem) : rules{withItem->getPossibleRules()} {}
    ChoiceData getChoice(integer64) const;

    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& index) const override;
};
}
