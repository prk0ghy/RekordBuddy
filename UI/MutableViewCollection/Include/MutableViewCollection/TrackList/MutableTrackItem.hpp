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

#include <CommonCollection/Collection.hpp>
#include <Base/Types.hpp>

#include <qcolor.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qstringlist.h>

#pragma once

namespace NxA { namespace RekordBuddy {

class MutableTrackListNode;

class MutableTrackItem : public AbstractTrackItem
{
protected:
    Pointer<Common::MutableTrack> p_track;

public:
    MutableTrackItem() = delete;
    MutableTrackItem(Optional<NotNull<MutableTrackListNode*>> parent, NotNull<Common::MutableTrack*>);

    NotNull<const Common::MutableTrack*> getTrack() const;
    NotNull<Common::MutableTrack*> getTrack();

    boolean canEdit() override;
    boolean isAMovieTrack() override;

    boolean sameTrackAs(NotNull<const AbstractTrackItem*> other) override;

    QString getTrackRelativeFilePath() const override;
    QString getTrackAbsoluteFilePath() const override;
    QString getTrackTitle() const override;
    QStringList getTrackArtists() const override;
    QString getTrackArtistsAsString() const override;
    QStringList getTrackProducers() const override;
    QString getTrackProducersAsString() const override;
    QStringList getTrackRemixers() const override;
    QString getTrackRemixersAsString() const override;
    QString getTrackBeatsPerMinute() const override;
    QStringList getTrackGenres() const override;
    QString getTrackGenresAsString() const override;
    QStringList getTrackTags() const override;
    QString getTrackTagsAsString() const override;
    QStringList getTrackKeys() const override;
    QString getTrackKeysAsString() const override;
    QString getTrackComments() const override;
    QString getTrackAlbum() const override;
    QString getTrackRecordLabel() const override;
    QString getTrackMixName() const override;
    QString getTrackGrouping() const override;
    Duration getTrackLength() const override;
    QString getTrackDateReleased() const override;
    QString getTrackDateAdded() const override;
    QDateTime getTrackTimeLastModified() const override;
    count getTrackNumber() const override;
    count getTrackCount() const override;
    count getDiscNumber() const override;
    QColor getTrackColor() const override;
    count getTrackPlayCount() const override;
    Common::TrackRating getTrackRating() const override;
    count getTrackFileSize() const override;
    QImage getTrackArtwork() const override;
    count getTrackBitRate() const override;
    count getTrackBitDepth() const override;
    count getTrackSampleRate() const override;
    QString getTrackFileType() const override;
    Array<Common::MarkerOfSomeSort> getMarkers() const override;

    QString nameOfParentCollection() const override;
    QString fullNameOfParentCollection() const override;
    Volume parentCollectionVolume() const override;

    void setTrackFilename(const QString& value) override;
    void setTrackTitle(const QString& value) override;
    void setTrackBeatsPerMinute(const QString& value) override;
    void setTrackComments(const QString& value) override;
    void setTrackAlbum(const QString& value) override;
    void setTrackRecordLabel(const QString& value) override;
    void setTrackMixName(const QString& value) override;
    void setTrackGrouping(const QString& value) override;
    void setTrackArtists(const QStringList& values) override;
    void setTrackProducers(const QStringList& values) override;
    void setTrackRemixers(const QStringList& values) override;
    void setTrackGenres(const QStringList& values) override;
    void setTrackTags(const QStringList& values) override;
    void setTrackKeys(const QStringList& values) override;
    void setTrackDateReleased(const QString& value) override;
    void setTrackDateAdded(const QString& value) override;
    void setTrackLength(const Duration& value) override;
    void setTrackNumber(const count& value) override;
    void setDiscNumber(const count& value) override;
    void setTrackPlayCount(const count& value) override;
    void setTrackRating(const Common::TrackRating& value) override;
    void setTrackColor(const QColor& value) override;
    void setTrackArtwork(const QImage& value) override;

    void markAsModifiedNow() override;
};

} }
