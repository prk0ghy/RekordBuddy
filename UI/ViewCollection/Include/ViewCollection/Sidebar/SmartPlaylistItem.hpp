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

#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <CommonCollection/Crates/SmartPlaylist.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>

#include <memory>

class QIcon;

#pragma once

namespace NxA::RekordBuddy {

class AbstractTrackListItem;
class AbstractPredicateItem;
class MainWindow;

class SmartPlaylistItem : public AbstractCrateItem
{
    NotNull<const Common::SmartPlaylist*> p_smartPlaylist;
    mutable Optional<Shared<AbstractTrackListNode>> p_treeCache;

    mutable Optional<QIcon> maybeCachedIcon;

protected:
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    SmartPlaylistItem() = delete;
    explicit SmartPlaylistItem(NotNull<SidebarModel*>, AbstractCrateItem*, WorkQueue&, NotNull<const Common::SmartPlaylist*>);
    ~SmartPlaylistItem() override = default;

    // -- Instance Methods
    void forceRefresh() override
    {
        if (this->p_treeCache.isValid()) {
            (*this->p_treeCache)->refreshTrackList();
        }

        this->SidebarItem::forceRefresh();
    }

    NotNull<const Common::SmartPlaylist*> getPlaylist() const;

    void populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*>, boolean) override;
    Optional<count> addTrackAtIndex(NotNull<AbstractTrackItem*>, count) override;

    count numberOfCratesAndTrackEntriesContainedWithin() const override;
    count numberOfTracks() const override;

    count crateCount() const override;

    void deleteCrate() override;
    void cloneCrate() override;
    boolean canEditPredicates() const override;
    boolean canBeDeleted() const override;
    boolean canBeCloned() const override;
    boolean canAddOrganization() const override;
    boolean isSelectable() const override;
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::Crate;
    }
    boolean canReceiveTracks() const override;

    Optional<QIcon> maybeIcon() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
    boolean hasTracksNotOnVolume(const Volume&) const override;

    QString getName() const override;
    void setName(const QString&) override;
    std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree() override;
    Optional<std::unique_ptr<AbstractGroupItem>> predicates() override;

    boolean isOrganizedBy(Common::Property::TypeID) const override;
    void organizeBy(Common::Property::TypeID) override;
    void removeOrganizationBy(Common::Property::TypeID) override;

    boolean isOrganizedBy(const Common::Tag&) const override;
    void organizeBy(Common::MutableTag&) override;
    void removeOrganizationBy(Common::MutableTag&) override;
};

}
