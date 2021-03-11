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

#include <MutableViewCollection/TrackList/MutableTrackItem.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>

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

MutableTrackItem::MutableTrackItem(Optional<NotNull<MutableTrackListNode*>> parent,
                                   NotNull<Common::MutableTrack*> withTrack) : AbstractTrackItem{ parent.isValid() ? parent->as<AbstractTrackListNode*>()
                                                                                                                   : Optional<NotNull<AbstractTrackListNode*>>{ } },
                                                                               p_track{ withTrack.get() }
{

}

boolean MutableTrackItem::canEdit()
{
    return true;
}

boolean MutableTrackItem::isAMovieTrack()
{
    return TrackFile::isAMovieTrack((*this->p_track).absoluteFilePath());
}

NotNull<const Common::MutableTrack*> MutableTrackItem::getTrack() const
{
    return this->p_track.asNotNull();
}

NotNull<Common::MutableTrack*> MutableTrackItem::getTrack()
{
    return this->p_track.asNotNull();
}

QString MutableTrackItem::getTrackRelativeFilePath() const
{
    return fromString((*this->p_track).relativeFilePath());
}

QString MutableTrackItem::getTrackAbsoluteFilePath() const
{
    return fromString((*this->p_track).absoluteFilePath());
}

QString MutableTrackItem::getTrackTitle() const
{
    auto value = (*this->p_track).maybeTitle();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QStringList MutableTrackItem::getTrackArtists() const
{
    auto value = (*this->p_track).artists();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackArtistsAsString() const
{
    return fromString(Common::Track::artistsAsStringFor(*this->p_track));
}

QStringList MutableTrackItem::getTrackProducers() const
{
    auto value = (*this->p_track).producers();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackProducersAsString() const
{
    return fromString(Common::Track::producersAsStringFor(*this->p_track));
}

QStringList MutableTrackItem::getTrackRemixers() const
{
    auto value = (*this->p_track).remixers();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackRemixersAsString() const
{
    return fromString(Common::Track::remixersAsStringFor(*this->p_track));
}

boolean MutableTrackItem::sameTrackAs(NotNull<const AbstractTrackItem*> other)
{
    auto otherCasted = other.maybeAs<const MutableTrackItem*>();
    if (!otherCasted.isValid()) {
        return false;
    }

    return this->p_track == (*otherCasted)->p_track;
}

QString MutableTrackItem::getTrackBeatsPerMinute() const
{
    auto value = (*this->p_track).maybeBeatsPerMinute();
    if (!value) {
        return { };
    }

    auto valueString = value->asString();
    return fromString(valueString);
}

QStringList MutableTrackItem::getTrackGenres() const
{
    auto value = (*this->p_track).genres();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackGenresAsString() const
{
    return fromString(Common::Track::genresAsStringFor(*this->p_track));
}

QStringList MutableTrackItem::getTrackTags() const
{
    auto value = (*this->p_track).tags();

    QStringList list;
    for (const auto& element : value) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackTagsAsString() const
{
    return fromString(Common::Track::tagsAsStringFor(*this->p_track));
}

QStringList MutableTrackItem::getTrackKeys() const
{
    auto values = (*this->p_track).musicalKeys();

    QStringList list;
    for (const auto& element : values) {
        list.append(fromString(element));
    }

    return list;
}

QString MutableTrackItem::getTrackKeysAsString() const
{
    return fromString(Common::Track::musicalKeysAsStringFor(*this->p_track));
}

QString MutableTrackItem::getTrackComments() const
{
    auto value = (*this->p_track).maybeComments();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString MutableTrackItem::getTrackAlbum() const
{
    auto value = (*this->p_track).maybeAlbum();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString MutableTrackItem::getTrackRecordLabel() const
{
    auto value = (*this->p_track).maybeRecordLabel();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString MutableTrackItem::getTrackMixName() const
{
    auto value = (*this->p_track).maybeMixName();
    if (!value) {
        return { };
    }

    return fromString(*value);
}

QString MutableTrackItem::getTrackGrouping() const
{
    return fromString((*this->p_track).maybeGrouping().valueOr(String{}));
}

Duration MutableTrackItem::getTrackLength() const
{
    return Duration::fromMilliseconds(((*this->p_track).maybeLengthInSeconds().valueOr(DecimalNumber{ }) * 1000).asInteger());
}

QString MutableTrackItem::getTrackDateReleased() const
{
    auto value = (*this->p_track).maybeDateReleased();
    if (!value) {
        return { };
    }

    return fromString(value->asString());
}

QString MutableTrackItem::getTrackDateAdded() const
{
    auto value = (*this->p_track).maybeDateAdded();
    if (!value) {
        return { };
    }

    return fromString(value->asString());
}

QDateTime MutableTrackItem::getTrackTimeLastModified() const
{
    auto value = (*this->p_track).lastModificationTime();

    QDateTime date;
    date.setTime_t(value.asUnixTimeStamp());

    return date;
}

count MutableTrackItem::getTrackNumber() const
{
    auto maybeValue = (*this->p_track).maybeTrackNumber();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count MutableTrackItem::getTrackCount() const
{
    auto maybeValue = (*this->p_track).maybeAlbumTrackCount();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count MutableTrackItem::getDiscNumber() const
{
    auto maybeDiscNumber = (*this->p_track).maybeDiscNumber();
    return maybeDiscNumber.isValid() ? *maybeDiscNumber : 0;
}

QColor MutableTrackItem::getTrackColor() const
{
    auto maybeColor = (*this->p_track).maybeColor();
    if (!maybeColor.isValid()) {
        return QColor{ };
    }

    return QColor{ maybeColor->red(), maybeColor->green(), maybeColor->blue() };
}

count MutableTrackItem::getTrackPlayCount() const
{
    auto maybePlayCount = (*this->p_track).maybePlayCount();
    return maybePlayCount.isValid() ? *maybePlayCount : 0;
}

Common::TrackRating MutableTrackItem::getTrackRating() const
{
    auto maybeRating = (*this->p_track).maybeRating();
    return maybeRating.isValid() ? *maybeRating : Common::TrackRating{ Common::TrackRating::Stars::Zero };
}

count MutableTrackItem::getTrackFileSize() const
{
    auto maybeValue = (*this->p_track).maybeFileSizeInBytes();
    return maybeValue.isValid() ? *maybeValue : 0;
}

QImage MutableTrackItem::getTrackArtwork() const
{
    auto trackFilePath = FilePath::filePathByJoiningPaths((*this->p_track).volume(), (*this->p_track).relativeFilePath());
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

count MutableTrackItem::getTrackBitRate() const
{
    auto maybeValue = (*this->p_track).maybeBitRateInKiloBitsPerSecond();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count MutableTrackItem::getTrackBitDepth() const
{
    auto maybeValue = (*this->p_track).maybeBitDepthInBits();
    return maybeValue.isValid() ? *maybeValue : 0;
}

count MutableTrackItem::getTrackSampleRate() const
{
    auto maybeValue = (*this->p_track).maybeSampleRateInHertz();
    return maybeValue.isValid() ? *maybeValue : 0;
}

QString MutableTrackItem::getTrackFileType() const
{
    auto maybeValue = TrackFile::maybeStringValueFor((*this->p_track).fileType());
    return maybeValue.isValid() ? fromString(*maybeValue) : QString{ };
}

Array<Common::MarkerOfSomeSort> MutableTrackItem::getMarkers() const
{
    return (*this->p_track).markers();
}

QString MutableTrackItem::nameOfParentCollection() const
{
    return fromString((*this->p_track).collection()->name());
}

QString MutableTrackItem::fullNameOfParentCollection() const
{
    return fromString((*this->p_track).collection()->fullName());
}

Volume MutableTrackItem::parentCollectionVolume() const
{
    return (*this->p_track).volume();
}

void MutableTrackItem::setTrackArtwork(const QImage& value)
{
    NXA_ALOG("To be implemented.");
}

void MutableTrackItem::setTrackFilename(const QString& value)
{
    NXA_ALOG("To be implemented.");
}

void MutableTrackItem::setTrackTitle(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setTitle(maybeString);
}

void MutableTrackItem::setTrackBeatsPerMinute(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    Optional<DecimalNumber> decValue;
    if (maybeString.isValid()) {
        // TODO: A factory method that returns Optional<DecimalNumber> that returns nothing in case of parse failure?
        decValue = DecimalNumber{ *maybeString };
    }

    (*this->p_track).setBeatsPerMinute(decValue);
}

void MutableTrackItem::setTrackComments(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setComments(maybeString);
}

void MutableTrackItem::setTrackAlbum(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setAlbum(maybeString);
}

void MutableTrackItem::setTrackRecordLabel(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setRecordLabel(maybeString);
}

void MutableTrackItem::setTrackMixName(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setMixName(maybeString);
}

void MutableTrackItem::setTrackGrouping(const QString& value)
{
    Optional<String> maybeString;

    if (value.length() > 0) {
        maybeString = fromString(value);
    }

    (*this->p_track).setGrouping(maybeString);
}

void MutableTrackItem::setTrackArtists(const QStringList& values)
{
    MutableArray<String> newValues;

    for (auto&& value : values) {
        if (value.length() > 0) {
            newValues.append(fromString(value));
        }
    }

    (*this->p_track).setArtists(newValues);
}

void MutableTrackItem::setTrackProducers(const QStringList& values)
{
    MutableArray<String> newValues;

    for (auto&& value : values) {
        if (value.length() > 0) {
            newValues.append(fromString(value));
        }
    }

    (*this->p_track).setProducers(newValues);
}

void MutableTrackItem::setTrackRemixers(const QStringList& values)
{
    MutableArray<String> newValues;

    for (auto&& value : values) {
        if (value.length() > 0) {
            newValues.append(fromString(value));
        }
    }

    (*this->p_track).setRemixers(newValues);
}

void MutableTrackItem::setTrackGenres(const QStringList& values)
{
    MutableArray<String> newValues;

    for (auto&& value : values) {
        if (value.length() > 0) {
            newValues.append(fromString(value));
        }
    }

    (*this->p_track).setGenres(newValues);
}

void MutableTrackItem::setTrackTags(const QStringList& values)
{
    MutableArray<String> newValues;

    for (auto&& value : values) {
        if (value.length() > 0) {
            newValues.append(fromString(value));
        }
    }

    (*this->p_track).setTags(newValues);
}

void MutableTrackItem::setTrackKeys(const QStringList& values)
{
    MutableArray<String> newKeys;

    for (auto&& value : values) {
        newKeys.append(fromString(value));
    }

    (*this->p_track).setMusicalKeys(newKeys);
}

void MutableTrackItem::setTrackDateReleased(const QString& value)
{
    (*this->p_track).setDateReleased(Date::maybeDateWithString(fromString(value)));
}

void MutableTrackItem::setTrackDateAdded(const QString& value)
{
    (*this->p_track).setDateAdded(Date::maybeDateWithString(fromString(value)));
}

void MutableTrackItem::setTrackLength(const Duration& value)
{
    (*this->p_track).setLengthInSeconds(DecimalNumber::withIntegerAndExponant(value.toMilliseconds(), -3));
}

void MutableTrackItem::setTrackNumber(const count& value)
{
    (*this->p_track).setTrackNumber(value);
}

void MutableTrackItem::setDiscNumber(const count& value)
{
    (*this->p_track).setDiscNumber(value);
}

void MutableTrackItem::setTrackPlayCount(const count& value)
{
    (*this->p_track).setPlayCount(value);
}

void MutableTrackItem::setTrackRating(const Common::TrackRating& value)
{
    (*this->p_track).setRating(value);
}

void MutableTrackItem::setTrackColor(const QColor& value)
{
    (*this->p_track).setColor(Color(*maybeNarrowCast<byte>(value.red()), *maybeNarrowCast<byte>(value.green()), *maybeNarrowCast<byte>(value.blue())));
}

void MutableTrackItem::markAsModifiedNow()
{
    (*this->p_track).markAsModifiedNow();
}

