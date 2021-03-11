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

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>

#include <UiModel/TrackListModel.hpp>

#include <Widgets/TrackListDetailsView.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/Assert.hpp>
#include <Base/Threading.hpp>
#include <Base/Types.hpp>

#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QDate>
#include <QAbstractItemModel>
#include <QCoreApplication>

using namespace NxA;
using namespace RekordBuddy;

// -- Shared globals

static constexpr auto tracklistMimeType = "application/x-nxa-Array<AbstractTrackItem*>*";

// -- Constructors & Destructors

TrackListModel::TrackListModel(WorkQueue& withWorkQueue,
                               QObject* parent) : QAbstractItemModel{ parent },
                                                  p_workQueue{ withWorkQueue } { }

// -- Instance Methods

QModelIndex TrackListModel::indexForItem(NotNull<const AbstractTrackListNode*> item) const
{
    if (!item->hasParentItem()) {
        return this->createIndex(-1, -1, item.constCast<AbstractTrackListNode*>().forceAs<void*>().get());
    }
    else {
        return this->createIndex(*maybeNarrowCast<int>(item->getRowInParent()),
                                 0,
                                 item.constCast<AbstractTrackListNode*>().forceAs<void*>().get());
    }
}

QModelIndex TrackListModel::indexForItem(NotNull<AbstractTrackListNode*> item)
{
    if (!item->hasParentItem()) {
        return this->createIndex(-1, -1, item.forceAs<void*>().get());
    }
    else {
        return this->createIndex(*maybeNarrowCast<int>(item->getRowInParent()),
                                 0,
                                 item.forceAs<void*>().get());
    }
}

NotNull<const AbstractTrackListNode*> TrackListModel::itemFromIndex(const QModelIndex& index) const
{
    if (index.internalPointer() == nullptr) {
        NXA_ASSERT_TRUE(this->p_maybeTrackTree.isValid());
        return { const_cast<AbstractTrackListNode*>((*this->p_maybeTrackTree).asRawPointer()) };
    }

    return { static_cast<AbstractTrackListNode*>(index.internalPointer()) };
}

NotNull<AbstractTrackListNode*> TrackListModel::itemFromIndex(const QModelIndex& index)
{
    if (index.internalPointer() == nullptr) {
        NXA_ASSERT_TRUE(this->p_maybeTrackTree.isValid());
        return { const_cast<AbstractTrackListNode*>((*this->p_maybeTrackTree).asRawPointer()) };
    }

    return { static_cast<AbstractTrackListNode*>(index.internalPointer()) };
}

Qt::ItemFlags TrackListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    // -- We don't want to be selectable or enabled for drag and drop by default.
    flags ^= Qt::ItemIsSelectable;
    flags ^= Qt::ItemIsDropEnabled;
    flags ^= Qt::ItemIsDragEnabled;

    if (this->p_maybeTrackTree.isValid()) {
        auto item = this->itemFromIndex(index);
        if (item.get() == (*this->p_maybeTrackTree).asRawPointer()) {
            if (index.row() >= 0) {
                // -- We are in-between items so this is a drop spot
                flags = flags | Qt::ItemIsDropEnabled;
            }
        }
        else if (item->childCount() == 0) {
            // -- This is an actual track item, so we can drag it.
            flags = flags | Qt::ItemIsDragEnabled;

            // -- Apparently we need to be able to select in order to drag.
            flags = flags | Qt::ItemIsSelectable;
        }
    }

    return flags;
}

QModelIndex TrackListModel::index(int row, int column, const QModelIndex& parentIndex) const
{
    if ((row < 0) || (column < 0)) {
        return this->createIndex(row, column, parentIndex.internalPointer());
    }

    if (!this->hasIndex(row, column, parentIndex)) {
        return { };
    }

    auto parentItem = this->itemFromIndex(parentIndex).get();
    return this->createIndex(row, column, const_cast<AbstractTrackListNode*>(parentItem->getChildAtIndex(row).asRawPointer()));
}

int TrackListModel::rowCount(const QModelIndex& parent) const
{
    if (!this->p_maybeTrackTree.isValid()) {
        return 0;
    }

    auto parentNode = this->itemFromIndex(parent);
    return *maybeNarrowCast<int>(parentNode->childCount());
}

bool TrackListModel::hasChildren(const QModelIndex& parent) const
{
    if (!this->p_maybeTrackTree.isValid()) {
        return false;
    }

    auto parentNode = this->itemFromIndex(parent);
    return parentNode->childCount() != 0;
}

