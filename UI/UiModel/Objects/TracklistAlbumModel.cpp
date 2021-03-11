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

#include <UiModel/TrackListAlbumModel.hpp>
#include "WorkQueue/WorkQueue.hpp"
#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>
#include <Base/Assert.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

TrackListAlbumModel::TrackListAlbumModel(WorkQueue& withWorkQueue, QObject* parent) : TrackListModel{withWorkQueue, parent} {}

// -- Overridden Instance Methods

int TrackListAlbumModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant TrackListAlbumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant{};
}
