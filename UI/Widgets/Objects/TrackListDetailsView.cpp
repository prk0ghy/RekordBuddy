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
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <UiModel/TrackListSortingModel.hpp>
#include <UiModel/TrackListDetailsModel.hpp>

#include <UiBase/Types.hpp>

#include <Widgets/TrackListDetailsView.hpp>

#include <Base/Types.hpp>
#include <Base/Utilities.hpp>
#include <Base/Volume.hpp>

#include <QDropEvent>
#include <QWidget>

#include <qheaderview.h>
#include <qmenu.h>

#include "Widgets/moc_TrackListDetailsView.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Static Methods

QSize TrackListDetailsView::GetHeaderDimensions()
{
    return { 740, 24 };
}

QSize TrackListDetailsView::GetTrackDimensions()
{
    return { 740, 28 };
}

// -- Constructors & Destructors

TrackListDetailsView::TrackListDetailsView(QWidget* parent) : QTreeView{ parent }
{
    auto tracklistHeader = header();
    tracklistHeader->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tracklistHeader, &QWidget::customContextMenuRequested, this, &TrackListDetailsView::showContextMenu);
    connect(tracklistHeader, &QHeaderView::sectionResized, this, &TrackListDetailsView::columnResized);
    connect(tracklistHeader, &QHeaderView::sectionMoved, this, &TrackListDetailsView::columnMoved);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
    setSortingEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
}

// -- Instance Methods

void TrackListDetailsView::showContextMenu(QPoint position)
{
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    QMenu contextMenu{ QObject::tr("Context menu"), this };
    detailsModel->populateHeaderContextMenu(this, contextMenu);
    contextMenu.exec(this->mapToGlobal(position));
}

String TrackListDetailsView::columnWidthIDAtIndex(count index) const
{
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    return String::stringWithFormat("%s_WIDTH", detailsModel->columnIDAtIndex(index).asUTF8());
}

String TrackListDetailsView::columnWidthIDWithUserDataIndex(count dataIndex) const
{
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    return String::stringWithFormat("%s_WIDTH", detailsModel->columnIDWithUserDataIndex(dataIndex).asUTF8());
}

String TrackListDetailsView::columnHiddenIDAtIndex(count index) const
{
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    return String::stringWithFormat("%s_HIDDEN", detailsModel->columnIDAtIndex(index).asUTF8());
}

void TrackListDetailsView::columnMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    if ((oldVisualIndex >= 0) && (newVisualIndex >= 0)) {
        detailsModel->columnsReOrdered(*maybeNarrowCast<count>(oldVisualIndex), *maybeNarrowCast<count>(newVisualIndex));
    }
}

void TrackListDetailsView::columnResized(int column, int oldSize, int newSize)
{
    auto maybeColumnIndex = maybeNarrowCast<count>(column);
    if (!maybeColumnIndex.isValid()) {
        return;
    }

    if (newSize != 0) {
        // -- We only save the size if it's not 0 because Qt uses 0 when hiding a column.
        auto columnID = this->columnWidthIDAtIndex(*maybeColumnIndex);
        RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume())->setIntegerForKey(newSize, columnID);
    }

    this->QTreeView::columnResized(column, oldSize, newSize);
}

void TrackListDetailsView::toggleColumnVisible()
{
    auto menuItem = dynamic_cast<QAction*>(sender());
    if (menuItem != nullptr) {
        QVariant userData = menuItem->data();
        boolean validData;
        integer columnIndex = userData.toInt(&validData);
        if (validData && columnIndex >= 0 && columnIndex < model()->columnCount()) {
            auto isHidden = !this->isColumnHidden(columnIndex);
            this->setColumnHidden(columnIndex, isHidden);

            RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume())->setBooleanForKey(isHidden,
                                                                                            this->columnHiddenIDAtIndex(columnIndex));
        }
    }
}

void TrackListDetailsView::keyPressEvent(QKeyEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
#if defined(NXA_PLATFORM_WINDOWS)
        if (keyEvent->key() == Qt::Key_Delete) {
#elif defined(NXA_PLATFORM_MACOS)
        if (keyEvent->key() == Qt::Key_Backspace) {
#else
        #error Unsupported platform.
#endif

            this->setDisabled(true);

            auto maybeCrate = this->p_maybeCurrentlyDisplayedCrate;
            if (maybeCrate.isValid() && (*maybeCrate)->canUserRemoveTracks() && (*maybeCrate)->maybeConfirmTrackDeletion()) {
                Optional<QModelIndex> maybeParentIndex;

                MutableSet<count> rowsSelected;
                for (auto&& index : this->selectedIndexes()) {
                    if (!maybeParentIndex.isValid()) {
                        maybeParentIndex = index.parent();
                    }

                    auto row = index.row();
                    if (row >= 0) {
                        rowsSelected.add(row);
                    }
                }

                if (maybeParentIndex.isValid()) {
                    MutableArray<count> sortedRows{ std::move(rowsSelected) };
                    sortedRows.sort();

                    count numberAlreadyDeleted = 0;
                    for (auto&& row : sortedRows) {
                        this->model()->removeRows(row - numberAlreadyDeleted++, 1, *maybeParentIndex);
                    }

                    this->clearSelection();
                }
            }

            this->setDisabled(false);
        }

        // -- Handled
        return;
    }

    // -- Unhandled inputs go to default handling
    return QTreeView::keyPressEvent(event);
}

void TrackListDetailsView::setModel(QAbstractItemModel* model)
{
    this-> QTreeView::setModel(model);

    // -- We set default values for a few columns, the rest will be sized bases on content for now.
    auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());
    userPreferences->setDefaultIntegerForKey(50, this->columnWidthIDWithUserDataIndex(enumToInteger(TrackListDetailsModel::TrackListUserDataIndex::IndexNumber)));
    userPreferences->setDefaultIntegerForKey(350, this->columnWidthIDWithUserDataIndex(enumToInteger(Common::Track::Property::Title)));

    // -- We can restore any hidden columns based on user preferences.
    auto proxyModel = dynamic_cast<TrackListSortingModel*>(this->model());
    auto detailsModel = dynamic_cast<TrackListDetailsModel*>(proxyModel->sourceModel());
    NXA_ASSERT_NOT_NULL(detailsModel);

    for (count columnIndex = 0; columnIndex < detailsModel->numberOfColumns(); ++columnIndex) {
        auto maybeHidden = userPreferences->maybeBooleanForKey(this->columnHiddenIDAtIndex(columnIndex));
        this->setColumnHidden(columnIndex, maybeHidden.isValid() && *maybeHidden);
    }
}
