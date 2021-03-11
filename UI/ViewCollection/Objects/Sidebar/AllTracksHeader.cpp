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

#include <ViewCollection/Sidebar/AllTracksHeader.hpp>
#include <ViewCollection/Sidebar/AllTracksItem.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Collection.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

AllTracksHeader::AllTracksHeader(NotNull<SidebarModel*> model,
                                 SidebarItem* parent,
                                 WorkQueue& withQueue,
                                 NotNull<const Common::Collection*> withCollection) : SidebarItem{ model,
                                                                                                   parent,
                                                                                                   withQueue },
                                                                                      p_collection{ withCollection }
{

}

// -- Instance Methods

InternalItemReference AllTracksHeader::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<AllTracksHeader*>(this);

    if (!mutableThis->p_maybeAllTracksPlaylist.isValid()) {
        mutableThis->p_maybeAllTracksPlaylist = mutableThis->p_collection->tracks();
    }

    return { mutableThis->p_maybeAllTracksPlaylist->get() };
}

Shared<SidebarItem> AllTracksHeader::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<AllTracksHeader*>(this);

    if (!this->p_maybeAllTracksPlaylist.isValid()) {
        mutableThis->p_maybeAllTracksPlaylist = mutableThis->p_collection->tracks();
    }

    return Shared<AllTracksItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, this->p_collection);
}

QString AllTracksHeader::getName() const
{
    return QObject::tr("ALL TRACKS");
}

count AllTracksHeader::childCount() const
{
    return 1;
}
