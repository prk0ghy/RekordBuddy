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

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Artist.hpp>

#include <Base/Array.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>
#include <memory>

#pragma once

namespace NxA::RekordBuddy {

class MutableArtistsHeader : public SidebarItem
{
    NotNull<Common::MutableCollection*> p_collection;
    mutable Optional<Array<Unique<Common::MutableArtist>>> p_artists;

protected:
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    MutableArtistsHeader(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&, NotNull<Common::MutableCollection*>);

    void forceRefresh() override;

    QString getName() const override;
    count childCount() const override;

    void maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>&) override;
};

}
