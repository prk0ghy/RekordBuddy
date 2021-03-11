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

#include <CommonCollection/Collection.hpp>

#include <Base/Pointers.hpp>

namespace NxA { namespace RekordBuddy {

class WorkQueue;
class MainWindow;

class AbstractCrateItem : public SidebarItem
{
private slots:
    // -- Private Slots
    void renameSelectedCrate();

protected:
    // -- Protected Instance Methods
    void removeChildAtIndex(count index) override
    {
        // -- We need to keep a shared pointer to the child so it doesn't get deleted before we want it to.
        auto childItem = this->p_cachedChildAtIndex(index);

        this->SidebarItem::removeChildAtIndex(index);

        auto maybeCrateItem = Pointer<SidebarItem>{ childItem.asRawPointer() }.maybeAs<AbstractCrateItem>();
        NXA_ASSERT_TRUE(maybeCrateItem.isValid());

        (*maybeCrateItem).asReference().deleteCrate();
    }

public:
    // -- Constructors & Destructors
    AbstractCrateItem(NotNull<SidebarModel*>, SidebarItem* parent, WorkQueue& queue);
    ~AbstractCrateItem() override = default;

    // -- Instance Methods
    virtual boolean isAnAllTracksPlaylist() const
    {
        return false;
    }

    count childCount() const final;
    boolean canBeRenamed() const override;
    void populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*>, boolean onlyAdd) override;

    virtual count crateCount() const = 0;
    virtual boolean canUserOrderTracks() const
    {
        return false;
    }
    virtual boolean canUserRemoveTracks() const
    {
        return false;
    }
    virtual void moveTracksAtIndicesToIndex(Array<count>, count)
    {
        NXA_ALOG("Should not be able to reorder this.");
    }
    virtual boolean maybeConfirmTrackDeletion() const
    {
        // -- By default we always go ahead and delete the track
        return true;
    }
    virtual void removeTrackAtIndex(count)
    {
        NXA_ALOG("Should not be able to remove tracks from this.");
    }
    virtual void deleteCrate();
    virtual void cloneCrate();
    virtual void setPredicateGroup(NotNull<AbstractGroupItem*>);
    virtual void createChildFolder(const String& name);
    virtual void createChildPlaylist(const String& name);
    virtual void createChildSmartPlaylist(const String& name);

    virtual boolean isOrganizedBy(Common::Property::TypeID) const;
    virtual void organizeBy(Common::Property::TypeID);
    virtual void removeOrganizationBy(Common::Property::TypeID);

    virtual boolean isOrganizedBy(const Common::Tag&) const;
    virtual void organizeBy(Common::MutableTag&);
    virtual void removeOrganizationBy(Common::MutableTag&);
};

namespace NxADetail {

using CrateType = std::tuple<QModelIndex, NotNull<AbstractCrateItem*>>;
extern Optional<Shared<MutableArray<CrateType>>> maybeDraggedCrateObjects;

}

} }
