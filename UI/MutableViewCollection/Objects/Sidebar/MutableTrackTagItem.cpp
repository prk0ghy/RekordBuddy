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

#include <MutableViewCollection/Sidebar/MutableTrackTagItem.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Tracks/Tag.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

MutableTrackTagItem::MutableTrackTagItem(NotNull<SidebarModel*> model,
                                         SidebarItem* parent,
                                         WorkQueue& withQueue,
                                         NotNull<Common::MutableTag*> withTag) : SidebarItem{ model,
                                                                                              parent,
                                                                                              withQueue },
                                                                                 p_tag{ std::move(withTag) }
{

}

// -- Instance Methods

InternalItemReference MutableTrackTagItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<MutableTrackTagItem*>(this);

    if (!mutableThis->p_maybeChildren.isValid()) {
        mutableThis->p_maybeChildren = mutableThis->p_tag->children();
    }

    return { (*mutableThis->p_maybeChildren)[index].asRawPointer() };
}

Shared<SidebarItem> MutableTrackTagItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutableTrackTagItem*>(this);

    if (!mutableThis->p_maybeChildren.isValid()) {
        mutableThis->p_maybeChildren = mutableThis->p_tag->children();
    }

    return Shared<MutableTrackTagItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_maybeChildren)[index].asRawPointer());
}

QString MutableTrackTagItem::getName() const
{
    return fromString(this->p_tag->value());
}

Optional<QIcon> MutableTrackTagItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Tag White/tag white.png" };
    return cachedIcon;
}

Optional<QString> MutableTrackTagItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_tag->collection()->type())) };
}

Optional<QString> MutableTrackTagItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_tag->collection()->fullName()) };
}

Optional<Volume> MutableTrackTagItem::maybeParentCollectionVolume() const
{
    return this->p_tag->collection()->volume();
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutableTrackTagItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_maybeTreeCache.isValid()) {
            this->p_maybeTreeCache = Shared<AbstractTrackListNode>(Shared<MutableTrackListNode>::with(nothing,
                                                                                                      nothing,
                                                                                                      [this]() -> MutableTrackListNode::RefreshType {
                                                                                                          return { this->p_tag->tracks() };
                                                                                                      },
                                                                                                      ""));
        }

        return this->p_maybeTreeCache;
    });
}

boolean MutableTrackTagItem::isSelectable() const
{
    return true;
}

count MutableTrackTagItem::childCount() const
{
    if (!this->p_maybeChildren.isValid()) {
        this->p_maybeChildren = this->p_tag->children();
    }

    return this->p_maybeChildren->length();
}
