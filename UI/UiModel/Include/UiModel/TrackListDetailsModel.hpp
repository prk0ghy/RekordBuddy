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

#include <UiModel/TrackListModel.hpp>

#include <Base/Types.hpp>
#include <Base/Array.hpp>
#include <Base/NotNull.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class AbstractTrackItem;
class AbstractTrackListItem;
class TrackListDetailsView;
class WorkQueue;

class TrackListDetailsModel : public TrackListModel
{
    // -- Private Types
    struct ColumnDefinition
    {
        QString name;
        count userDataIndex;

        ColumnDefinition(QString withName,
                         count withIndex) : name{ std::move(withName) },
                                            userDataIndex{ withIndex } { }
    };

    // -- Private Instance Variables
    MutableArray<ColumnDefinition> p_columnDefinitions;
    MutableArray<integer> p_columnDataIndicesAsDisplayed;

public:
    // -- Constructors & Destructors
    TrackListDetailsModel() = delete;
    TrackListDetailsModel(WorkQueue& queue, QObject* parent);
    ~TrackListDetailsModel() override = default;

    // -- Instance Methods
    void populateHeaderContextMenu(NotNull<TrackListDetailsView*> owner, QMenu& contextMenu) const;

    count numberOfColumns() const
    {
        return this->p_columnDefinitions.length();
    }
    void columnsReOrdered(count, count);
    String columnIDWithUserDataIndex(count userDataIndex)
    {
        return String::stringWithFormat("NXA_DETAIL_COLUMN_%d", userDataIndex);
    }
    String columnIDAtIndex(count index)
    {
        return this->columnIDWithUserDataIndex(this->p_columnDefinitions[index].userDataIndex);
    }
    void saveColumnsOrder();

    // -- Overridden Instance Methods
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

} }
