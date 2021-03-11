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

#include <AbstractViewCollection/Sidebar/AbstractCollection.hpp>
#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <UiModel/SidebarModel.hpp>

#include <UiBase/Types.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/Array.hpp>
#include <Base/CrashLog.hpp>
#include <Base/Threading.hpp>

#include "UiModel/moc_SidebarModel.cpp"

#include <QIcon>
#include <QFont>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QApplication>

#include <algorithm>

using namespace NxA;
using namespace RekordBuddy;

static constexpr auto tracklistMimeType = "application/x-nxa-Array<AbstractTrackItem*>*";
static constexpr auto cratelistMimeType = "application/x-nxa-Array<AbstractCrateItem*>*";
static constexpr auto collectionMimeType = "application/x-nxa-AbstractCollection*";
static constexpr auto uriListMimeType = "text/uri-list";

SidebarModel::SidebarModel(WorkQueue& withWorkQueue) : QAbstractItemModel{ nullptr },
                                                       p_workQueue{ withWorkQueue }
{
    qRegisterMetaType<QList<QPersistentModelIndex>>();
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>();
}

void SidebarModel::beginInsertRows(const QModelIndex& parentIndex, int first, int last)
{
    Threading::runOnMainThread([this, parentIndex, first, last]() {
        this->QAbstractItemModel::beginInsertRows(parentIndex, first, last);
    }, Threading::Blocking::Yes);
}

void SidebarModel::endInsertRows()
{
    Threading::runOnMainThread([this]() {
        this->QAbstractItemModel::endInsertRows();
    }, Threading::Blocking::Yes);
}

void SidebarModel::beginRemoveRows(const QModelIndex& parentIndex, int first, int last)
{
    Threading::runOnMainThread([this, parentIndex, first, last]() {
        this->QAbstractItemModel::beginRemoveRows(parentIndex, first, last);
    }, Threading::Blocking::Yes);
}

void SidebarModel::endRemoveRows()
{
    Threading::runOnMainThread([this]() {
        this->QAbstractItemModel::endRemoveRows();
    }, Threading::Blocking::Yes);
}

void SidebarModel::beginMoveRows(const QModelIndex& fromParentIndex, int fromRow, int fromLast,
                                 const QModelIndex& toParentIndex, int toRow)
{
    Threading::runOnMainThread([this, fromParentIndex, fromRow, fromLast, toParentIndex, toRow]() {
        this->QAbstractItemModel::beginMoveRows(fromParentIndex, fromRow, fromLast, toParentIndex, toRow);
    }, Threading::Blocking::Yes);
}

void SidebarModel::endMoveRows()
{
    Threading::runOnMainThread([this]() {
        this->QAbstractItemModel::endMoveRows();
    }, Threading::Blocking::Yes);
}

boolean SidebarModel::displayedItemIsnItemsOrChildOfItemsAtIndices(const QModelIndexList& indices)
{
    for (auto&& index : indices) {
        auto indexToTest = this->p_displayedIndex;

        while (indexToTest.isValid()) {
            if (indexToTest == index) {
                return true;
            }

            indexToTest = indexToTest.parent();
        }
    }

    return false;
}

int SidebarModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant SidebarModel::headerData(int, Qt::Orientation, int) const
{
    return { };
}

boolean SidebarModel::hasChangesToSave() const
{
    if (!this->p_rootItem) {
        return false;
    }

    return this->p_rootItem->hasChangesToSave();
}

QModelIndex SidebarModel::indexForItem(NotNull<const SidebarItem*> item) const
{
    if (!item->hasParentItem()) {
        return this->createIndex(-1, -1, item.constCast<SidebarItem*>().forceAs<void*>().get());
    }
    else {
        return this->createIndex(*maybeNarrowCast<int>(item->getRowInParent()),
                                 0,
                                 item.constCast<SidebarItem*>().forceAs<void*>().get());
    }
}

QModelIndex SidebarModel::indexForItem(NotNull<SidebarItem*> item) const
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

NotNull<SidebarItem*> SidebarModel::itemFromIndex(const QModelIndex& index) const
{
    if (index.internalPointer() == nullptr) {
        return { this->p_rootItem };
    }

    return { static_cast<SidebarItem*>(index.internalPointer()) };
}

void SidebarModel::maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>& tracks)
{
    this->p_rootItem->maybeUpdateBecauseTracksChanged(tracks);
}