QModelIndex TrackListModel::parent(const QModelIndex& index) const
{
    auto childItem = this->itemFromIndex(index);
    if (!childItem->hasParentItem()) {
        return { };
    }

    return this->indexForItem(*childItem->maybeParent());
}

QMimeData* TrackListModel::mimeData(const QModelIndexList& indexes) const
{
    if (this->p_workQueue.isWorking()) {
        return nullptr;
    }

    auto& maybeList = NxADetail::maybeDraggedTrackObjects;
    if (!maybeList.isValid()) {
        maybeList = Shared<MutableArray<NxADetail::TrackType>>::with();
    }

    auto list = maybeList->asRawPointer();
    list->removeAll();

    MutableSet<NotNull<AbstractTrackListNode*>> nodesFound;

    for (const QModelIndex& index : indexes) {
        auto* node = static_cast<AbstractTrackListNode*>(index.internalPointer());
        if (node == nullptr) {
            continue;
        }

        NotNull<AbstractTrackListNode*> noteFound{ node };
        if (nodesFound.addingObjectCausedAnInsertion(noteFound)) {
            auto maybeTrackItem = noteFound->maybeTrackItem();
            if (!maybeTrackItem.isValid()) {
                continue;
            }

            list->emplaceAppend(index, *maybeTrackItem);
        }
    }

    if (list->length() == 0) {
        return nullptr;
    }

    QByteArray data;
    data.setNum(reinterpret_cast<qulonglong>(list));

    auto* mimeData = new QMimeData;
    mimeData->setData(tracklistMimeType, data);

    return mimeData;
}

bool TrackListModel::canDropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
    if (this->p_workQueue.isWorking()) {
        return false;
    }

    if ((row < 0) || !mimeData->hasFormat(tracklistMimeType)) {
        // -- We only support dropping tracks in between tracks
        return false;
    }

    auto parentItem = this->itemFromIndex(parent);
    return parentItem->canUserOrderTracks();
}

bool TrackListModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parentIndex)
{
    NXA_ASSERT_TRUE(row >= 0);
    NXA_ASSERT_TRUE(mimeData->hasFormat(tracklistMimeType));

    auto maybeDraggedTracks = NxADetail::maybeDraggedTrackObjects;
    if (!maybeDraggedTracks.isValid()) {
        return false;
    }

    auto data = mimeData->data(tracklistMimeType);
    auto droppedTracks = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(data.toULongLong());
    auto originalDragged = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(maybeDraggedTracks->asRawPointer());
    if ((droppedTracks == nullptr) || (originalDragged == nullptr) || (*originalDragged != *droppedTracks) || (droppedTracks->length() == 0)) {
        return false;
    }

    this->p_workQueue.enqueueJob(droppedTracks->length(), [this, row, parentIndex, droppedTracks](auto job) {
        job->updateStatus(QString("Moving %1 tracks...").arg(droppedTracks->length()));

        MutableArray<count> trackIndices;
        for (auto&& [index, track] : *droppedTracks) {
            auto trackRow = index.row();
            NXA_ASSERT_TRUE(trackRow >= 0);

            trackIndices.append(static_cast<count>(trackRow));
        }

        this->beginResetModel();
        auto parentItem = this->itemFromIndex(parentIndex);

        parentItem->moveTracksAtIndicesToIndex(trackIndices, row);

        this->endResetModel();

        this->notifyLayoutChangedAt(parentIndex);

        job->setProgress(droppedTracks->length());

        NxADetail::maybeDraggedTrackObjects = nothing;
    });

    return (droppedTracks->length() > 0u);
}

Qt::DropActions TrackListModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

Qt::DropActions TrackListModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QStringList TrackListModel::mimeTypes() const
{
    return QStringList() << tracklistMimeType;
}

void TrackListModel::beginMoveRows(const QModelIndex& fromParentIndex, int fromRow, int fromLast,
                                   const QModelIndex& toParentIndex, int toRow)
{
    Threading::runOnMainThread([this, fromParentIndex, fromRow, fromLast, toParentIndex, toRow]() {
        this->QAbstractItemModel::beginMoveRows(fromParentIndex, fromRow, fromLast,
                                                toParentIndex, toRow);
    }, Threading::Blocking::Yes);
}

void TrackListModel::endMoveRows()
{
    Threading::runOnMainThread([this]() {
        this->QAbstractItemModel::endMoveRows();
    }, Threading::Blocking::Yes);
}

