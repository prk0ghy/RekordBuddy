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

#include <AbstractViewCollection/Sidebar/SidebarItem.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class WorkQueue;
class SidebarModel;

class AbstractCollection : public SidebarItem
{
public:
    // -- Constructors & Destructors
    AbstractCollection(NotNull<SidebarModel*>, SidebarItem* parent, WorkQueue&);

    // -- Instance Methods
    boolean canBeDragged() const override
    {
        return true;
    }

    boolean isSelectable() const override
    {
        return true;
    }
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::Collection;
    }

    virtual boolean saveChangesWithProgress(std::function<void(double)>&&) = 0;
    virtual void notifyUserPreferencesHaveChanged() = 0;
    virtual boolean isOpened() const = 0;
    virtual boolean hasChangesToSave() const = 0;

    virtual boolean offerToRemove() = 0;
};

} }
