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

#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <CommonCollection/Crates/Playlist.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>
#include <memory>

#pragma once

namespace NxA::Common {

class MutableCollection;
class MutablePlaylist;

}

namespace NxA::RekordBuddy {

class AbstractTrackListItem;

class AllTracksItem : public AbstractCrateItem
{
    NotNull<const Common::Collection*> p_collection;
    mutable Optional<NotNull<const Common::Playlist*>> p_maybeAllTracksPlaylist;
    mutable Optional<Shared<AbstractTrackListNode>> p_treeCache;

    NotNull<const Common::Playlist*> p_makeSureAllTracksPlaylistIsLoaded() const;

public:
    AllTracksItem() = delete;
    explicit AllTracksItem(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&, NotNull<const Common::Collection*>);
    ~AllTracksItem() override = default;

    // -- Instance Methods
    void forceRefresh() override
    {
        if (this->p_treeCache.isValid()) {
            (*this->p_treeCache)->refreshTrackList();
        }

        this->SidebarItem::forceRefresh();
    }

    boolean isAnAllTracksPlaylist() const override
    {
        return true;
    }

    count numberOfCratesAndTrackEntriesContainedWithin() const override;
    count numberOfTracks() const override;

    count crateCount() const override
    {
        return 0;
    }

    QString getName() const override;

    std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree() override;

    Optional<QIcon> maybeIcon() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
    boolean hasTracksNotOnVolume(const Volume&) const override;
    boolean hasMovieTracks() const override;

    boolean isSelectable() const override;
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::AllTracks;
    }
    boolean canBeRenamed() const override;
    boolean canReceiveTracks() const override;

    Optional<count> addTrackAtIndex(NotNull<AbstractTrackItem*>, count) override;
};

}