Qt::ItemFlags SidebarModel::flags(const QModelIndex& index) const
{
    auto flags = QAbstractItemModel::flags(index);

    if (index.internalPointer() == nullptr) {
        return flags;
    }

    // -- We don't want to be selectable or enabled for drag and drop by default.
    flags ^= Qt::ItemIsSelectable;
    flags ^= Qt::ItemIsDropEnabled;
    flags ^= Qt::ItemIsDragEnabled;

    auto item = this->itemFromIndex(index);
    if (item->canBeRenamed()) {
        flags = flags | Qt::ItemIsEditable;
    }

    if (item->isSelectable()) {
        flags = flags | Qt::ItemIsSelectable;

        if (item->canBeDragged()) {
            flags = flags | Qt::ItemIsDragEnabled;
        }
    }

    if (item->canReceiveTracks() ||
        item->canReceiveCrates() ||
        item->canReceiveCollection() ||
        item->canReceiveFiles()) {
        flags = flags | Qt::ItemIsDropEnabled;
    }

    return flags;
}

int SidebarModel::rowCount(const QModelIndex& parentIndex) const
{
    if (parentIndex.column() > 0) {
        return 0;
    }

    auto parentNode = this->itemFromIndex(parentIndex);
    if (parentNode->isLoading()) {
        return 1;
    }

    return *maybeNarrowCast<int>(parentNode->childCount());
}

boolean SidebarModel::saveChangesWithProgress(std::function<void(double)>&& progressCallBack)
{
    this->p_rootItem->saveChangesWithProgress(std::move(progressCallBack));

    // -- This has no specific meaning right now.
    return true;
}

boolean SidebarModel::requestDeletionOfItemsAtIndices(const QModelIndexList& indices)
{
    MutableArray<NotNull<AbstractCrateItem*>> cratesToDelete;
    MutableArray<NotNull<AbstractCollection*>> collectionsToDelete;

    for (auto&& index : indices) {
        if (!index.isValid()) {
            continue;
        }

        auto item = this->itemFromIndex(index);
        if (item->canBeDeleted()) {
            auto maybeAsCrateItem = item.maybeAs<AbstractCrateItem*>();
            if (maybeAsCrateItem.isValid()) {
                // -- We are deleting a crate.
                cratesToDelete.append(*maybeAsCrateItem);
            }
            else {
                auto maybeAsCollection = item.maybeAs<AbstractCollection*>();
                if (maybeAsCollection.isValid()) {
                    collectionsToDelete.append(*maybeAsCollection);
                }
            }
        }
    }

    if ((cratesToDelete.length() == 0) && (collectionsToDelete.length() == 0)) {
        // -- We don't seem to have anything valid to delete.
        return false;
    }

    if (cratesToDelete.length() != 0) {
        boolean okToDelete = true;
        for (auto&& crateItem : cratesToDelete) {
            if (crateItem->needsConfirmationToDelete()) {
                auto typeName = (cratesToDelete.length() == 1) ? String::stringWithFormat("'%s'", crateItem->getName().toUtf8().data())
                                                               : "these playlists/folders"_String;
                if (!ConfirmationMessageBox::promptForConfirmOkCancel(
                        QObject::tr("Confirm Deletion"), QObject::tr("Are you sure you want to delete %1?").arg(typeName.asUTF8()),
                        QObject::tr("You can still choose not to save these changes when you exit the app."))) {
                    okToDelete = false;
                }

                // -- We just ask the user once for everything that needs deleting.
                break;
            }
        }

        if (okToDelete) {
            for (auto&& crateItem : cratesToDelete) {
                auto index = crateItem->getModelIndex();

                auto rowToRemove = index.row();
                auto parentIndex = this->parent(index);

                this->beginRemoveRows(parentIndex, rowToRemove, rowToRemove);

                auto parentItem = this->itemFromIndex(parentIndex);
                parentItem->removeChildAtIndex(rowToRemove);

                this->endRemoveRows();

                this->notifyLayoutChangedAt(parentIndex);
            }
        }
    }

    for (auto&& collectionItem : collectionsToDelete) {
        // -- We are removing a collection, the method below will offer to remove it and refresh the collection list.
        // -- For collections, since this is a big deal, we ask for every collection.
        return collectionItem->offerToRemove();
    }

    return false;
}

