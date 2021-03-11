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

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Tracks/Track.hpp>

#include <MutableViewCollection/Sidebar/MutableAllTracksItem.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>

#include <ViewCollection/TrackList/TrackListNode.hpp>

#include <UiModel/SidebarModel.hpp>

#include <UiBase/AlertMessageBox.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <utility>
#include <future>

using namespace NxA;
using namespace RekordBuddy;

// -- Instance Methods

NotNull<Common::MutablePlaylist*> MutableAllTracksItem::p_makeSureAllTracksPlaylistIsLoaded() const
{
    if (!this->p_maybeAllTracksPlaylist.isValid()) {
        this->p_maybeAllTracksPlaylist = this->p_collection->tracks();
    }

    return *this->p_maybeAllTracksPlaylist;
}

MutableAllTracksItem::MutableAllTracksItem(NotNull<SidebarModel*> model,
                                           SidebarItem* parent,
                                           WorkQueue& withQueue,
                                           NotNull<Common::MutableCollection*> withCollection) : AbstractCrateItem{ model,
                                                                                                                    parent,
                                                                                                                    withQueue },
                                                                                                 p_collection{ withCollection } { }

count MutableAllTracksItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return p_makeSureAllTracksPlaylistIsLoaded()->numberOfCratesAndTrackEntriesContainedWithin();;
}

count MutableAllTracksItem::numberOfTracks() const
{
    return p_makeSureAllTracksPlaylistIsLoaded()->numberOfTracks();
}

QString MutableAllTracksItem::getName() const
{
    return QObject::tr("All Tracks");
}

Optional<QIcon> MutableAllTracksItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Playlist Icon/MusicPlaylist invert.png" };
    //if (this->isLoading()) {
    //    return NxADetail::loadingIcon;
    //}
    return cachedIcon;
}

Optional<QString> MutableAllTracksItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_collection->type())) };
}

Optional<QString> MutableAllTracksItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_collection->fullName()) };
}

Optional<Volume> MutableAllTracksItem::maybeParentCollectionVolume() const
{
    return this->p_collection->volume();
}

boolean MutableAllTracksItem::parentCollectionMustHaveTracksOnTheSameVolume() const
{
    return this->p_collection->mustHaveTracksOnTheSameVolume();
}

boolean MutableAllTracksItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_collection->hasTracksNotOnVolume(volume);
}

boolean MutableAllTracksItem::parentCollectionAllowsMovieTracksInPlaylists() const
{
    // -- Movie tracks are currently always allowed in the all track playlist.
    return true;
}

boolean MutableAllTracksItem::hasMovieTracks() const
{
    // -- Movie tracks are currently always allowed in the all track playlist.
    return false;
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutableAllTracksItem::maybeTrackTree()
{
    if (!this->p_collection->isOpened()) {
        return std::async([]() {
            return Optional<Shared<AbstractTrackListNode>>{ };
        });
    }

    return this->p_queue.enqueueJobWithStdFuture<Optional<Shared<AbstractTrackListNode>>>(0, [this](auto job) {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<MutableTrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) },
                                                       nothing,
                                                       [this]() -> MutableTrackListNode::RefreshType {
                                                           MutableArray<NotNull<Common::MutableTrack*>> tracks;
                                                           auto allTracks = this->p_makeSureAllTracksPlaylistIsLoaded();
                                                           auto initialModificationTime = allTracks->lastModificationTime();

                                                           for (count i = 0; i < allTracks->numberOfTracks(); ++i) {
                                                               tracks.emplaceAppend(allTracks->trackAtIndex(i));
                                                           }

                                                           if (allTracks->lastModificationTime() != initialModificationTime) {
                                                               // -- If accessing the tracks modified them then this means something was fixed.
                                                               Threading::runOnMainThread([]() {
                                                                   AlertMessageBox alert;
                                                                   alert.setText(QObject::tr(""),
                                                                                 QObject::tr("Looks like this collection contained some corrupted data."),
                                                                                 QObject::tr("This has been fixed and will be saved if you choose to do so when you exit the app."));
                                                                   alert.setButtonLayout(QObject::tr("Ok"));
                                                                   alert.exec();
                                                               }, Threading::Blocking::No);
                                                           }

                                                           return { Array<NotNull<Common::MutableTrack*>>(std::move(tracks)) };
                                                       },
                                                       ""));
        }

        return this->p_treeCache;
    });
}

void MutableAllTracksItem::moveTracksAtIndicesToIndex(Array<count> trackIndices, count toIndex)
{
    this->p_makeSureAllTracksPlaylistIsLoaded()->moveTracksAtIndicesToIndex(trackIndices, toIndex);

    if (this->p_treeCache.isValid()) {
        (*this->p_treeCache)->refreshTrackList();
    }
}

boolean MutableAllTracksItem::maybeConfirmTrackDeletion() const
{
    return ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Confirm Deletion"),
                                                            QObject::tr("Are you sure you want to remove this/these track(s) from the ENTIRE collection?"),
                                                            QObject::tr("You can still choose not to save these changes when you exit the app.")    );
}

void MutableAllTracksItem::removeTrackAtIndex(count index)
{
    MutableArray<NotNull<AbstractTrackItem*>> tracksThatHaveChanged;
    tracksThatHaveChanged.append(NotNull<AbstractTrackItem*>{ this->tracks().get()[index] });
    this->getModel()->maybeUpdateBecauseTracksChanged(tracksThatHaveChanged);

    // -- We do this last because we need the track item to still be around to perform the update above.
    this->p_makeSureAllTracksPlaylistIsLoaded()->removeTrackAtIndex(index);
}

boolean MutableAllTracksItem::canBeRenamed() const
{
    return false;
}

boolean MutableAllTracksItem::isSelectable() const
{
    return true;
}

boolean MutableAllTracksItem::canReceiveTracks() const
{
    return true;
}

boolean MutableAllTracksItem::canReceive(NotNull<AbstractTrackItem*> abstractTrack) const
{
    // -- For now we are going to deal with tracks that can't be received by a dialog after the drop is ordered
    return true;

#if 0
    if (auto directTrack = abstractTrack.maybeAs<MutableTrackItem*>(); directTrack.isValid()) {
        this->p_makeSureAllTracksPlaylistIsLoaded()->canReceive((*directTrack)->getTrack().as<Common::MutableTrack*>());
    }
    else if (auto directTrack = abstractTrack.maybeAs<TrackItem*>(); directTrack.isValid()) {
        this->p_makeSureAllTracksPlaylistIsLoaded()->canReceive((*directTrack)->getTrack().as<const Common::Track*>());
    }

    return false;
#endif
}

Optional<count> MutableAllTracksItem::addTrackAtIndex(NotNull<AbstractTrackItem*> abstractTrack, count index)
{
    if (auto directTrack = abstractTrack.maybeAs<MutableTrackItem*>(); directTrack.isValid()) {
        return this->p_makeSureAllTracksPlaylistIsLoaded()->addTrackAtIndex((*directTrack)->getTrack(),
                                                                            index,
                                                                            Common::MutablePlaylist::AndUpdateTracks::Yes);
    }
    else if (auto directTrack = abstractTrack.maybeAs<TrackItem*>(); directTrack.isValid()) {
        return this->p_makeSureAllTracksPlaylistIsLoaded()->addTrackAtIndex((*directTrack)->getTrack(),
                                                                            index,
                                                                            Common::MutablePlaylist::AndUpdateTracks::Yes);
    }

    return nothing;
}
