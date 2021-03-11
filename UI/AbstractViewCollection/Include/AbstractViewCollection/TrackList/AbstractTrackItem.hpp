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

#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>

#include <CommonCollection/Markers/Markers.hpp>

#include <CommonCollection/Tracks/TrackRating.hpp>

#include <Base/Duration.hpp>
#include <Base/NotNull.hpp>
#include <Base/Set.hpp>
#include <Base/Pointers.hpp>
#include <Base/Variant.hpp>

#include <QVariant>

#include <memory>

namespace NxA { namespace RekordBuddy {

// -- Public Inferface
class AbstractTrackItem : public std::enable_shared_from_this<AbstractTrackItem>
{
    // -- Private Instance Variables
    Optional<NotNull<AbstractTrackListNode*>> p_maybeParentNode;

public:
    // -- Constructors & Destructors
    AbstractTrackItem(Optional<NotNull<AbstractTrackListNode*>> parent);
    virtual ~AbstractTrackItem() = default;

    // -- Instance Methods
    Optional<NotNull<AbstractTrackListNode*>> maybeParent();

    virtual boolean sameTrackAs(NotNull<const AbstractTrackItem*> other) = 0;
    virtual boolean canEdit() = 0;
    virtual boolean isAMovieTrack() = 0;

    virtual QString getTrackRelativeFilePath() const = 0;
    virtual QString getTrackAbsoluteFilePath() const = 0;
    virtual QString getTrackTitle() const = 0;
    virtual QString getTrackBeatsPerMinute() const = 0;
    virtual QString getTrackComments() const = 0;
    virtual QString getTrackAlbum() const = 0;
    virtual QString getTrackRecordLabel() const = 0;
    virtual QString getTrackMixName() const = 0;
    virtual QString getTrackGrouping() const = 0;
    virtual QStringList getTrackArtists() const = 0;
    virtual QString getTrackArtistsAsString() const = 0;
    virtual QStringList getTrackProducers() const = 0;
    virtual QString getTrackProducersAsString() const = 0;
    virtual QStringList getTrackRemixers() const = 0;
    virtual QString getTrackRemixersAsString() const = 0;
    virtual QStringList getTrackGenres() const = 0;
    virtual QString getTrackGenresAsString() const = 0;
    virtual QStringList getTrackTags() const = 0;
    virtual QString getTrackTagsAsString() const = 0;
    virtual QStringList getTrackKeys() const = 0;
    virtual QString getTrackKeysAsString() const = 0;
    virtual QString getTrackDateReleased() const = 0;
    virtual QString getTrackDateAdded() const = 0;
    virtual QDateTime getTrackTimeLastModified() const = 0;
    virtual Duration getTrackLength() const = 0;
    virtual count getTrackNumber() const = 0;
    virtual count getTrackCount() const = 0;
    virtual count getDiscNumber() const = 0;
    virtual count getTrackPlayCount() const = 0;
    virtual Common::TrackRating getTrackRating() const = 0;
    virtual QColor getTrackColor() const = 0;
    virtual count getTrackFileSize() const = 0;
    virtual QImage getTrackArtwork() const = 0;
    virtual count getTrackBitRate() const = 0;
    virtual count getTrackBitDepth() const = 0;
    virtual count getTrackSampleRate() const = 0;
    virtual QString getTrackFileType() const = 0;
    virtual Array<Common::MarkerOfSomeSort> getMarkers() const = 0;

    virtual QString nameOfParentCollection() const = 0;
    virtual QString fullNameOfParentCollection() const = 0;
    virtual Volume parentCollectionVolume() const = 0;

    virtual void setTrackFilename(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackTitle(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackBeatsPerMinute(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackComments(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackAlbum(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackRecordLabel(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackMixName(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackGrouping(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackArtists(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackProducers(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackRemixers(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackGenres(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackTags(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackKeys(const QStringList&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackDateReleased(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackDateAdded(const QString&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackLength(const Duration&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackNumber(const count&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setDiscNumber(const count&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackPlayCount(const count&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackRating(const Common::TrackRating&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackColor(const QColor&)
    {
        NXA_ALOG("Unsupported");
    }
    virtual void setTrackArtwork(const QImage&)
    {
        NXA_ALOG("Unsupported");
    }

    virtual void markAsModifiedNow()
    {
        NXA_ALOG("Unsupported");
    }
};

namespace NxADetail {

using TrackType = std::tuple<QModelIndex, NotNull<AbstractTrackItem*>>;
extern Optional<Shared<MutableArray<TrackType>>> maybeDraggedTrackObjects;

}

} }
