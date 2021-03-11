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

#include <ViewCollection/Sidebar/ArtistsHeader.hpp>
#include <ViewCollection/Sidebar/ArtistItem.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Collection.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

ArtistsHeader::ArtistsHeader(NotNull<SidebarModel*> model,
                             SidebarItem* parent,
                             WorkQueue& withQueue,
                             NotNull<Common::Collection*> withCollection) : SidebarItem{ model,
                                                                                         parent,
                                                                                         withQueue },
                                                                            p_collection{ withCollection }
{

}

// -- Instance Methods

InternalItemReference ArtistsHeader::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<ArtistsHeader*>(this);

    if (!mutableThis->p_artists.isValid()) {
        mutableThis->p_artists = mutableThis->p_collection->artists();
    }

    return { (*mutableThis->p_artists)[index].asRawPointer() };
}

Shared<SidebarItem> ArtistsHeader::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<ArtistsHeader*>(this);

    if (!mutableThis->p_artists.isValid()) {
        mutableThis->p_artists = mutableThis->p_collection->artists();
    }

    return Shared<ArtistItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_artists)[index].asRawPointer());
}

QString ArtistsHeader::getName() const
{
    return QObject::tr("ARTISTS");
}

count ArtistsHeader::childCount() const
{
    if (!this->p_artists.isValid()) {
        this->p_artists = this->p_collection->artists();
    }

    return this->p_artists->length();
}
