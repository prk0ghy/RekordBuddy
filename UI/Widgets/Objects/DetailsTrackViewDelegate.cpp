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

#include <UiModel/TrackListModel.hpp>
#include <Widgets/DetailsTrackViewDelegate.hpp>
#include <Widgets/TrackListDetailsView.hpp>
#include <RekordBuddyCollection/UserPreferences.hpp>
#include <UiBase/Types.hpp>
#include <Base/Volume.hpp>
#include <qpushbutton.h>
#include <qpainter.h>

#include "Widgets/moc_DetailsTrackViewDelegate.cpp"

namespace NxA::RekordBuddy {

// -- Types & Defines

#define TEXT_FIELD_WIDTH 444
#define TEXT_FIELD_VERTICAL_PADDING 2
#if defined(NXA_PLATFORM_MACOS)
#define TEXT_HEADER_VERTICAL_PADDING 4
#elif defined(NXA_PLATFORM_WINDOWS)
#define TEXT_HEADER_VERTICAL_PADDING 2
#else
#error Unsupported platform.
#endif

// -- Constructors & Destructors

DetailsTrackViewDelegate::DetailsTrackViewDelegate(QWidget* parent) : QStyledItemDelegate(parent)
{
    static QFont boldFont{ "Inter UI", NXA_NORMAL_FONT, QFont::Bold, false };
    QFontMetrics boldFontMetrics{ boldFont };
    integer64 boldFontHeight = boldFontMetrics.height();
    integer64 maxFontHeightWithBuffer = boldFontHeight + TEXT_FIELD_VERTICAL_PADDING;

    this->frameIconExpanded.setRect(4, 8, 9, 6);
    this->frameIconCollapsed.setRect(6, 7, 6, 9);
    this->frameHeaderTitle.setRect(20, TEXT_HEADER_VERTICAL_PADDING, TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);
}

// -- Overridden Instance Methods

void DetailsTrackViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    static QImage expandedIcon{ QStringLiteral(":/qss_icons/rc/branch_open.png") };
    static QImage collapsedIcon{ QStringLiteral(":/qss_icons/rc/branch_closed.png") };
    static QImage expandedIconSelected{ QStringLiteral(":/qss_icons/rc/branch_open-on.png") };
    static QImage collapsedIconSelected{ QStringLiteral(":/qss_icons/rc/branch_closed-on.png") };
    static QFont boldFont{ QStringLiteral("Inter UI"), NXA_NORMAL_FONT, QFont::Bold, false };

    if (index.data(TrackListModel::TrackListUserDataIndex::IsAHeader).toBool()) {
        QStyledItemDelegate::paint(painter, option, index);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->translate(option.rect.x(), option.rect.y());
        if (index.column() == 0) {
            if ((option.state & QStyle::State_Open) > 0) {
                if ((option.state & QStyle::State_MouseOver) > 0) {
                    painter->drawImage(this->frameIconExpanded, expandedIconSelected);
                }
                else {
                    painter->drawImage(this->frameIconExpanded, expandedIcon);
                }
            }
            else {
                if ((option.state & QStyle::State_MouseOver) > 0) {
                    painter->drawImage(this->frameIconCollapsed, collapsedIconSelected);
                }
                else {
                    painter->drawImage(this->frameIconCollapsed, collapsedIcon);
                }
            }
            painter->setFont(boldFont);
            painter->drawText(this->frameHeaderTitle, Qt::AlignLeft, index.data(TrackListModel::TrackListUserDataIndex::HeaderText).toString());
        }
        painter->restore();
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize DetailsTrackViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
#if 0
    // -- TODO: Figure out why the size changes when tracks are moved in the tracklist.
    if (index.data(TrackListModel::TrackListUserDataIndex::IsAHeader).toBool()) {
        return TrackListDetailsView::GetHeaderDimensions();
    }
    return QStyledItemDelegate::sizeHint(option, index);
#else
    return TrackListDetailsView::GetTrackDimensions();
#endif
}

}
