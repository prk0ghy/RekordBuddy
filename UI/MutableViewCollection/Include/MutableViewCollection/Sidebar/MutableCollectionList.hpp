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

#include <AbstractViewCollection/Sidebar/AbstractCollectionList.hpp>

#include <CommonCollection/Collection.hpp>

#include <Base/Array.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <future>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class WorkQueue;
class AbstractCollection;

// -- Public Interface
class MutableCollectionList : public AbstractCollectionList
{
    // -- Private Instance Methods
    Shared<AbstractCollection> p_viewCollectionWith(Shared<Common::Collection>);
    Shared<AbstractCollection> p_viewCollectionWith(Shared<Common::MutableCollection>);

protected:
    // -- Protected Instance Variables
    MutableArray<Shared<AbstractCollection>> p_collectionsInSidebar;

    MutableArray<Common::CollectionOfSomeSort> p_currentCollections;

    // -- Protected Instance Methods
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

    void removeChildAtIndex(count index) override
    {
        this->SidebarItem::removeChildAtIndex(index);

        this->p_currentCollections.removeObjectAtIndex(index);
        this->p_collectionsInSidebar.removeObjectAtIndex(index);
    }

public:
    // -- Constructors & Destructors
    explicit MutableCollectionList(NotNull<SidebarModel*>, WorkQueue&);
    ~MutableCollectionList() override = default;

    // -- Instance Methods
    count childCount() const override;
    QString getName() const override;
    Array<CollectionOpenError> refreshCollectionsWithPerCollectionProgressCallBack(const std::function<void(void)>& callback) override;
    boolean saveChangesWithProgress(std::function<void(double)>&&) override;
    void notifyUserPreferencesHaveChanged();
    boolean hasChangesToSave() const override;

    boolean canReceiveFiles() const override
    {
        return true;
    }
    boolean receiveFileAt(const FilePath&) override;

    void maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>&) override;
};

} }
