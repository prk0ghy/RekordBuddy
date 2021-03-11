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

#include <MutableViewCollection/Sidebar/MutableKeyItem.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

MutableKeyItem::MutableKeyItem(NotNull<SidebarModel*> model,
                               SidebarItem* parent,
                               WorkQueue& withQueue,
                               NotNull<Common::MutableMusicalKey*> withKey) : SidebarItem{ model,
                                                                                           parent,
                                                                                           withQueue },
                                                                              p_key{ std::move(withKey) }
{

}

QString MutableKeyItem::getName() const
{
    return fromString(this->p_key->stringValue());
}

Optional<QIcon> MutableKeyItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Musical Key/key.png" };
    return cachedIcon;
}

Optional<QString> MutableKeyItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_key->collection()->type())) };
}

Optional<QString> MutableKeyItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_key->collection()->fullName()) };
}

Optional<Volume> MutableKeyItem::maybeParentCollectionVolume() const
{
    return this->p_key->collection()->volume();
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutableKeyItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(Shared<MutableTrackListNode>::with(nothing,
                                                                                                 nothing,
                                                                                                 [this]() -> MutableTrackListNode::RefreshType {
                                                                                                     return { this->p_key->tracks() };
                                                                                                 },
                                                                                                 ""));
        }

        return this->p_treeCache;
    });
}

boolean MutableKeyItem::isSelectable() const
{
    return true;
}
