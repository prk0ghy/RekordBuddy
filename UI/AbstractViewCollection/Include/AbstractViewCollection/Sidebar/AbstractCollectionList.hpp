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

#pragma once

#include <AbstractViewCollection/Sidebar/CollectionOpenError.hpp>
#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

#include <Base/FilePath.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class WorkQueue;

// -- Public Interface
class AbstractCollectionList : public SidebarItem
{
public:
    // -- Constructors & Destructors
    AbstractCollectionList(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&);
    virtual boolean saveChangesWithProgress(std::function<void(double)>&&) = 0;
    virtual boolean hasChangesToSave() const = 0;
    virtual Array<CollectionOpenError> refreshCollectionsWithPerCollectionProgressCallBack(const std::function<void(void)>& callback) = 0;
};

} }
