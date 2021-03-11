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

#include <ViewCollection/TrackList/TrackItem.hpp>
#include <ViewCollection/TrackList/TrackListNode.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <UiBase/Types.hpp>

#include <RekordBuddyCollection/RekordBuddy.hpp>

#include <CommonCollection/Tracks/Track.hpp>

#include <TrackFiles/TrackFileFactory.hpp>
#include <TrackFiles/TrackFile.hpp>

#include <Base/Base.hpp>

#include <QtGlobal>
#include <QDataStream>
#include <QImage>

#include <type_traits>

using namespace NxA;
using namespace RekordBuddy;

TrackItem::TrackItem(NotNull<const AbstractCrateItem*> withItems,
                     Optional<NotNull<TrackListNode*>> parent,
                     NotNull<const Common::Track*> withTrack) : AbstractTrackItem{ parent.isValid() ? parent->as<AbstractTrackListNode*>() : Optional<NotNull<AbstractTrackListNode*>>{ } },
                                                                p_track{ withTrack.get() }
{

}

boolean TrackItem::canEdit()
{
    return false;
}

boolean TrackItem::isAMovieTrack()
{
    return TrackFile::isAMovieTrack((*this->p_track).absoluteFilePath());
}

NotNull<const Common::Track*> TrackItem::getTrack() const
{
    return this->p_track.asNotNull();
}

NotNull<const Common::Track*> TrackItem::getTrack()
{
    return this->p_track.asNotNull();
}

QString TrackItem::getTrackRelativeFilePath() const
{
    return fromString(this->p_track.asReference().relativeFilePath());
}

QString TrackItem::getTrackAbsoluteFilePath() const
{
    return fromString(this->p_track.asReference().absoluteFilePath());
}

