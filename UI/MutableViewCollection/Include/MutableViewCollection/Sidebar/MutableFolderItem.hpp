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

#include <CommonCollection/Crates/Folder.hpp>

#include <Base/Array.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <qvariant.h>
#include <memory>

class QIcon;

#pragma once

namespace NxA::RekordBuddy {

class AbstractTrackListItem;
class SidebarItem;

class MutableFolderItem : public AbstractCrateItem
{
    NotNull<Common::MutableFolder*> p_folder;

protected:
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    MutableFolderItem() = delete;
    explicit MutableFolderItem(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&, NotNull<Common::MutableFolder*>);
    ~MutableFolderItem() override = default;

    NotNull<const Common::MutableFolder*> getFolder() const;
    NotNull<Common::MutableFolder*> getFolder();

    boolean canBeRenamed() const override;
    boolean canBeDeleted() const override;
    boolean canBeCloned() const override;
    boolean canAddPlaylistChildren() const override;
    boolean canAddFolderChildren() const override;
    boolean isSelectable() const override;
    TypeOfSelection typeOfSelectionForThis() const override
    {
        return TypeOfSelection::Crate;
    }
    boolean canReceiveCrates() const override;
    boolean canBeDragged() const override;
    boolean canReceive(NotNull<AbstractCrateItem*>) const override;
    Optional<count> receiveDraggedCrateAtIndexWithPerItemProgressCallBack(NotNull<AbstractCrateItem*>,
                                                                          count,
                                                                          std::function<void(void)>&&,
                                                                          AndOverWriteExisting) override;

    Optional<QIcon> maybeIcon() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
    boolean parentCollectionMustHaveTracksOnTheSameVolume() const override;
    boolean hasTracksNotOnVolume(const Volume&) const override;
    boolean parentCollectionAllowsMovieTracksInPlaylists() const override;
    boolean hasMovieTracks() const override;

    void deleteCrate() override;
    void cloneCrate() override;

    void createChildFolder(const String& name) override;
    void createChildPlaylist(const String& name) override;
    void createChildSmartPlaylist(const String& name) override;

    count numberOfCratesAndTrackEntriesContainedWithin() const override;
    count numberOfTracks() const override;

    count crateCount() const override;
    void setName(const QString& name) override;
    QString getName() const override;

    std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree() override;
};
}
