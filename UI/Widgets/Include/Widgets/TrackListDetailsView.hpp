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

#include <Base/Types.hpp>
#include <Base/Array.hpp>
#include <qtreeview.h>

namespace NxA::RekordBuddy {

// -- Forward Declarations
class AbstractCrateItem;

// -- Public Interface
class TrackListDetailsView : public QTreeView
{
    Q_OBJECT

    // -- Private Instance Variables
    Optional<NotNull<AbstractCrateItem*>> p_maybeCurrentlyDisplayedCrate;

public:
    // -- Static Methods
    static QSize GetHeaderDimensions();
    static QSize GetTrackDimensions();

    // -- Constructors & Destructors
    explicit TrackListDetailsView(QWidget* = nullptr);

    // -- Instance Methods
    void showContextMenu(QPoint);
    void toggleColumnVisible();
    String columnWidthIDAtIndex(count) const;
    String columnWidthIDWithUserDataIndex(count) const;
    String columnHiddenIDAtIndex(count) const;
    void columnMoved(int, int, int);
    void columnResized(int, int, int);

    void setCurrentlyDisplayedCrate(Optional<NotNull<AbstractCrateItem*>> crate)
    {
        this->p_maybeCurrentlyDisplayedCrate = crate;
    }

    // -- Overidden Qt Instance Methods
    void keyPressEvent(QKeyEvent*) override;
    void setModel(QAbstractItemModel *) override;
};

}
