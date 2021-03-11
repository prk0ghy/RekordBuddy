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

#include <ViewCollection/Sidebar/TrackTagItem.hpp>
#include <ViewCollection/TrackList/TrackListNode.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Tracks/Tag.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

TrackTagItem::TrackTagItem(NotNull<SidebarModel*> model,
                           SidebarItem* parent,
                           WorkQueue& withQueue,
                           NotNull<Common::Tag*> withTag) : SidebarItem{ model,
                                                                         parent,
                                                                         withQueue },
                                                            p_tag{ std::move(withTag) }
{

}

// -- Instance Methods

InternalItemReference TrackTagItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<TrackTagItem*>(this);

    if (!mutableThis->p_children) {
        mutableThis->p_children = mutableThis->p_tag->children();
    }

    return { (*mutableThis->p_children)[index].asRawPointer() };
}

Shared<SidebarItem> TrackTagItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<TrackTagItem*>(this);

    if (!mutableThis->p_children) {
        mutableThis->p_children = mutableThis->p_tag->children();
    }

    return Shared<TrackTagItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_children)[index].asRawPointer());
}

QString TrackTagItem::getName() const
{
    return fromString(this->p_tag->value());
}

Optional<QIcon> TrackTagItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Tag White/tag white.png" };
    return cachedIcon;
}

Optional<QString> TrackTagItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_tag->collection()->type())) };
}

Optional<QString> TrackTagItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_tag->collection()->fullName()) };
}

Optional<Volume> TrackTagItem::maybeParentCollectionVolume() const
{
    return this->p_tag->collection()->volume();
}

std::future<Optional<Shared<AbstractTrackListNode>>> TrackTagItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(Shared<TrackListNode>::with(nothing,
                                                                                          nothing,
                                                                                          [this]() -> TrackListNode::RefreshType {
                                                                                              return { this->p_tag->tracks() };
                                                                                          },
                                                                                          ""));
        }

        return this->p_treeCache;
    });
}

boolean TrackTagItem::isSelectable() const
{
    return true;
}

count TrackTagItem::childCount() const
{
    if (!this->p_children) {
        this->p_children = this->p_tag->children();
    }

    return this->p_children->length();
}
