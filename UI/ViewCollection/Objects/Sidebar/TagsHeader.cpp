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

#include <ViewCollection/Sidebar/TagsHeader.hpp>
#include <ViewCollection/Sidebar/TrackTagItem.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Collection.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

TagsHeader::TagsHeader(NotNull<SidebarModel*> model,
                       SidebarItem* parent,
                       WorkQueue& withQueue,
                       NotNull<Common::Collection*> withCollection) : SidebarItem{ model,
                                                                                   parent,
                                                                                   withQueue }, p_collection{ withCollection }
{

}

// -- Instance Methods

InternalItemReference TagsHeader::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<TagsHeader*>(this);

    if (!mutableThis->p_tags.isValid()) {
        mutableThis->p_tags = mutableThis->p_collection->tags();
    }

    return { (*mutableThis->p_tags)[index].asRawPointer() };
}

Shared<SidebarItem> TagsHeader::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<TagsHeader*>(this);

    if (!mutableThis->p_tags.isValid()) {
        mutableThis->p_tags = mutableThis->p_collection->tags();
    }

    return Shared<TrackTagItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_tags)[index].asRawPointer());
}

QString TagsHeader::getName() const
{
    return QObject::tr("TAGS");
}

count TagsHeader::childCount() const
{
    if (!this->p_tags.isValid()) {
        this->p_tags = this->p_collection->tags();
    }

    return this->p_tags->length();
}
