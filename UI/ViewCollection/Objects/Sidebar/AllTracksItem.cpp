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

#include <ViewCollection/Sidebar/AllTracksItem.hpp>
#include <ViewCollection/TrackList/TrackListNode.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Crates/Folder.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

NotNull<const Common::Playlist*> AllTracksItem::p_makeSureAllTracksPlaylistIsLoaded() const
{
    if (!this->p_maybeAllTracksPlaylist.isValid()) {
        this->p_maybeAllTracksPlaylist = this->p_collection->tracks();
    }

    return *this->p_maybeAllTracksPlaylist;
}

AllTracksItem::AllTracksItem(NotNull<SidebarModel*> model,
                             SidebarItem* parent,
                             WorkQueue& withQueue,
                             NotNull<const Common::Collection*> withCollection) : AbstractCrateItem{ model,
                                                                                                     parent,
                                                                                                     withQueue },
                                                                                  p_collection{ withCollection } { }

count AllTracksItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return p_makeSureAllTracksPlaylistIsLoaded()->numberOfCratesAndTrackEntriesContainedWithin();;
}

count AllTracksItem::numberOfTracks() const
{
    return p_makeSureAllTracksPlaylistIsLoaded()->numberOfTracks();
}

QString AllTracksItem::getName() const
{
    return QObject::tr("All Tracks");
}

Optional<QIcon> AllTracksItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Playlist Icon/MusicPlaylist invert.png" };
    //if (this->isLoading()) {
    //    return NxADetail::loadingIcon;
    //}
    return cachedIcon;
}

Optional<QString> AllTracksItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_collection->type())) };
}

Optional<QString> AllTracksItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_collection->fullName()) };
}

Optional<Volume> AllTracksItem::maybeParentCollectionVolume() const
{
    return this->p_collection->volume();
}

boolean AllTracksItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_collection->hasTracksNotOnVolume(volume);
}

boolean AllTracksItem::hasMovieTracks() const
{
    return this->p_collection->rootFolder()->hasMovieTracks();
}

std::future<Optional<Shared<AbstractTrackListNode>>> AllTracksItem::maybeTrackTree()
{
    if (!this->p_collection->isOpened()) {
        return std::async([]() {
            return Optional<Shared<AbstractTrackListNode>>{ };
        });
    }

    return this->p_queue.enqueueJobWithStdFuture<Optional<Shared<AbstractTrackListNode>>>(0, [this](auto job) {
        this->p_makeSureAllTracksPlaylistIsLoaded();

        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<TrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) }, nothing,
                                                    [this]() -> TrackListNode::RefreshType {
                                                        MutableArray<NotNull<const Common::Track*>> tracks;

                                                        auto& allTracks = *this->p_maybeAllTracksPlaylist;
                                                        for (count i = 0; i < allTracks->numberOfTracks(); ++i) {
                                                            tracks.emplaceAppend(allTracks->trackAtIndex(i));
                                                        }

                                                        return { Array<NotNull<const Common::Track*>>(std::move(tracks)) };
                                                    }, ""));
        }

        return this->p_treeCache;
    });
}

boolean AllTracksItem::canBeRenamed() const
{
    return false;
}

boolean AllTracksItem::isSelectable() const
{
    return true;
}

boolean AllTracksItem::canReceiveTracks() const
{
    return false;
}

Optional<count> AllTracksItem::addTrackAtIndex(NotNull<AbstractTrackItem*>, count)
{
    return nothing;
}