QModelIndex SidebarModel::index(int row, int column, const QModelIndex& parentIndex) const
{
    if ((row < 0) || (column < 0)) {
        return this->createIndex(row, column, parentIndex.internalPointer());
    }

    if (!this->hasIndex(row, column, parentIndex)) {
        return { };
    }

    auto parentItem = this->itemFromIndex(parentIndex).get();
    return this->createIndex(row, column, parentItem->getChildAtIndex(row).asRawPointer());
}

QStringList SidebarModel::mimeTypes() const
{
    return QStringList{ } << tracklistMimeType << cratelistMimeType << collectionMimeType << uriListMimeType;
}

QMimeData* SidebarModel::mimeData(const QModelIndexList& indexes) const
{
    if (this->p_workQueue.isWorking()) {
        return nullptr;
    }

    if (indexes.length() == 1) {
        // -- If we only have one index, this could be a collection.
        auto& index = indexes.first();

        if (index.column() != 0) {
            // -- The sidebar should only have items in column 0.
            return nullptr;
        }

        auto* asACollection = dynamic_cast<AbstractCollection*>(static_cast<SidebarItem*>(index.internalPointer()));
        if (asACollection != nullptr) {
            if (!asACollection->isOpened()) {
                // -- We can only drag opened collections.
                return nullptr;
            }

            QByteArray data;
            data.setNum(reinterpret_cast<qulonglong>(asACollection));

            auto* mimeData = new QMimeData;
            mimeData->setData(collectionMimeType, data);

            return mimeData;
        }
    }

    auto& maybeList = NxADetail::maybeDraggedCrateObjects;
    if (!maybeList.isValid()) {
        maybeList = Shared<MutableArray<NxADetail::CrateType>>::with();
    }
    else {
        (*maybeList)->removeAll();
    }

    auto list = maybeList->asRawPointer();

    for (const QModelIndex& index : indexes) {
        if (index.column() != 0) {
            // -- The sidebar should only have items in column 0.
            continue;
        }

        auto* asACrate = dynamic_cast<AbstractCrateItem*>(static_cast<SidebarItem*>(index.internalPointer()));
        if (asACrate != nullptr) {
            // -- We want to make sure we're not dragging the AllTracks Playlist for now. We may let this happen at a later date.
            if (!asACrate->isAnAllTracksPlaylist()) {
                list->emplaceAppend(index, asACrate);
            }
        }
    }

    if (list->length() == 0) {
        maybeList = nothing;
        return nullptr;
    }

    QByteArray data;
    data.setNum(reinterpret_cast<qulonglong>(list));

    auto* mimeData = new QMimeData;
    mimeData->setData(cratelistMimeType, data);

    return mimeData;
}

