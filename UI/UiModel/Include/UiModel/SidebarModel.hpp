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

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCollectionList.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <Base/Array.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <QAbstractItemModel>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class WorkQueue;

// -- Public Interface
class SidebarModel : public QAbstractItemModel
{
    Q_OBJECT

    // -- Private Instance Variables
    WorkQueue& p_workQueue;
    AbstractCollectionList* p_rootItem = nullptr;

    QModelIndex p_displayedIndex;

public:
    // -- Constructors & Destructors
    explicit SidebarModel(WorkQueue& withWorkQueue);

    // -- Instance Methods
    void setCollectionList(NotNull<AbstractCollectionList*> withItem)
    {
        this->p_rootItem = withItem.get();
    }
    NotNull<SidebarItem*> itemFromIndex(const QModelIndex&) const;
    boolean requestDeletionOfItemsAtIndices(const QModelIndexList&);
    boolean hasChangesToSave() const;
    boolean saveChangesWithProgress(std::function<void(double)>&&);
    QModelIndex indexForItem(NotNull<const SidebarItem*>) const;
    QModelIndex indexForItem(NotNull<SidebarItem*>) const;

    inline QModelIndex rootModelIndex() const
    {
        return this->p_rootItem->getModelIndex();
    }
    inline void refreshCollections()
    {
        this->p_rootItem->refreshCollectionsWithPerCollectionProgressCallBack([]() { });
    }

    void maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>&);

    void beginInsertRows(const QModelIndex&, int, int);
    void endInsertRows();

    void beginRemoveRows(const QModelIndex&, int, int);
    void endRemoveRows();

    void beginMoveRows(const QModelIndex&, int, int, const QModelIndex&, int);
    void endMoveRows();

    void setDisplayedIndex(const QModelIndex& index)
    {
        this->p_displayedIndex = index;
    }
    boolean displayedItemIsnItemsOrChildOfItemsAtIndices(const QModelIndexList&);

    int columnCount(const QModelIndex& parent) const final;
    QModelIndex parent(const QModelIndex& childIndex) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;
    QModelIndex index(int row, int column, const QModelIndex& parent) const final;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final;
    int rowCount(const QModelIndex& parent) const final;
    Qt::ItemFlags flags(const QModelIndex& index) const final;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool canDropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
    bool dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parentIndex) override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;

public slots:
    void notifyLayoutChangedAt(QModelIndex);

signals:
    void sidebarSelectionNeedsClearing();
};

} }
