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

#include <ViewCollection/TrackList/TrackListNode.hpp>
#include <ViewCollection/TrackList/TrackItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <CommonCollection/Crates/SmartPlaylist.hpp>
#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/Folder.hpp>

#include <Base/Base.hpp>

#include <QDataStream>
#include <list>
#include <type_traits>
#include <utility>

using namespace NxA;
using namespace RekordBuddy;

struct NxA::RekordBuddy::RefreshVisitor
{
    NotNull<TrackListNode*> that;

    void operator()(Array<NotNull<const Common::Track*>>& tracks)
    {
        for (auto&& track : tracks) {
            that->p_children.append(Shared<TrackListNode>::with(that->p_maybeCrate, that, std::move(track)));
        }
    }

    void operator()(Array<Common::SubCrate>& subCrates)
    {
        for (auto&& subCrate : subCrates) {
            this->that->p_children.append(subCrate.apply([this](auto& subCrate) {
                return Shared<TrackListNode>::with(this->that->p_maybeCrate, this->that,
                                                       [&subCrate]() -> TrackListNode::RefreshType {
                                                           return { subCrate->tracks() };
                                                       },
                                                       fromString(subCrate->name()));
            }));
        }
    }
};

// -- Constructor & Destructors

TrackListNode::TrackListNode(Optional<NotNull<AbstractCrateItem*>> withCrate,
                             Optional<NotNull<TrackListNode*>> withParent,
                             std::function<RefreshType()>&& withTracks,
                             QString withName) : p_refresh{ std::move(withTracks) },
                                                 p_maybeCrate{ withCrate },
                                                 p_name{ std::move(withName) },
                                                 p_maybeParent{ withParent }
{
    this->refreshTrackList();
}

TrackListNode::TrackListNode(Optional<NotNull<AbstractCrateItem*>> withCrate,
                             Optional<NotNull<TrackListNode*>> withParent,
                             NotNull<const Common::Track*>&& withTrack) : p_maybeCrate{ withCrate },
                                                                          p_maybeParent{ withParent }
{
    NXA_ASSERT_TRUE(withCrate.isValid());

    this->p_maybeTrackItem = Shared<AbstractTrackItem>{ Shared<TrackItem>::with(withCrate->constCast<const AbstractCrateItem*>(), nothing, std::move(withTrack)) };
}

// -- Overridden Instance Methods

Optional<NotNull<const AbstractTrackListNode*>> TrackListNode::maybeChildNode(count index) const
{
    if (index < this->p_children.length()) {
        return NotNull<const AbstractTrackListNode*>{ this->p_children[index].asRawPointer() };
    }

    return nothing;
}

Optional<NotNull<AbstractTrackListNode*>> TrackListNode::maybeChildNode(count index)
{
    if (index < this->p_children.length()) {
        return NotNull<AbstractTrackListNode*>{ this->p_children[index].asRawPointer() };
    }

    return nothing;
}

Optional<NotNull<const AbstractTrackListNode*>> TrackListNode::maybeParent() const
{
    return this->p_maybeParent.isValid() ? this->p_maybeParent->as<const AbstractTrackListNode*>() : Optional<NotNull<const AbstractTrackListNode*>>{ };
}

Optional<NotNull<AbstractTrackListNode*>> TrackListNode::maybeParent()
{
    return this->p_maybeParent.isValid() ? this->p_maybeParent->as<AbstractTrackListNode*>() : Optional<NotNull<AbstractTrackListNode*>>{ };
}

boolean TrackListNode::hasParentItem() const
{
    return this->p_maybeParent.isValid();
}

count TrackListNode::childCount() const
{
    return this->p_children.length();
}

Pointer<const AbstractTrackListNode> TrackListNode::getChildAtIndex(count index) const
{
    return { this->p_children[index].asRawPointer() };
}

Pointer<AbstractTrackListNode> TrackListNode::getChildAtIndex(count index)
{
    return { this->p_children[index].asRawPointer() };
}

Optional<NotNull<const AbstractCrateItem*>> TrackListNode::maybeCrateItem() const
{
    return this->p_maybeCrate.isValid() ? this->p_maybeCrate->constCast<const AbstractCrateItem*>() : Optional<NotNull<const AbstractCrateItem*>>{ };
}

Optional<NotNull<AbstractCrateItem*>> TrackListNode::maybeCrateItem()
{
    return this->p_maybeCrate;
}

Optional<NotNull<const AbstractTrackItem*>> TrackListNode::maybeTrackItem() const
{
    return this->p_maybeTrackItem.isValid() ? NotNull<const AbstractTrackItem*>{ this->p_maybeTrackItem->asRawPointer() } : Optional<NotNull<const AbstractTrackItem*>>{ };
}

Optional<NotNull<AbstractTrackItem*>> TrackListNode::maybeTrackItem()
{
    return this->p_maybeTrackItem.isValid() ? NotNull<AbstractTrackItem*>{ this->p_maybeTrackItem->asRawPointer() } : Optional<NotNull<AbstractTrackItem*>>{ };
}

count TrackListNode::getRowInParent() const
{
    auto maybeParent = this->p_maybeParent;
    if (maybeParent.isValid()) {
        auto& parent = *maybeParent;

        for (count index = 0; index < parent->childCount(); ++ index) {
            auto maybeChild = parent->maybeChildNode(index);
            if (maybeChild.isValid() && ((*maybeChild).get() == this)) {
                return index;
            }
        }
    }

    NXA_ALOG("Can't find us in our parent.");
}

QString TrackListNode::getSectionName() const
{
    return this->p_name;
}

Optional<count> TrackListNode::getTrackIndex(NotNull<AbstractTrackItem*> identity) const
{
    auto directIdentity = dynamic_cast<TrackItem*>(identity.get());
    if (directIdentity == nullptr) {
        return nothing;
    }

    for (count index = 0; index < this->p_children.length(); ++index) {
        auto track = this->p_children[index]->maybeTrack();
        if (track && directIdentity->getTrack() == *track) {
            return index;
        }
    }

    return nothing;
}

void TrackListNode::refreshTrackList()
{
    for (auto&& child : this->p_children) {
        child->p_maybeParent = nothing;
    }

    this->p_children.removeAll();

    auto tracks = this->p_refresh();
    if (tracks.isValid()) {
        tracks->apply(RefreshVisitor{ this });
    }
}

Optional<NotNull<const Common::Track*>> TrackListNode::maybeTrack() const
{
    if (this->p_maybeTrackItem.isValid()) {
        auto asMutableTrackItem = dynamic_cast<const TrackItem*>((*this->p_maybeTrackItem).asRawPointer());
        if (asMutableTrackItem != nullptr) {
            return asMutableTrackItem->getTrack();
        }
    }

    return nothing;
}