bool SidebarModel::canDropMimeData(const QMimeData* mimeData,
                                   Qt::DropAction action,
                                   int row,
                                   int column,
                                   const QModelIndex& parent) const
{
    if (p_workQueue.isWorking() || (action != Qt::CopyAction)) {
        return false;
    }

    auto receivingItem = this->itemFromIndex(parent);
    boolean isDroppingOnItem = (row == -1) && (column == -1);

    if (mimeData->hasFormat(collectionMimeType)) {
        if (!receivingItem->canReceiveCollection() ||
            (row >= 0) ||
            (column >= 0)) {
            return false;
        }

        auto maybeReceivingCollection = receivingItem.maybeAs<AbstractCollection*>();
        if (!maybeReceivingCollection.isValid()) {
            return false;
        }

        auto data = mimeData->data(collectionMimeType);
        auto droppedCollection = reinterpret_cast<AbstractCollection*>(data.toULongLong());
        if (droppedCollection == (*maybeReceivingCollection).get()) {
            return false;
        }
        if ((droppedCollection == nullptr) || !(*maybeReceivingCollection)->canReceive(droppedCollection)) {
            return false;
        }

        return true;
    }

    if (mimeData->hasFormat(cratelistMimeType)) {
        if (!receivingItem->canReceiveCrates() || !(isDroppingOnItem || ((row >= 0) && (column == 0)))) {
            return false;
        }

        auto maybeDraggedCrates = NxADetail::maybeDraggedCrateObjects;
        if (!maybeDraggedCrates.isValid()) {
            return false;
        }

        auto data = mimeData->data(cratelistMimeType);
        auto droppedCrates = reinterpret_cast<MutableArray<NxADetail::CrateType>*>(data.toULongLong());
        auto originalDragged = reinterpret_cast<MutableArray<NxADetail::CrateType>*>(maybeDraggedCrates->asRawPointer());
        if (droppedCrates == nullptr || originalDragged == nullptr || *originalDragged != *droppedCrates ||
            (droppedCrates->length() == 0u)) {
            return false;
        }

        for (auto&&[index, crate] : *droppedCrates) {
            if (isDroppingOnItem && crate->hasParentItem() && (crate->parentItem() == receivingItem.get())) {
                // -- We can't drop on an item that's already our parent, we can only move inside the parent.
                return false;
            }

            if (!receivingItem->canReceive(crate.get())) {
                return false;
            }
        }

        return true;
    }

    if (mimeData->hasFormat(tracklistMimeType)) {
        auto displayedItemInSidebar = this->itemFromIndex(this->p_displayedIndex);
        if (isDroppingOnItem && (displayedItemInSidebar == receivingItem)) {
            // -- We can't drop tracks directly on the playlist to which they are coming from
            // -- (which should be the one currently selected).
            return false;
        }

        if (!receivingItem->canReceiveTracks() || !isDroppingOnItem) {
            return false;
        }

        auto maybeDraggedTracks = NxADetail::maybeDraggedTrackObjects;
        if (!maybeDraggedTracks.isValid()) {
            return false;
        }

        auto data = mimeData->data(tracklistMimeType);
        auto droppedTracks = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(data.toULongLong());
        auto originalDragged = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(maybeDraggedTracks->asRawPointer());
        if (droppedTracks == nullptr || originalDragged == nullptr || *originalDragged != *droppedTracks || (droppedTracks->length() == 0u)) {
            return false;
        }

        for (auto&& [index, track] : *droppedTracks) {
            if (!receivingItem->canReceive(track)) {
                return false;
            }
        }

        return true;
    }

    if (mimeData->hasFormat(uriListMimeType)) {
        if (!receivingItem->canReceiveFiles()) {
            return false;
        }

        auto urls = mimeData->urls();
        NXA_ASSERT_FALSE_DEBUG(urls.empty());
        auto urlAsLocalFile = urls.first().toLocalFile();

        QFileInfo info{ urlAsLocalFile };
        if (!info.isFile() || !info.isReadable()) {
            return false;
        }

        auto asFilePath = fromLocalFile(urlAsLocalFile);

        if ((row < 0) && (column < 0)) {
            auto maybeReceivingCollection = receivingItem.maybeAs<AbstractCollection*>();
            if (!maybeReceivingCollection.isValid() || !(*maybeReceivingCollection)->isOpened()) {
                return false;
            }
        }
        else {
            auto maybeReceivingCollectionList = receivingItem.maybeAs<AbstractCollectionList*>();
            if (!maybeReceivingCollectionList.isValid() || (*maybeNarrowCast<count>(row) < receivingItem->childCount())) {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool SidebarModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parentIndex)
{
    NXA_ASSERT_TRUE(!p_workQueue.isWorking());
    NXA_ASSERT_TRUE(action == Qt::CopyAction);
    NXA_ASSERT_NOT_NULL(parentIndex.internalPointer());

    auto receivingItem = this->itemFromIndex(parentIndex);

    if (mimeData->hasFormat(collectionMimeType)) {
        NXA_ASSERT_TRUE((row == -1) || (column == -1));

        auto droppedCollection = reinterpret_cast<AbstractCollection*>(mimeData->data(collectionMimeType).toULongLong());
        if (droppedCollection == nullptr) {
            return false;
        }

        if (receivingItem->parentCollectionMustHaveTracksOnTheSameVolume()) {
            auto maybeCollectionVolume = receivingItem->maybeParentCollectionVolume();
            if (!maybeCollectionVolume.isValid()) {
                return false;
            }

            if (droppedCollection->hasTracksNotOnVolume(*maybeCollectionVolume)) {
                auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                      QObject::tr("This collection contains tracks that are not on the same volume as the destination collection."),
                                                                      QObject::tr("This is not supported by %1 collections and any such tracks will be skipped.").arg(collectionTypeName))) {
                    return false;
                }
            }
        }

        if (!receivingItem->parentCollectionAllowsMovieTracksInPlaylists()) {
            if (droppedCollection->hasMovieTracks()) {
                auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                      QObject::tr("Some of the playlists in the dragged collection contain movie tracks."),
                                                                      QObject::tr("This is not supported by %1 collections and any such tracks will not be added to the destination playlist.").arg(collectionTypeName))) {
                    return false;
                }
            }
        }

        count totalNumberOfItemsToProcess = droppedCollection->numberOfCratesAndTrackEntriesContainedWithin();
        totalNumberOfItemsToProcess += droppedCollection->numberOfTracks();

        this->p_workQueue.enqueueJob(*maybeNarrowCast<int>(totalNumberOfItemsToProcess),
                                     [this, parentIndex, receivingItem, droppedCollection, totalNumberOfItemsToProcess](auto job) {
            job->updateStatus(QString("Copying %1 crates and tracks...").arg(totalNumberOfItemsToProcess));

            auto sidebarSelectionWillNeedClearing = this->displayedItemIsnItemsOrChildOfItemsAtIndices(QModelIndexList{ parentIndex });

            auto maybeFromCollectionName = droppedCollection->maybeParentCollectionTypeName();
            auto maybeToCollectionName = receivingItem->maybeParentCollectionTypeName();
            if (maybeFromCollectionName.isValid() &&
                maybeToCollectionName.isValid() &&
                (*maybeFromCollectionName != *maybeToCollectionName)) {
                auto maybeFromVolumeFullName = droppedCollection->maybeParentCollectionFullName();
                auto maybeToVolumeFullName = receivingItem->maybeParentCollectionFullName();
                CrashLog::addBreadCrumb(String::stringWithFormat("Drag collection from %s to %s",
                                                                 maybeFromVolumeFullName.isValid() ? maybeFromVolumeFullName->toUtf8().constData() : "Unknown Volume",
                                                                 maybeToVolumeFullName.isValid() ? maybeToVolumeFullName->toUtf8().constData() : "Unknown Volume"));
            }

            receivingItem->receiveDraggedCollectionWithPerItemProgressCallBack(droppedCollection, [&job]() {
                job->incrementProgressByOne();
            });

            if (sidebarSelectionWillNeedClearing) {
                emit sidebarSelectionNeedsClearing();
            }
        });

        return true;
    }

    if (mimeData->hasFormat(cratelistMimeType)) {
        NXA_ASSERT_TRUE(((row == -1) && (column == -1)) || ((row >= 0) && (column == 0)));

        auto maybeDraggedCrates = NxADetail::maybeDraggedCrateObjects;
        if (!maybeDraggedCrates.isValid()) {
            return false;
        }

        auto data = mimeData->data(cratelistMimeType);
        auto droppedCrates = reinterpret_cast<MutableArray<NxADetail::CrateType>*>(data.toULongLong());
        auto originalDragged = reinterpret_cast<MutableArray<NxADetail::CrateType>*>(maybeDraggedCrates->asRawPointer());
        if (droppedCrates == nullptr || originalDragged == nullptr || *originalDragged != *droppedCrates || (droppedCrates->length() == 0u)) {
            return false;
        }

        boolean parentCollectionMustHaveTracksOnTheSameVolume = receivingItem->parentCollectionMustHaveTracksOnTheSameVolume();
        boolean parentCollectionAllowsMovieTracksInPlaylists = receivingItem->parentCollectionAllowsMovieTracksInPlaylists();

        if (parentCollectionMustHaveTracksOnTheSameVolume || !parentCollectionAllowsMovieTracksInPlaylists) {
            auto maybeCollectionVolume = receivingItem->maybeParentCollectionVolume();
            if (!maybeCollectionVolume.isValid()) {
                return false;
            }

            for (auto&& [index, crate] : *droppedCrates) {
                if (!parentCollectionMustHaveTracksOnTheSameVolume && parentCollectionAllowsMovieTracksInPlaylists) {
                    // -- The user must have agreed to everything by now.
                    break;
                }

                if (parentCollectionMustHaveTracksOnTheSameVolume && crate->hasTracksNotOnVolume(*maybeCollectionVolume)) {
                    auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                    if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                          QObject::tr("Some of the crates contains tracks that are not on the same volume as the destination collection."),
                                                                          QObject::tr("This is not supported by %1 collections and any such tracks will be skipped.").arg(collectionTypeName))) {
                        return false;
                    }

                    // -- The user has been notified so there's no need to test for this again.
                    parentCollectionMustHaveTracksOnTheSameVolume = false;
                }

                if (!parentCollectionAllowsMovieTracksInPlaylists && crate->hasMovieTracks()) {
                    auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                    if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                          QObject::tr("Some of the playlists in the dragged crates contain movie tracks."),
                                                                          QObject::tr("This is not supported by %1 collections and any such tracks will not be added to the destination playlist.").arg(collectionTypeName))) {
                        return false;
                    }

                    // -- The user has been notified so there's no need to test for this again.
                    parentCollectionAllowsMovieTracksInPlaylists = true;
                }
            }
        }

        count totalNumberOfItemsToProcess = 0;
        for (auto&& [index, crate] : *droppedCrates) {
            totalNumberOfItemsToProcess += crate->numberOfCratesAndTrackEntriesContainedWithin();
            totalNumberOfItemsToProcess += crate->numberOfTracks();
        }

        this->p_workQueue.enqueueJob(*maybeNarrowCast<int>(totalNumberOfItemsToProcess),
                                     [this, parentIndex, row, receivingItem, droppedCrates, totalNumberOfItemsToProcess](auto job) {
            boolean onlyEmitEventForFirstItems = true;

            job->updateStatus(QString("Copying %1 crates and tracks...").arg(totalNumberOfItemsToProcess));

            auto sidebarSelectionWillNeedClearing = this->displayedItemIsnItemsOrChildOfItemsAtIndices(QModelIndexList{ parentIndex });

            count destinationIndex = (row == -1) ? receivingItem->childCount() : row;

            MutableSet<String> namesOfCratesAlreadyDropped;
            MutableArray<count> indicesOfCratesAdded;

            for (auto&& [index, crate] : *droppedCrates) {
                NXA_ASSERT_TRUE_DEBUG(receivingItem->canReceive(crate.get()));

                if (onlyEmitEventForFirstItems) {
                    auto maybeFromCollectionName = crate->maybeParentCollectionTypeName();
                    auto maybeToCollectionName = receivingItem->maybeParentCollectionTypeName();
                    if (maybeFromCollectionName.isValid() &&
                        maybeToCollectionName.isValid() &&
                        (*maybeFromCollectionName != *maybeToCollectionName)) {
                        CrashLog::addBreadCrumb(String::stringWithFormat("Drag collection from %s to %s",
                                                                         maybeFromCollectionName->toUtf8().constData(),
                                                                         maybeToCollectionName->toUtf8().constData()));
                    }

                    onlyEmitEventForFirstItems = false;
                }

                auto overWriteExisting = SidebarItem::AndOverWriteExisting::Yes;
                if (!namesOfCratesAlreadyDropped.addingObjectCausedAnInsertion(fromString(crate->getName()))) {
                    // -- If we are dropping crates with the same name, we only overwrite the first crate dropped.
                    overWriteExisting = SidebarItem::AndOverWriteExisting::No;
                }

                auto maybeIndexInReceivingCrate = receivingItem->receiveDraggedCrateAtIndexWithPerItemProgressCallBack(crate,
                                                                                                                       destinationIndex,
                                                                                                                       [&job]() {
                                                                                                                           job->incrementProgressByOne();
                                                                                                                       }, overWriteExisting);
                if (maybeIndexInReceivingCrate.isValid()) {
                    auto& indexInReceivingCrate = *maybeIndexInReceivingCrate;
                    if (indexInReceivingCrate == destinationIndex) {
                        // -- If the crate was dropped in the place we asked for then we can continue to the next index.
                        // -- Otherwise we stay where we were before.
                        ++destinationIndex;
                    }

                    indicesOfCratesAdded.append(*maybeIndexInReceivingCrate);
                }
            }

            if (sidebarSelectionWillNeedClearing) {
                emit sidebarSelectionNeedsClearing();
            }

            NxADetail::maybeDraggedCrateObjects = nothing;
        });

        return true;
    }

    if (mimeData->hasFormat(tracklistMimeType)) {
        NXA_ASSERT_TRUE((row == -1) && (column == -1));

        auto maybeDraggedTracks = NxADetail::maybeDraggedTrackObjects;
        if (!maybeDraggedTracks.isValid()) {
            return false;
        }

        auto droppedTracks = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(mimeData->data(tracklistMimeType).toULongLong());
        auto originalDragged = reinterpret_cast<MutableArray<NxADetail::TrackType>*>(maybeDraggedTracks->asRawPointer());
        if (droppedTracks == nullptr || originalDragged == nullptr || *originalDragged != *droppedTracks || (droppedTracks->length() == 0u)) {
            return false;
        }

        boolean parentCollectionMustHaveTracksOnTheSameVolume = receivingItem->parentCollectionMustHaveTracksOnTheSameVolume();
        boolean parentCollectionAllowsMovieTracksInPlaylists = receivingItem->parentCollectionAllowsMovieTracksInPlaylists();

        if (parentCollectionMustHaveTracksOnTheSameVolume || !parentCollectionAllowsMovieTracksInPlaylists) {
            auto maybeCollectionVolume = receivingItem->maybeParentCollectionVolume();
            if (!maybeCollectionVolume.isValid()) {
                return false;
            }

            for (auto&& [index, track] : *droppedTracks) {
                if (!parentCollectionMustHaveTracksOnTheSameVolume && parentCollectionAllowsMovieTracksInPlaylists) {
                    // -- The user must have agreed to everything by now.
                    break;
                }

                if (parentCollectionMustHaveTracksOnTheSameVolume && (track->parentCollectionVolume() != *maybeCollectionVolume)) {
                    auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                    if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                          QObject::tr("Some of these tracks are not on the same volume as the destination collection."),
                                                                          QObject::tr("This is not supported by %1 collections and any such tracks will be skipped.").arg(collectionTypeName))) {
                        return false;
                    }

                    // -- The user has been notified so there's no need to test for this again.
                    parentCollectionMustHaveTracksOnTheSameVolume = false;
                }

                if (!parentCollectionAllowsMovieTracksInPlaylists && track->isAMovieTrack()) {
                    auto collectionTypeName = receivingItem->maybeParentCollectionTypeName().valueOr(QString{ "these" });
                    if (!ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Are you sure?"),
                                                                          QObject::tr("Some of these tracks are movie tracks."),
                                                                          QObject::tr("This is not supported in playlists by %1 collections and any such tracks will not be added to the destination playlist.").arg(collectionTypeName))) {
                        return false;
                    }

                    // -- The user has been notified so there's no need to test for this again.
                    parentCollectionAllowsMovieTracksInPlaylists = true;
                }
            }
        }

        this->p_workQueue.enqueueJob(*maybeNarrowCast<int>(droppedTracks->length()),
                                     [this, receivingItem, droppedTracks](auto job) {
            auto currentIndex = receivingItem->numberOfTracks();
            boolean onlyEmitEventForFirstItems = true;

            MutableArray<count> indicesOfTracksAdded;

            job->updateStatus(QString("Copying %1 tracks...").arg(droppedTracks->length()));

            for (auto&& [index, track] : *droppedTracks) {
                if (onlyEmitEventForFirstItems) {
                    auto fromCollectionName = track->nameOfParentCollection();
                    auto maybeToCollectionName = receivingItem->maybeParentCollectionTypeName();
                    if (maybeToCollectionName.isValid() &&
                        (fromCollectionName != *maybeToCollectionName)) {
                        CrashLog::addBreadCrumb(String::stringWithFormat("Drag tracks from %s to %s",
                                                                         fromCollectionName.toUtf8().constData(),
                                                                         maybeToCollectionName->toUtf8().constData()));
                    }

                    onlyEmitEventForFirstItems = false;
                }

                if (receivingItem->canReceive(track)) {
                    auto receivingAtIndex = std::min(currentIndex++, receivingItem->numberOfTracks());
                    auto maybeActualIndex = receivingItem->addTrackAtIndex(track.get(), *maybeNarrowCast<count>(receivingAtIndex));
                    if (maybeActualIndex.isValid()) {
                        indicesOfTracksAdded.append(*maybeActualIndex);
                    }

                    job->incrementProgressByOne();
                }
            }

            receivingItem->forceRefresh();

            MutableArray<NotNull<AbstractTrackItem*>> tracksThatHaveChanged;
            auto tracks = receivingItem->tracks().get();
            for (auto&& indexOfTrackAdded : indicesOfTracksAdded) {
                NXA_ASSERT_TRUE_WITH_BLOCK(indexOfTrackAdded < tracks.length(), [&indexOfTrackAdded, &tracks]() {
                    CrashLog::addUserInfoWithKey(String::stringWithFormat("%d", indexOfTrackAdded), "indexOfTrackAdded");
                    CrashLog::addUserInfoWithKey(String::stringWithFormat("%d", tracks.length()), "tracks.length");
                });

                tracksThatHaveChanged.append(NotNull<AbstractTrackItem*>{ tracks[indexOfTrackAdded].get() });
            }
            this->maybeUpdateBecauseTracksChanged({ std::move(tracksThatHaveChanged) });

            NxADetail::maybeDraggedTrackObjects = nothing;
        });

        return true;
    }

    if (mimeData->hasFormat(uriListMimeType)) {
        auto urls = mimeData->urls();
        NXA_ASSERT_FALSE_DEBUG(urls.empty());
        FilePath asFilePath{ String{ urls.first().toLocalFile().toUtf8().constData() } };

        if ((row >= 0) && (column >= 0)) {
            if (!receivingItem->receiveFileAt(asFilePath)) {
                return false;
            }

            this->p_workQueue.enqueueJob(0, [this](auto) {
                this->refreshCollections();
            });

            return true;
        }
        else {
            count numberOfOriginalChildren = receivingItem->childCount();
            if (numberOfOriginalChildren) {
                this->beginRemoveRows(parentIndex, 0, numberOfOriginalChildren - 1);
            }

            auto receiveWasSuccessful = receivingItem->receiveFileAt(asFilePath);

            this->notifyLayoutChangedAt(parentIndex);

            if (numberOfOriginalChildren) {
                this->endRemoveRows();
            }

            return receiveWasSuccessful;
        }
    }

    return false;
}

