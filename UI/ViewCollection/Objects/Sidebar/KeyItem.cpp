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

#include <ViewCollection/Sidebar/KeyItem.hpp>
#include <ViewCollection/TrackList/TrackListNode.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

KeyItem::KeyItem(NotNull<SidebarModel*> model,
                 SidebarItem* parent,
                 WorkQueue& withQueue,
                 NotNull<Common::MusicalKey*> withKey) : SidebarItem{ model,
                                                                      parent,
                                                                      withQueue },
                                                         p_key{ std::move(withKey) }
{

}

QString KeyItem::getName() const
{
    return fromString(this->p_key->stringValue());
}

Optional<QIcon> KeyItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Musical Key/key.png" };
    return cachedIcon;
}

Optional<QString> KeyItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_key->collection()->type())) };
}

Optional<QString> KeyItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_key->collection()->fullName()) };
}

Optional<Volume> KeyItem::maybeParentCollectionVolume() const
{
    return this->p_key->collection()->volume();
}

std::future<Optional<Shared<AbstractTrackListNode>>> KeyItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(Shared<TrackListNode>::with(nothing,
                                                                                          nothing,
                                                                                          [this]() -> TrackListNode::RefreshType {
                                                                                              return { this->p_key->tracks() };
                                                                                          },
                                                                                          ""));
        }

        return this->p_treeCache;
    });
}

boolean KeyItem::isSelectable() const
{
    return true;
}
