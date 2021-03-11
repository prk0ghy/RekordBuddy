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
#include <ViewCollection/TrackList/TrackItem.hpp>

#include <CommonCollection/Crates/Playlist.hpp>

#include <Base/Types.hpp>

#include <memory>

namespace NxA::RekordBuddy {

class AbstractTrackItem;
class TrackItem;

struct RefreshVisitor;

class TrackListNode : public AbstractTrackListNode
{
public:
    // -- Public type aliases
    using NodeType = Variant<Array<Common::SubCrate>, Array<NotNull<const Common::Track*>>>;
    using RefreshType = Optional<NodeType>;

    friend RefreshVisitor;

protected:
    // -- Instance Variables
    Optional<Shared<AbstractTrackItem>> p_maybeTrackItem;

    std::function<RefreshType()> p_refresh;
    MutableArray<Shared<TrackListNode>> p_children;
    Optional<NotNull<AbstractCrateItem*>> p_maybeCrate;
    QString p_name;

    Optional<NotNull<TrackListNode*>> p_maybeParent;

public:
    // -- Constructor & Destructors
    TrackListNode() = delete;
    TrackListNode(Optional<NotNull<AbstractCrateItem*>> withCrate, Optional<NotNull<TrackListNode*>> withParent, std::function<RefreshType()>&& withTracks, QString withName = "");
    TrackListNode(Optional<NotNull<AbstractCrateItem*>> withCrate, Optional<NotNull<TrackListNode*>> withParent, NotNull<const Common::Track*>&& withTrack);

    // -- Overridden Instance Methods
    Optional<NotNull<const AbstractTrackListNode*>> maybeChildNode(count index) const override;
    Optional<NotNull<AbstractTrackListNode*>> maybeChildNode(count index) override;
    Optional<NotNull<const AbstractTrackListNode*>> maybeParent() const override;
    Optional<NotNull<AbstractTrackListNode*>> maybeParent() override;
    boolean hasParentItem() const override;
    count childCount() const override;
    Pointer<const AbstractTrackListNode> getChildAtIndex(count) const override;
    Pointer<AbstractTrackListNode> getChildAtIndex(count) override;
    Optional<NotNull<const AbstractCrateItem*>> maybeCrateItem() const override;
    Optional<NotNull<AbstractCrateItem*>> maybeCrateItem() override;
    Optional<NotNull<const AbstractTrackItem*>> maybeTrackItem() const override;
    Optional<NotNull<AbstractTrackItem*>> maybeTrackItem() override;
    count getRowInParent() const override;
    QString getSectionName() const override;
    Optional<count> getTrackIndex(NotNull<AbstractTrackItem*> identity) const override;
    void refreshTrackList() override;

    // Instance Methods
    Optional<NotNull<const Common::Track*>> maybeTrack() const;
};

}