Qt::DropActions SidebarModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

Qt::DropActions SidebarModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QModelIndex SidebarModel::parent(const QModelIndex& childIndex) const
{
    auto childItem = this->itemFromIndex(childIndex);
    if (!childItem->hasParentItem()) {
        return { };
    }

    return this->indexForItem(NotNull<const SidebarItem*>{ childItem->parentItem() });
}

bool SidebarModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    // -- This is a bit convoluted but we need to make sure the inputted value is normalized here.
    auto asString = fromString(fromString(value.toString()).asNormalizedString());
    auto item = this->itemFromIndex(index);
    NXA_ASSERT_FALSE(item->isLoading() || !item->isLoaded());
    NXA_ASSERT_TRUE(item->canBeRenamed());

    if (asString.isEmpty()) {
        return false;
    }

    item->setName(asString);

    if (item->getName() == asString) {
        return false;
    }

    this->dataChanged(index, index);

    return true;
}

QVariant SidebarModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return { };
    }

    static QFont mainFont{ "Inter UI", NXA_NORMAL_FONT, QFont::Normal, false };
    static QFont headerFont{ "Inter UI", NXA_SMALL_FONT, QFont::Medium, false };

    auto item = this->itemFromIndex(index);
    if (item->hasParentItem() && item->parentItem()->isLoading()) {
        switch (role) {
            case Qt::FontRole: {
                return mainFont;
            }
            case Qt::DisplayRole: {
                return QString{ "Loading..." };
            }
            default: {
                break;
            }
        }

        return { };
    }

    switch (role) {
        case Qt::FontRole: {
            if (item->isSelectable()) {
                return mainFont;
            }

            return headerFont;
        }
        case Qt::DecorationRole: {
            auto icon = item->maybeIcon();
            if (icon) {
                return *icon;
            }

            break;
        }
        case Qt::DisplayRole: {
            NXA_ASSERT_EQ(index.column(), 0);
            return item->getName();
        }
        case Qt::ToolTipRole: {
            auto maybeDescription = item->getDescription();
            if (maybeDescription) {
                return *maybeDescription;
            }

            break;
        }
        default: {
            break;
        }
    }

    return { };
}

void SidebarModel::notifyLayoutChangedAt(QModelIndex alteredIndex)
{
    Threading::runOnMainThread([this, alteredIndex]() {
        QList<QPersistentModelIndex> itemsIndices{ alteredIndex };
        emit layoutChanged(itemsIndices);
    }, Threading::Blocking::Yes);
}