bool TrackListModel::removeRows(int row, int numberOfRowsToRemove, const QModelIndex& parentIndex)
{
    if ((numberOfRowsToRemove < 1) || (row < 0) || ((row + numberOfRowsToRemove) > this->rowCount(parentIndex)) || parentIndex.isValid()) {
        return false;
    }

    if (!this->p_maybeTrackTree.isValid()) {
        return false;
    }

    auto maybeCrate = (*this->p_maybeTrackTree)->maybeCrateItem();
    if (!maybeCrate.isValid()) {
        return false;
    }

    NXA_ASSERT_TRUE((*maybeCrate)->canUserRemoveTracks());

    this->beginRemoveRows(parentIndex, row, row + numberOfRowsToRemove - 1);

    for (count index = 0; index < *maybeNarrowCast<count>(numberOfRowsToRemove); ++index) {
        (*maybeCrate)->removeTrackAtIndex(row);

        (*this->p_maybeTrackTree)->refreshTrackList();
    }

    this->endRemoveRows();

    this->notifyLayoutChangedAt(parentIndex);

    return true;
}

void TrackListModel::setCurrentPlaylistBeingDisplayed(Optional<Pointer<Common::MutablePlaylist>>& maybePlaylist)
{
    this->p_maybeCurrentPlaylistBeingViewed = maybePlaylist;
}

Optional<NotNull<const AbstractTrackItem*>> TrackListModel::maybeTrackAtIndex(const QModelIndex& index) const
{
    NXA_ASSERT_TRUE(index.isValid());
    NXA_ASSERT_TRUE(this->p_maybeTrackTree.isValid());

    auto* asTrackListNode = static_cast<const AbstractTrackListNode*>(index.internalPointer());
    NXA_ASSERT_NOT_NULL(asTrackListNode);

    return asTrackListNode->maybeTrackItem();
}

Optional<NotNull<AbstractTrackItem*>> TrackListModel::maybeTrackAtIndex(const QModelIndex& index)
{
    NXA_ASSERT_TRUE(index.isValid());
    NXA_ASSERT_TRUE(this->p_maybeTrackTree.isValid());

    auto* asTrackListNode = static_cast<AbstractTrackListNode*>(index.internalPointer());
    NXA_ASSERT_NOT_NULL(asTrackListNode);

    return asTrackListNode->maybeTrackItem();
}

Shared<AbstractTrackListNode> TrackListModel::getTrackTree()
{
    NXA_ASSERT_TRUE(this->p_maybeTrackTree.isValid());
    return *this->p_maybeTrackTree;
}

void TrackListModel::setTrackTree(const Shared<AbstractTrackListNode>& trackTree)
{
    this->beginResetModel();

    this->p_maybeTrackTree = trackTree;
    (*this->p_maybeTrackTree)->refreshTrackList();

    this->endResetModel();
}

