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

#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>
#include <MutableViewCollection/TrackList/MutableTrackItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/SmartPlaylist.hpp>

#include <Base/Base.hpp>

#include <QDataStream>
#include <list>
#include <type_traits>
#include <utility>

using namespace NxA;
using namespace RekordBuddy;

struct NxA::RekordBuddy::RefreshVisitor
{
    NotNull<MutableTrackListNode*> that;

    void operator()(Array<NotNull<Common::MutableTrack*>>& tracks)
    {
        for (auto&& track : tracks) {
            this->that->p_children.append(Shared<MutableTrackListNode>::with(this->that, std::move(track)));
        }
    }

    void operator()(Array<Common::MutableSubCrate>& subCrates)
    {
        for (auto&& subCrate : subCrates) {
            this->that->p_children.append(subCrate.apply([this](auto& subCrate) {
                return Shared<MutableTrackListNode>::with(this->that->p_maybeCrate, this->that,
                                                          [&subCrate]() -> MutableTrackListNode::RefreshType {
                                                              return { subCrate->tracks() };
                                                          },
                                                          fromString(subCrate->name()));
            }));
        }
    }
};

// -- Constructor & Destructors

MutableTrackListNode::MutableTrackListNode(Optional<NotNull<AbstractCrateItem*>> withCrate,
                                           Optional<NotNull<MutableTrackListNode*>> withParent,
                                           std::function<RefreshType()>&& withTracks,
                                           QString withName) : p_refresh{ std::move(withTracks) },
                                                               p_maybeCrate{ withCrate },
                                                               p_name{ std::move(withName) },
                                                               p_maybeParent{ withParent }
{
    this->refreshTrackList();
}

MutableTrackListNode::MutableTrackListNode(Optional<NotNull<MutableTrackListNode*>> withParent,
                                           NotNull<Common::MutableTrack*>&& withTrack) : p_maybeParent{ withParent }
{
    this->p_maybeTrackItem = Shared<AbstractTrackItem>{ Shared<MutableTrackItem>::with(nothing, std::move(withTrack)) };
}

// -- Overridden Instance Methods

Optional<NotNull<const AbstractTrackListNode*>> MutableTrackListNode::maybeChildNode(count index) const
{
    if (index < this->p_children.length()) {
        return NotNull<const AbstractTrackListNode*>{ this->p_children[index].asRawPointer() };
    }

    return nothing;
}

Optional<NotNull<AbstractTrackListNode*>> MutableTrackListNode::maybeChildNode(count index)
{
    if (index < this->p_children.length()) {
        return NotNull<AbstractTrackListNode*>{ this->p_children[index].asRawPointer() };
    }

    return nothing;
}

Optional<NotNull<const AbstractTrackListNode*>> MutableTrackListNode::maybeParent() const
{
    return this->p_maybeParent.isValid() ? this->p_maybeParent->as<const AbstractTrackListNode*>() : Optional<NotNull<const AbstractTrackListNode*>>{ };
}

Optional<NotNull<AbstractTrackListNode*>> MutableTrackListNode::maybeParent()
{
    return this->p_maybeParent.isValid() ? this->p_maybeParent->as<AbstractTrackListNode*>() : Optional<NotNull<AbstractTrackListNode*>>{ };
}

boolean MutableTrackListNode::hasParentItem() const
{
    return this->p_maybeParent.isValid();
}

Optional<NotNull<const AbstractCrateItem*>> MutableTrackListNode::maybeCrateItem() const
{
    return this->p_maybeCrate.isValid() ? this->p_maybeCrate->as<const AbstractCrateItem*>() : Optional<NotNull<const AbstractCrateItem*>>{ };
}

Optional<NotNull<AbstractCrateItem*>> MutableTrackListNode::maybeCrateItem()
{
    return this->p_maybeCrate;
}

Optional<NotNull<const AbstractTrackItem*>> MutableTrackListNode::maybeTrackItem() const
{
    return this->p_maybeTrackItem.isValid() ? NotNull<const AbstractTrackItem*>{ this->p_maybeTrackItem->asRawPointer() } : Optional<NotNull<const AbstractTrackItem*>>{ };
}

Optional<NotNull<AbstractTrackItem*>> MutableTrackListNode::maybeTrackItem()
{
    return this->p_maybeTrackItem.isValid() ? NotNull<AbstractTrackItem*>{ this->p_maybeTrackItem->asRawPointer() } : Optional<NotNull<AbstractTrackItem*>>{ };
}

count MutableTrackListNode::childCount() const
{
    return this->p_children.length();
}

Pointer<const AbstractTrackListNode> MutableTrackListNode::getChildAtIndex(count index) const
{
    return { this->p_children[index].asRawPointer() };
}

Pointer<AbstractTrackListNode> MutableTrackListNode::getChildAtIndex(count index)
{
    return { this->p_children[index].asRawPointer() };
}

boolean MutableTrackListNode::canUserOrderTracks() const
{
    if (!this->p_maybeCrate.isValid()) {
        return false;
    }

    return (*this->p_maybeCrate)->canUserOrderTracks();
}

void MutableTrackListNode::moveTracksAtIndicesToIndex(Array<count> trackIndices, count toIndex)
{
    (*this->p_maybeCrate)->moveTracksAtIndicesToIndex(trackIndices, toIndex);

    this->p_children.moveObjectsAtIndicesTo(trackIndices, toIndex);
}

count MutableTrackListNode::getRowInParent() const
{
    auto maybeParent = this->p_maybeParent;
    if (maybeParent.isValid()) {
        auto& parent = *maybeParent;
        for (count index = 0; index < parent->childCount(); ++ index) {
            auto maybeChild = parent->maybeChildNode(index);
            if (maybeChild.isValid() && maybeChild->get() == this) {
                return index;
            }
        }
    }

    NXA_ALOG("Can't find us in our parent.");
}

QString MutableTrackListNode::getSectionName() const
{
    return this->p_name;
}

Optional<count> MutableTrackListNode::getTrackIndex(NotNull<AbstractTrackItem*> identity) const
{
    auto directIdentity = dynamic_cast<MutableTrackItem*>(identity.get());
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

void MutableTrackListNode::refreshTrackList()
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

Optional<NotNull<const Common::MutableTrack*>> MutableTrackListNode::maybeTrack() const
{
    if (this->p_maybeTrackItem.isValid()) {
        auto asMutableTrackItem = dynamic_cast<const MutableTrackItem*>((*this->p_maybeTrackItem).asRawPointer());
        if (asMutableTrackItem != nullptr) {
            return asMutableTrackItem->getTrack();
        }
    }

    return nothing;
}

Optional<NotNull<Common::MutableTrack*>> MutableTrackListNode::maybeTrack()
{
    if (this->p_maybeTrackItem.isValid()) {
        auto asMutableTrackItem = dynamic_cast<MutableTrackItem*>((*this->p_maybeTrackItem).asRawPointer());
        if (asMutableTrackItem != nullptr) {
            return asMutableTrackItem->getTrack();
        }
    }

    return nothing;
}
