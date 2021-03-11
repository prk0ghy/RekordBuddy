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

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>

#include <UiModel/TrackListDetailsModel.hpp>

#include <Widgets/TrackListDetailsView.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/NotNull.hpp>
#include <Base/Utilities.hpp>

#include <qmenu.h>

using namespace NxA;
using namespace RekordBuddy;

// -- Constants

#define NXA_TRACK_LIST_DETAILS_COLUMNS_ORDER_PREFERENCES_KEY                           "TrackListDetailsColumnsOrder"

// -- Constructors & Destructors

TrackListDetailsModel::TrackListDetailsModel(WorkQueue& withWorkQueue,
                                             QObject* parent) : TrackListModel{ withWorkQueue,
                                                                                parent}
{
    auto columsOrder = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume())->arrayOfIntegersForKey(String{ NXA_TRACK_LIST_DETAILS_COLUMNS_ORDER_PREFERENCES_KEY });
    if (columsOrder.length() == 0) {
        MutableArray<integer> newColumsOrder;
        newColumsOrder.append(TrackListUserDataIndex::IndexNumber);
        newColumsOrder.append(enumToInteger(Common::Track::Property::Title));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Artists));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Remixers));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Producers));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Comments));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Tags));
        newColumsOrder.append(enumToInteger(Common::Track::Property::MusicalKeys));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Genres));
        newColumsOrder.append(enumToInteger(Common::Track::Property::RecordLabel));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Rating));
        newColumsOrder.append(enumToInteger(Common::Track::Property::MixName));
        newColumsOrder.append(enumToInteger(Common::Track::Property::PlayCount));
        newColumsOrder.append(enumToInteger(Common::Track::Property::BeatsPerMinute));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Length));
        newColumsOrder.append(enumToInteger(Common::Track::Property::BitRate));
        newColumsOrder.append(enumToInteger(Common::Track::Property::BitDepth));
        newColumsOrder.append(enumToInteger(Common::Track::Property::FileLocation));
        newColumsOrder.append(enumToInteger(Common::Track::Property::SampleRate));
        newColumsOrder.append(enumToInteger(Common::Track::Property::DateAdded));
        newColumsOrder.append(enumToInteger(Common::Track::Property::DateModified));
        newColumsOrder.append(enumToInteger(Common::Track::Property::DateReleased));
        newColumsOrder.append(enumToInteger(Common::Track::Property::AlbumTitle));
        newColumsOrder.append(enumToInteger(Common::Track::Property::TrackCount));
        newColumsOrder.append(enumToInteger(Common::Track::Property::TrackNumber));
        newColumsOrder.append(enumToInteger(Common::Track::Property::Grouping));
        newColumsOrder.append(enumToInteger(Common::Track::Property::FileType));
        newColumsOrder.append(enumToInteger(Common::Track::Property::FileSize));

        columsOrder = { std::move(newColumsOrder) };
    }

    static Map<count, const character*> columnIndexToName = {
            { TrackListUserDataIndex::IndexNumber, "#" },
            { enumToInteger(Common::Track::Property::Title), "Title" },
            { enumToInteger(Common::Track::Property::Artists), "Artists" },
            { enumToInteger(Common::Track::Property::Remixers), "Remixers" },
            { enumToInteger(Common::Track::Property::Producers), "Producers" },
            { enumToInteger(Common::Track::Property::Comments), "Comments" },
            { enumToInteger(Common::Track::Property::Tags), "Tags" },
            { enumToInteger(Common::Track::Property::MusicalKeys), "Keys" },
            { enumToInteger(Common::Track::Property::Genres), "Genres" },
            { enumToInteger(Common::Track::Property::RecordLabel), "Label" },
            { enumToInteger(Common::Track::Property::Rating), "Rating" },
            { enumToInteger(Common::Track::Property::MixName), "Mix Name" },
            { enumToInteger(Common::Track::Property::PlayCount), "Play Count" },
            { enumToInteger(Common::Track::Property::BeatsPerMinute), "Bpm" },
            { enumToInteger(Common::Track::Property::Length), "Length" },
            { enumToInteger(Common::Track::Property::BitRate), "Bit Rate" },
            { enumToInteger(Common::Track::Property::BitDepth), "Bit Depth" },
            { enumToInteger(Common::Track::Property::FileLocation), "Location" },
            { enumToInteger(Common::Track::Property::SampleRate), "Sample Rate" },
            { enumToInteger(Common::Track::Property::DateAdded), "Date Added" },
            { enumToInteger(Common::Track::Property::DateModified), "Date Modified" },
            { enumToInteger(Common::Track::Property::DateReleased), "Date Released" },
            { enumToInteger(Common::Track::Property::AlbumTitle), "Album" },
            { enumToInteger(Common::Track::Property::TrackCount), "Track Count" },
            { enumToInteger(Common::Track::Property::TrackNumber), "Track Number" },
            { enumToInteger(Common::Track::Property::Grouping), "Grouping" },
            { enumToInteger(Common::Track::Property::FileType), "File Type" },
            { enumToInteger(Common::Track::Property::FileSize), "File Size" },
    };

    for (auto&& columnUserDataIndex : columsOrder) {
        auto maybeName = columnIndexToName.maybeValueForKey(columnUserDataIndex);
        if (maybeName.isValid()) {
            this->p_columnDefinitions.emplaceAppend(QObject::tr(*maybeName), columnUserDataIndex);
            this->p_columnDataIndicesAsDisplayed.append(columnUserDataIndex);
        }
    }
}

// -- Instance Methods

void TrackListDetailsModel::populateHeaderContextMenu(NotNull<TrackListDetailsView*> owner, QMenu& contextMenu) const
{
    for (count columnIndex = 0; columnIndex < p_columnDefinitions.length(); ++columnIndex) {
        auto column = &p_columnDefinitions[columnIndex];
        QAction* action = contextMenu.addAction(column->name);
        action->setCheckable(true);
        action->setChecked(!owner->isColumnHidden(*maybeNarrowCast<int>(columnIndex)));
        action->setData(columnIndex);
        connect(action, &QAction::triggered, owner.get(), &TrackListDetailsView::toggleColumnVisible);
    }
}

void TrackListDetailsModel::columnsReOrdered(count fromIndex, count toIndex)
{
    this->p_columnDataIndicesAsDisplayed.moveObjectAtIndexTo(fromIndex, toIndex);
}

void TrackListDetailsModel::saveColumnsOrder()
{
    RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume())->setArrayOfIntegersForKey({ this->p_columnDataIndicesAsDisplayed },
                                                                                            String{ NXA_TRACK_LIST_DETAILS_COLUMNS_ORDER_PREFERENCES_KEY });
}

// -- Overridden Instance Methods

int TrackListDetailsModel::columnCount(const QModelIndex& parent) const
{
    return *maybeNarrowCast<int>(p_columnDefinitions.length());
}

QVariant TrackListDetailsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (section >= 0 && section < *maybeNarrowCast<int>(p_columnDefinitions.length())) {
            return p_columnDefinitions[*maybeNarrowCast<count>(section)].name;
        }
    }

    return QVariant{ };
}

QVariant TrackListDetailsModel::data(const QModelIndex& index, int role) const
{
    if ((role == Qt::DisplayRole) && (index.column() >= 0) && (index.column() < *maybeNarrowCast<int>(this->p_columnDefinitions.length()))) {
        return this->columnDisplayData(index, this->p_columnDefinitions[*maybeNarrowCast<count>(index.column())].userDataIndex);
    }

    return this->TrackListModel::data(index, role);
}