QString TrackItem::getTrackTitle() const
{
    auto value = this->p_track.asReference().maybeTitle();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QStringList TrackItem::getTrackArtists() const
{
    auto value = this->p_track.asReference().artists();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackArtistsAsString() const
{
    return fromString(Common::Track::artistsAsStringFor(*this->p_track));
}

QStringList TrackItem::getTrackProducers() const
{
    auto value = this->p_track.asReference().producers();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackProducersAsString() const
{
    return fromString(Common::Track::producersAsStringFor(*this->p_track));
}

QStringList TrackItem::getTrackRemixers() const
{
    auto value = this->p_track.asReference().remixers();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackRemixersAsString() const
{
    return fromString(Common::Track::remixersAsStringFor(*this->p_track));
}

boolean TrackItem::sameTrackAs(NotNull<const AbstractTrackItem*> other)
{
    auto otherCasted = other.maybeAs<const TrackItem*>();
    if (!otherCasted.isValid()) {
        return false;
    }
    return this->p_track == (*otherCasted)->p_track;
}

QString TrackItem::getTrackBeatsPerMinute() const
{
    auto value = this->p_track.asReference().maybeBeatsPerMinute();
    if (!value) {
        return { };
    }

    auto valueString = value->asString();
    return fromString(valueString);
}

QStringList TrackItem::getTrackGenres() const
{
    auto value = this->p_track.asReference().genres();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackGenresAsString() const
{
    return fromString(Common::Track::genresAsStringFor(*this->p_track));
}

QStringList TrackItem::getTrackTags() const
{
    auto value = this->p_track.asReference().tags();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackTagsAsString() const
{
    return fromString(Common::Track::tagsAsStringFor(*this->p_track));
}

QStringList TrackItem::getTrackKeys() const
{
    auto values = this->p_track.asReference().musicalKeys();

    QStringList list;
    for (const auto& element : values) {
        list.append(fromString(element));
    }

    return list;
}

QString TrackItem::getTrackKeysAsString() const
{
    return fromString(Common::Track::musicalKeysAsStringFor(*this->p_track));
}

QString TrackItem::getTrackComments() const
{
    auto value = this->p_track.asReference().maybeComments();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString TrackItem::getTrackAlbum() const
{
    auto value = this->p_track.asReference().maybeAlbum();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString TrackItem::getTrackRecordLabel() const
{
    auto value = this->p_track.asReference().maybeRecordLabel();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString TrackItem::getTrackMixName() const
{
    auto value = this->p_track.asReference().maybeMixName();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString TrackItem::getTrackGrouping() const
{
    return fromString(this->p_track.asReference().maybeGrouping().valueOr(String{}));
}

Duration TrackItem::getTrackLength() const
{
    return Duration::fromMilliseconds((this->p_track.asReference().maybeLengthInSeconds().valueOr(DecimalNumber{ }) * 1000).asInteger());
}

QString TrackItem::getTrackDateReleased() const
{
    auto value = this->p_track.asReference().maybeDateReleased();
    if (!value) {
        return { };
    }

    return fromString(value->asString());
}

QString TrackItem::getTrackDateAdded() const
{
    auto value = this->p_track.asReference().maybeDateAdded();
    if (!value) {
        return { };
    }

    return fromString(value->asString());
}

QDateTime TrackItem::getTrackTimeLastModified() const
{
    auto value = this->p_track.asReference().lastModificationTime();

    QDateTime date;
    date.setTime_t(value.asUnixTimeStamp());

    return date;
}

count TrackItem::getTrackNumber() const
{
    auto maybeTrackNumber = this->p_track.asReference().maybeTrackNumber();
    return maybeTrackNumber.isValid() ? *maybeTrackNumber : 0;
}

count TrackItem::getTrackCount() const
{
    auto maybeValue = this->p_track.asReference().maybeAlbumTrackCount();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count TrackItem::getDiscNumber() const
{
    auto maybeDiscNumber = this->p_track.asReference().maybeDiscNumber();
    return maybeDiscNumber.isValid() ? *maybeDiscNumber : 0;
}

QColor TrackItem::getTrackColor() const
{
    auto maybeColor = this->p_track.asReference().maybeColor();
    if (!maybeColor.isValid()) {
        return QColor{};
    }

    return QColor{ maybeColor->red(), maybeColor->green(), maybeColor->blue() };
}

count TrackItem::getTrackPlayCount() const
{
    auto maybePlayCount = this->p_track.asReference().maybePlayCount();
    return maybePlayCount.isValid() ? *maybePlayCount : 0;
}

Common::TrackRating TrackItem::getTrackRating() const
{
    auto maybeRating = this->p_track.asReference().maybeRating();
    return maybeRating.isValid() ? *maybeRating : Common::TrackRating{ Common::TrackRating::Stars::Zero };
}

count TrackItem::getTrackFileSize() const
{
    auto maybeValue = this->p_track.asReference().maybeFileSizeInBytes();
    return maybeValue.isValid() ? *maybeValue : 0;
}

QImage TrackItem::getTrackArtwork() const
{
    auto trackFilePath = FilePath::filePathByJoiningPaths(this->p_track.asReference().volume(), this->p_track.asReference().relativeFilePath());
    Optional<TrackFile> trackFile = TrackFileFactory::maybeTrackFileForPath(trackFilePath);
    if (trackFile.isValid()) {
        auto artwork = trackFile->artwork();
        if (artwork.size() != 0u) {
            QImage artworkForQt;
            artworkForQt.loadFromData(artwork.data().get(), artwork.size());
            return artworkForQt;
        }
    }

    // -- No image available
    return QImage{};
}

count TrackItem::getTrackBitRate() const
{
    auto maybeValue = this->p_track.asReference().maybeBitRateInKiloBitsPerSecond();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count TrackItem::getTrackBitDepth() const
{
    auto maybeValue = this->p_track.asReference().maybeBitDepthInBits();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count TrackItem::getTrackSampleRate() const
{
    auto maybeValue = this->p_track.asReference().maybeSampleRateInHertz();
    return maybeValue.isValid() ? *maybeValue : 0;
}

QString TrackItem::getTrackFileType() const
{
    auto maybeValue = TrackFile::maybeStringValueFor(this->p_track.asReference().fileType());
    return maybeValue.isValid() ? fromString(*maybeValue) : QString{ };
}

Array<Common::MarkerOfSomeSort> TrackItem::getMarkers() const
{
    return this->p_track.asReference().markers();
}

QString TrackItem::nameOfParentCollection() const
{
    return fromString(this->p_track.asReference().collection()->name());
}

QString TrackItem::fullNameOfParentCollection() const
{
    return fromString(this->p_track.asReference().collection()->fullName());
}

Volume TrackItem::parentCollectionVolume() const
{
    return (*this->p_track).collection()->volume();
}
