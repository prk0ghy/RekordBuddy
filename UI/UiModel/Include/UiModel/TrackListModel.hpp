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

#include <CommonCollection/Tracks/Track.hpp>

#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Types.hpp>

#include <QAbstractItemModel>

#include <qtreeview.h>

namespace NxA { namespace Common {

// -- Forward Declarations
class MutablePlaylist;

}

namespace RekordBuddy {

// -- Forward Declarations
class AbstractTrackItem;
class AbstractTrackListNode;
class WorkQueue;

// -- Public Interface
class TrackListModel : public QAbstractItemModel
{
protected:
    // -- Instance Variables
    Optional<Shared<AbstractTrackListNode>> p_maybeTrackTree;

    Optional<Pointer<Common::MutablePlaylist>> p_maybeCurrentPlaylistBeingViewed;

    WorkQueue& p_workQueue;

public:
    // -- Instance Types
    enum TrackListUserDataIndex {
        IsAHeader = 0x230,
        HeaderText = 0x231,

        // -- These are stored in user data and should not be re-ordered or modified.
        // -- New values can be added to the list though.
        IndexNumber = 0x330,
    };
    // -- We need to make sure our constants don't clash with others.
    static_assert(enumToInteger(Common::Track::Property::LastProperty) < TrackListUserDataIndex::IndexNumber, "Enums are potentially clashing.");
    static_assert(enumToInteger(Qt::UserRole) < TrackListUserDataIndex::IndexNumber, "Enums are potentially clashing.");

    // -- Constructors & Destructors
    TrackListModel() = delete;
    TrackListModel(WorkQueue&, QObject*);
    ~TrackListModel() override = default;

    // -- Instance Methods
    void setCurrentPlaylistBeingDisplayed(Optional<Pointer<Common::MutablePlaylist>>&);

    QModelIndex indexForItem(NotNull<const AbstractTrackListNode*>) const;
    QModelIndex indexForItem(NotNull<AbstractTrackListNode*>);

    NotNull<const AbstractTrackListNode*> itemFromIndex(const QModelIndex&) const;
    NotNull<AbstractTrackListNode*> itemFromIndex(const QModelIndex&);

    Optional<NotNull<const AbstractTrackItem*>> maybeTrackAtIndex(const QModelIndex&) const;
    Optional<NotNull<AbstractTrackItem*>> maybeTrackAtIndex(const QModelIndex&);

    Shared<AbstractTrackListNode> getTrackTree();
    void setTrackTree(const Shared<AbstractTrackListNode>&);

    QVariant dataHeader(const QModelIndex&, int) const;
    QVariant dataTrack(const QModelIndex&, int) const;
    QVariant columnDisplayData(const QModelIndex&, int) const;

    void beginMoveRows(const QModelIndex&, int, int, const QModelIndex&, int);
    void endMoveRows();

    // -- Overridden Qt Instance Methods
    Qt::ItemFlags flags(const QModelIndex&) const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&) override;
    bool canDropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&) const override;
    bool removeRows(int, int, const QModelIndex&) override;
    QMimeData* mimeData(const QModelIndexList&) const override;
    QStringList mimeTypes() const override;
    QModelIndex parent(const QModelIndex&) const override;
    int rowCount(const QModelIndex&) const override;
    bool hasChildren(const QModelIndex&) const override;
    QModelIndex index(int, int, const QModelIndex&) const override;
    QVariant data(const QModelIndex&, int = Qt::DisplayRole) const override;

public slots:
    void notifyLayoutChangedAt(QModelIndex);
};

} }
