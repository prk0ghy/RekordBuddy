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

#include <ViewCollection/Sidebar/KeysHeader.hpp>
#include <ViewCollection/Sidebar/KeyItem.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Collection.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

KeysHeader::KeysHeader(NotNull<SidebarModel*> model,
                       SidebarItem* parent,
                       WorkQueue& withQueue,
                       NotNull<Common::Collection*> withCollection) : SidebarItem{ model,
                                                                                   parent,
                                                                                   withQueue },
                                                                      p_collection{ withCollection }
{

}

// -- Instance Methods

InternalItemReference KeysHeader::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<KeysHeader*>(this);

    if (!mutableThis->p_keys) {
        mutableThis->p_keys = mutableThis->p_collection->musicalKeys();
    }

    return { (*mutableThis->p_keys)[index].asRawPointer() };
}

Shared<SidebarItem> KeysHeader::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<KeysHeader*>(this);

    if (!mutableThis->p_keys) {
        mutableThis->p_keys = mutableThis->p_collection->musicalKeys();
    }

    return Shared<KeyItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_keys)[index].asRawPointer());
}

QString KeysHeader::getName() const
{
    return QObject::tr("KEYS");
}

count KeysHeader::childCount() const
{
    if (!this->p_keys) {
        this->p_keys = this->p_collection->musicalKeys();
    }

    return this->p_keys->length();
}
