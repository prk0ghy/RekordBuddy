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
#include <qstyleditemdelegate.h>

namespace NxA::RekordBuddy {
class ArtworkTrackViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    // -- Constructors & Destructors
    ArtworkTrackViewDelegate(QWidget* parent = Q_NULLPTR);
    // -- Overridden Instance Methods
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    // -- Class Methods
    static QSize getArtworkDisplayDimensions();

protected:
    QRect frameIconExpanded;
    QRect frameIconCollapsed;
    QRect frameArtwork;
    QRect frameTitle;
    QRect frameArtists;
    QRect frameTags;
    QRect frameRecordLabel;
    QRect frameKeys;
    QRect frameHeaderTitle;
    QSize sizeHeaderEntry;
    QSize sizeTrackEntry;
};
}
