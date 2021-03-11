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

#include <UiModel/RuleListModel.hpp>

using namespace NxA;
using namespace RekordBuddy;

int RuleListModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant RuleListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return "Name";
            default:
                return {};
        }
    }
    return {};
}

ChoiceData RuleListModel::getChoice(integer64 index) const
{
    return this->rules[index];
}

int RuleListModel::rowCount(const QModelIndex& index) const
{
    return this->rules.length();
}

QVariant RuleListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (role != Qt::DisplayRole) {
        return {};
    }

    auto item = this->getChoice(index.row());

    switch (index.column()) {
        case 0: {
            return item.name;
        }
        case 1: {
            return QVariant::fromValue(item);
        }
        default:
            return {};
    }
}