QVariant TrackListModel::columnDisplayData(const QModelIndex& index, int dataIndex) const
{
    if (!index.isValid()) {
        return { };
    }

    if (!this->p_maybeTrackTree.isValid()) {
        return { };
    }

    if (dataIndex == TrackListUserDataIndex::IndexNumber) {
        return index.row() + 1;
    }

    Optional<NotNull<AbstractTrackItem*>> maybeTrack;
    auto* tracknode = static_cast<AbstractTrackListNode*>(index.internalPointer());
    if (tracknode != nullptr) {
        maybeTrack = tracknode->maybeTrackItem();
        if (!maybeTrack.isValid()) {
            if (dataIndex == TrackListUserDataIndex::HeaderText) {
                return tracknode->getSectionName();
            }

            if (dataIndex == TrackListUserDataIndex::IsAHeader) {
                return true;
            }

            return { };
        }
    }

    if (this->p_workQueue.isWorking()) {
        // -- Tracklist should never end up being updated during a job on the work queue but
        // -- we leave this here just in case so that users don't get crashes.
        //NXA_ALOG_DEBUG("Updating tracks!!!\n");
        return { };
    }

    if (maybeTrack.isValid()) {
        switch (dataIndex) {
            case Qt::UserRole: {
                // -- This is used as source during drags.
                return (*maybeTrack)->getTrackTitle();
            }
            case enumToInteger(Common::Track::Property::Title): {
                return (*maybeTrack)->getTrackTitle();
            }
            case enumToInteger(Common::Track::Property::Artists): {
                return (*maybeTrack)->getTrackArtistsAsString();
            }
            case enumToInteger(Common::Track::Property::Remixers): {
                return (*maybeTrack)->getTrackRemixersAsString();
            }
            case enumToInteger(Common::Track::Property::Producers): {
                return (*maybeTrack)->getTrackProducersAsString();
            }
            case enumToInteger(Common::Track::Property::Comments): {
                return (*maybeTrack)->getTrackComments();
            }
            case enumToInteger(Common::Track::Property::Tags): {
                return (*maybeTrack)->getTrackTagsAsString();
            }
            case enumToInteger(Common::Track::Property::MusicalKeys): {
                return (*maybeTrack)->getTrackKeysAsString();
            }
            case enumToInteger(Common::Track::Property::Genres): {
                return (*maybeTrack)->getTrackGenresAsString();
            }
            case enumToInteger(Common::Track::Property::RecordLabel): {
                return (*maybeTrack)->getTrackRecordLabel();
            }
            case enumToInteger(Common::Track::Property::Rating): {
                return fromString((*maybeTrack)->getTrackRating().asString());
            }
            case enumToInteger(Common::Track::Property::MixName): {
                return (*maybeTrack)->getTrackMixName();
            }
            case enumToInteger(Common::Track::Property::PlayCount): {
                auto value = (*maybeTrack)->getTrackPlayCount();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::BeatsPerMinute): {
                return (*maybeTrack)->getTrackBeatsPerMinute();
            }
            case enumToInteger(Common::Track::Property::Length): {
                auto hms = (*maybeTrack)->getTrackLength().toHoursMinutesSeconds();
                return QObject::tr("%1:%2:%3").arg(hms.hours, 2, 10, QLatin1Char('0')).arg(hms.minutes, 2, 10, QLatin1Char('0')).arg(hms.seconds, 2, 10, QLatin1Char('0'));
            }
            case enumToInteger(Common::Track::Property::BitRate): {
                auto value = (*maybeTrack)->getTrackBitRate();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::BitDepth): {
                auto value = (*maybeTrack)->getTrackBitDepth();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::FileLocation): {
                return (*maybeTrack)->getTrackAbsoluteFilePath();
            }
            case enumToInteger(Common::Track::Property::SampleRate): {
                auto value = (*maybeTrack)->getTrackSampleRate();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::DateAdded): {
                return (*maybeTrack)->getTrackDateAdded();
            }
            case enumToInteger(Common::Track::Property::DateModified): {
                return (*maybeTrack)->getTrackTimeLastModified();
            }
            case enumToInteger(Common::Track::Property::DateReleased): {
                return (*maybeTrack)->getTrackDateReleased();
            }
            case enumToInteger(Common::Track::Property::AlbumTitle): {
                return (*maybeTrack)->getTrackAlbum();
            }
            case enumToInteger(Common::Track::Property::TrackCount): {
                auto value = (*maybeTrack)->getTrackCount();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::TrackNumber): {
                auto value = (*maybeTrack)->getTrackNumber();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::Grouping): {
                return (*maybeTrack)->getTrackGrouping();
            }
            case enumToInteger(Common::Track::Property::FileType): {
                return (*maybeTrack)->getTrackFileType();
            }
            case enumToInteger(Common::Track::Property::FileSize): {
                auto value = (*maybeTrack)->getTrackFileSize();
                return (value == 0) ? QString{ } : QObject::tr("%1").arg(value);
            }
            case enumToInteger(Common::Track::Property::AlbumArt): {
                return (*maybeTrack)->getTrackArtwork();
            }
            case TrackListUserDataIndex::IsAHeader: {
                return false;
            }
            default: {
                // If asking for a user data index and we don't support it, we error out in debug builds.
                NXA_ALOG_DEBUG("TrackListModel is being asked for data it doesn't support.");
            }
        }
    }

    return { };
}

QVariant TrackListModel::data(const QModelIndex& index, int role) const
{
    static QFont mainFont{ "Inter UI", NXA_SMALL_FONT, QFont::Normal, false };
    if (role == Qt::FontRole) {
        return mainFont;
    }

    if (role == Qt::BackgroundRole && index.data(TrackListUserDataIndex::IsAHeader).toBool()) {
        QSize headerSize = TrackListDetailsView::GetHeaderDimensions();
        QLinearGradient gradient({ 0, 0 }, { 0, qreal(headerSize.height()) });
        gradient.setColorAt(0, QColor(125, 125, 125));
        gradient.setColorAt(1, QColor(44, 44, 44));
        return QBrush(gradient);
    }

    return { };
}

QVariant TrackListModel::dataHeader(const QModelIndex& index, int role) const
{
    return { };
}

QVariant TrackListModel::dataTrack(const QModelIndex& index, int role) const
{
    return { };
}

void TrackListModel::notifyLayoutChangedAt(QModelIndex alteredIndex)
{
    Threading::runOnMainThread([this, alteredIndex]() {
        QList<QPersistentModelIndex> itemsIndices{ alteredIndex };
        emit layoutChanged(itemsIndices);
    }, Threading::Blocking::Yes);
}
