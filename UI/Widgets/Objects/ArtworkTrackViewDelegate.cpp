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

#include <Widgets/ArtworkTrackViewDelegate.hpp>

#include <UiModel/TrackListModel.hpp>
#include <UiBase/Types.hpp>

#include <qpushbutton.h>
#include <qpainter.h>

#include "Widgets/moc_ArtworkTrackViewDelegate.cpp"

namespace NxA::RekordBuddy {

// -- Types & Defines

#define ARTWORK_WIDTH 64
#define ARTWORK_HEIGHT 64
#define ARTWORK_PADDING 9
#define TEXT_TOP_PADDING 4
#define TEXT_LEFT_PADDING 4
#define TEXT_FIELD_WIDTH 444
#define TEXT_FIELD_VERTICAL_PADDING 2
#define TRACK_ENTRY_DEFAULT_WIDTH 740
#if defined(NXA_PLATFORM_MACOS)
#define TEXT_HEADER_VERTICAL_PADDING 4
#elif defined(NXA_PLATFORM_WINDOWS)
#define TEXT_HEADER_VERTICAL_PADDING 2
#else
#error Unsupported platform.
#endif

// -- Constructors & Destructors

ArtworkTrackViewDelegate::ArtworkTrackViewDelegate(QWidget* parent) : QStyledItemDelegate(parent)
{
    // Build a layout based on the OS's font size (Win & OSX each can scale text at the OS level)
    QFont boldFont{QStringLiteral("Inter UI"), NXA_NORMAL_FONT, QFont::Bold, false};
    QFontMetrics boldFontMetrics(boldFont);
    integer64 boldFontHeight = boldFontMetrics.height();
    integer64 maxFontHeightWithBuffer = boldFontHeight + TEXT_FIELD_VERTICAL_PADDING;

    this->frameArtwork.setRect(0, 0, ARTWORK_WIDTH, ARTWORK_HEIGHT);

    this->frameTitle.setRect(this->frameArtwork.right() + ARTWORK_PADDING, TEXT_TOP_PADDING, TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);
    this->frameArtists.setRect(this->frameArtwork.right() + ARTWORK_PADDING, this->frameTitle.bottom(), TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);

    this->frameRecordLabel.setRect(this->frameTitle.right() + TEXT_LEFT_PADDING, TEXT_TOP_PADDING, TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);
    this->frameKeys.setRect(this->frameTitle.right() + TEXT_LEFT_PADDING, this->frameTitle.bottom(), TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);
    this->frameTags.setRect(this->frameArtwork.right() + ARTWORK_PADDING, this->frameKeys.bottom(), TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);

    integer64 maxVerticalSize = (this->frameTags.bottom() > ARTWORK_HEIGHT) ? this->frameTags.bottom() : ARTWORK_HEIGHT;
    this->sizeTrackEntry = QSize(TRACK_ENTRY_DEFAULT_WIDTH, maxVerticalSize);

    this->frameIconExpanded.setRect(4, 8, 9, 6);
    this->frameIconCollapsed.setRect(6, 7, 6, 9);
    this->frameHeaderTitle.setRect(20, TEXT_HEADER_VERTICAL_PADDING, TEXT_FIELD_WIDTH, maxFontHeightWithBuffer);
    this->sizeHeaderEntry = QSize(TRACK_ENTRY_DEFAULT_WIDTH, maxFontHeightWithBuffer + (TEXT_HEADER_VERTICAL_PADDING * 2));
}

// -- Overridden Instance Methods

void ArtworkTrackViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    static QImage noArtwork{QImage{":/Icons/Warnings and Errors/No Artwork/No Artwork.png"}.scaled(getArtworkDisplayDimensions(),
                                                                                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation)};
    static QFont smallFont{"Inter UI", NXA_SMALL_FONT, QFont::Normal, false};
    static QFont mediumFont{"Inter UI", NXA_NORMAL_FONT, QFont::Medium, false};
    static QFont boldFont{"Inter UI", NXA_NORMAL_FONT, QFont::Bold, false};
    static QImage expandedIcon{":/qss_icons/rc/branch_open.png"};
    static QImage collapsedIcon{":/qss_icons/rc/branch_closed.png"};
    static QImage expandedIconSelected{":/qss_icons/rc/branch_open-on.png"};
    static QImage collapsedIconSelected{":/qss_icons/rc/branch_closed-on.png"};

    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (index.data(TrackListModel::TrackListUserDataIndex::IsAHeader).toBool()) {
        painter->translate(option.rect.x(), option.rect.y());
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
    else {
        painter->translate(option.rect.x(), option.rect.y());

        auto albumArtwork = index.data(enumToInteger(Common::Track::Property::AlbumArt)).value<QImage>();
        if (!albumArtwork.isNull()) {
            painter->drawImage(this->frameArtwork, albumArtwork);
        }
        else {
            painter->drawImage(this->frameArtwork, noArtwork);
        }

        painter->setFont(boldFont);
        painter->drawText(this->frameTitle, Qt::AlignLeft, index.data(enumToInteger(Common::Track::Property::Title)).toString());

        auto by = index.data(enumToInteger(Common::Track::Property::Artists)).toString();
        if (!by.isEmpty()) {
            painter->setFont(mediumFont);
            painter->drawText(this->frameArtists, Qt::AlignLeft, QString{"by %1"}.arg(by));
        }
        painter->setFont(smallFont);
        painter->drawText(this->frameRecordLabel, Qt::AlignLeft, index.data(enumToInteger(Common::Track::Property::RecordLabel)).toString());
        painter->drawText(this->frameKeys, Qt::AlignLeft, index.data(enumToInteger(Common::Track::Property::MusicalKeys)).toString());
        painter->drawText(this->frameTags, Qt::AlignLeft, index.data(enumToInteger(Common::Track::Property::Tags)).toString());
    }
    painter->restore();
}

QSize ArtworkTrackViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data(TrackListModel::TrackListUserDataIndex::IsAHeader).toBool()) {
        return this->sizeHeaderEntry;
    }
    return this->sizeTrackEntry;
}

// -- Class Methods

QSize ArtworkTrackViewDelegate::getArtworkDisplayDimensions()
{
    return {ARTWORK_WIDTH, ARTWORK_HEIGHT};
}
}
