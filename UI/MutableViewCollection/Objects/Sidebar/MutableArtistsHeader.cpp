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

#include <MutableViewCollection/Sidebar/MutableArtistsHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableArtistItem.hpp>

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <UiModel/SidebarModel.hpp>

#include <CommonCollection/Collection.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

MutableArtistsHeader::MutableArtistsHeader(NotNull<SidebarModel*> model,
                                           SidebarItem* parent,
                                           WorkQueue& withQueue,
                                           NotNull<Common::MutableCollection*> withCollection) : SidebarItem{ model,
                                                                                                              parent,
                                                                                                              withQueue },
                                                                                                 p_collection{ withCollection }
{

}

// -- Instance Methods

void MutableArtistsHeader::forceRefresh()
{
    auto currentNumberOfChildren = this->childCount();
    if (!currentNumberOfChildren) {
        return;
    }

    auto model = this->getModel();
    auto modelIndex = this->getModelIndex();

    model->beginRemoveRows(modelIndex, 0, currentNumberOfChildren - 1);

    this->p_artists = nothing;

    this->SidebarItem::forceRefresh();

    model->endRemoveRows();

    model->notifyLayoutChangedAt(modelIndex);
}

InternalItemReference MutableArtistsHeader::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<MutableArtistsHeader*>(this);

    if (!mutableThis->p_artists.isValid()) {
        mutableThis->p_artists = mutableThis->p_collection->artists();
    }

    return { (*this->p_artists)[index].asRawPointer() };
}

Shared<SidebarItem> MutableArtistsHeader::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutableArtistsHeader*>(this);

    if (!mutableThis->p_artists.isValid()) {
        mutableThis->p_artists = mutableThis->p_collection->artists();
    }

    return Shared<MutableArtistItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, (*mutableThis->p_artists)[index].asRawPointer());
}

QString MutableArtistsHeader::getName() const
{
    return QObject::tr("ARTISTS");
}

count MutableArtistsHeader::childCount() const
{
    if (!this->p_artists.isValid()) {
        this->p_artists = this->p_collection->artists();
    }

    return this->p_artists->length();
}

void MutableArtistsHeader::maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>& tracks)
{
    // -- For now we just force reset the entire list but this should be way more granular in the future.
    this->forceRefresh();
}
