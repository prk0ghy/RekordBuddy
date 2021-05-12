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

#include <MutableViewCollection/Sidebar/MutableFolderItem.hpp>
#include <MutableViewCollection/Sidebar/MutablePlaylistItem.hpp>
#include <MutableViewCollection/Sidebar/Traits.hpp>

#include <ViewCollection/Sidebar/FolderItem.hpp>
#include <ViewCollection/Sidebar/PlaylistItem.hpp>

#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <UiModel/SidebarModel.hpp>

#include <CommonCollection/Collection.hpp>

#include <Base/Types.hpp>

#include <qicon.h>
#include <qstring.h>

#include <type_traits>

namespace NxA::RekordBuddy {

class SidebarItem;

}

using namespace NxA;
using namespace RekordBuddy;

MutableFolderItem::MutableFolderItem(NotNull<SidebarModel*> model,
                                     SidebarItem* parent,
                                     WorkQueue& withQueue,
                                     NotNull<Common::MutableFolder*> withFolder) : AbstractCrateItem{ model,
                                                                                                      parent,
                                                                                                      withQueue },
                                                                                   p_folder{ std::move(withFolder) } { }

InternalItemReference MutableFolderItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<MutableFolderItem*>(this);
    return mutableThis->p_folder->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> MutableFolderItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutableFolderItem*>(this);
    return mutableThis->p_folder->subCrateAtIndex(index).apply(CrateVisitor<MutableFolderItem>{ mutableThis,
                                                                                                mutableThis->p_queue,
                                                                                                mutableThis->getModel() });
}

NotNull<const Common::MutableFolder*> MutableFolderItem::getFolder() const
{
    return this->p_folder;
}

NotNull<Common::MutableFolder*> MutableFolderItem::getFolder()
{
    return this->p_folder;
}

void MutableFolderItem::createChildFolder(const String& name)
{
    this->p_folder->newFolderWithName(name);
}

void MutableFolderItem::createChildPlaylist(const String& name)
{
    this->p_folder->newPlaylistWithName(name);
}

void MutableFolderItem::createChildSmartPlaylist(const String& name)
{
    this->p_folder->newSmartPlaylistWithName(name);
}

void MutableFolderItem::setName(const QString& name)
{
    this->p_folder->setName(fromString(name));
}

QString MutableFolderItem::getName() const
{
    return fromString(this->p_folder->name());
}

boolean MutableFolderItem::canBeRenamed() const
{
    return true;
}

boolean MutableFolderItem::canBeDeleted() const
{
    return true;
}

boolean MutableFolderItem::canBeCloned() const
{
    return true;
}

boolean MutableFolderItem::canAddPlaylistChildren() const
{
    return true;
}


boolean MutableFolderItem::canBeDragged() const
{
    return true;
}

boolean MutableFolderItem::canAddFolderChildren() const
{
    return true;
}

boolean MutableFolderItem::isSelectable() const
{
    return true;
}

boolean MutableFolderItem::canReceiveCrates() const
{
    return true;
}

boolean MutableFolderItem::canReceive(NotNull<AbstractCrateItem*> item) const
{
    auto maybeFolderItem = dynamic_cast<FolderItem*>(item.get());
    if (maybeFolderItem != nullptr) {
        return this->p_folder->canReceive(maybeFolderItem->getFolder());
    }

    auto maybeMutableFolderItem = dynamic_cast<MutableFolderItem*>(item.get());
    if (maybeMutableFolderItem != nullptr) {
        return this->p_folder->canReceive(maybeMutableFolderItem->getFolder());
    }

    auto maybePlaylistItem = dynamic_cast<PlaylistItem*>(item.get());
    if (maybePlaylistItem != nullptr) {
        return this->p_folder->canReceive(maybePlaylistItem->getPlaylist());
    }

    auto maybeMutablePlaylistItem = dynamic_cast<MutablePlaylistItem*>(item.get());
    if (maybeMutablePlaylistItem != nullptr) {
        return this->p_folder->canReceive(const_cast<MutablePlaylistItem*>(maybeMutablePlaylistItem)->getPlaylist());
    }

    return false;
}

void MutableFolderItem::deleteCrate()
{
    Common::MutableFolder::removeFromParent(this->p_folder);
}

void MutableFolderItem::cloneCrate()
{
    this->p_folder->cloneWithName(String::stringWithFormat("Clone of %s", this->p_folder->name().asUTF8()));
}

Optional<count> MutableFolderItem::receiveDraggedCrateAtIndexWithPerItemProgressCallBack(NotNull<AbstractCrateItem*> draggedItem,
                                                                                         count toIndex,
                                                                                         std::function<void(void)>&& callback,
                                                                                         AndOverWriteExisting andOverWriteExisting)
{
    Optional<Common::MutableFolder::SourceCrate> maybeDraggedCrate;
    Optional<Common::MutableFolder::MutableSourceCrate> maybeMutableDraggedCrate;
    Optional<NotNull<const Common::Collection*>> maybeOtherCollection;

    auto maybeFolderItem = dynamic_cast<FolderItem*>(draggedItem.get());
    if (maybeFolderItem != nullptr) {
        auto folder = maybeFolderItem->getFolder();
        maybeOtherCollection = folder->collection();
        maybeDraggedCrate = Common::MutableFolder::SourceCrate{ folder };
    }
    else {
        auto maybeMutableFolderItem = dynamic_cast<MutableFolderItem*>(draggedItem.get());
        if (maybeMutableFolderItem != nullptr) {
            auto folder = maybeMutableFolderItem->getFolder();
            maybeOtherCollection = folder.constCast<const Common::MutableFolder*>()->collection();
            maybeDraggedCrate = Common::MutableFolder::SourceCrate{ folder };
            maybeMutableDraggedCrate = Common::MutableFolder::MutableSourceCrate{ folder };
        }
        else {
            auto maybePlaylistItem = dynamic_cast<PlaylistItem*>(draggedItem.get());
            if (maybePlaylistItem != nullptr) {
                auto playlist = maybePlaylistItem->getPlaylist();
                maybeOtherCollection = playlist->collection();
                maybeDraggedCrate = Common::MutableFolder::SourceCrate{ playlist };
            }
            else {
                auto maybeMutablePlaylistItem = dynamic_cast<MutablePlaylistItem*>(draggedItem.get());
                if (maybeMutablePlaylistItem != nullptr) {
                    auto playlist = maybeMutablePlaylistItem->getPlaylist();
                    maybeOtherCollection = playlist.constCast<const Common::MutablePlaylist*>()->collection();
                    maybeDraggedCrate = Common::MutableFolder::SourceCrate{ playlist };
                    maybeMutableDraggedCrate = Common::MutableFolder::MutableSourceCrate{ playlist };
                }
            }
        }
    }

    auto thisFolder = this->getFolder();
    auto model = this->getModel();
    auto toParentIndex = this->getModelIndex();
    if (!maybeDraggedCrate.isValid() || !toParentIndex.isValid()) {
        // -- Something went wrong, we're receiving a crate of an unknown type or an invalid parent index.

        // -- This could be optimized by passing the callback a progress count.
        for (count itemCount = 0; itemCount < thisFolder->numberOfCratesAndTrackEntriesContainedWithin(); ++itemCount) {
            callback();
        }

        return nothing;
    }

    if (Common::Collection::isEqual(*(thisFolder.constCast<const Common::MutableFolder*>()->collection()), **maybeOtherCollection)) {
        // -- We are receiving a crate from the same collection, this means we are moving.
        if (maybeMutableDraggedCrate.isValid() && draggedItem->hasParentItem()) {
            auto fromParentIndex = draggedItem->parentItem()->getModelIndex();
            if (fromParentIndex.isValid()) {
                auto rowInParent = draggedItem->getRowInParent();
                if (rowInParent != toIndex) {
                    model->beginMoveRows(fromParentIndex, rowInParent, rowInParent, toParentIndex, toIndex);

#if defined(NXA_BUGSNAG_APP_ID)
                    NXA_ASSERT_TRUE_WITH_BLOCK(toIndex <= thisFolder->numberOfSubCrates(), [thisFolder, &toIndex]() {
                        CrashLog::addUserInfoWithKey(String::stringWithFormat("%llu", toIndex), "toIndex");
                        CrashLog::addUserInfoWithKey(String::stringWithFormat("%llu", thisFolder->numberOfSubCrates()), "numberOfSubCrates");
                    });
#endif
                    thisFolder->moveSubCrateToIndex(*maybeMutableDraggedCrate, toIndex);

                    model->endMoveRows();

                    model->notifyLayoutChangedAt(fromParentIndex);
                }
            }
        }

        // -- This could be optimized by passing the callback a progress count.
        for (count itemCount = 0; itemCount < thisFolder->numberOfCratesAndTrackEntriesContainedWithin(); ++itemCount) {
            callback();
        }
    }
    else {
        // -- We are receiving a crate from another collection, this means we are copying.
        if (andOverWriteExisting == AndOverWriteExisting::Yes) {
            auto maybeIndexOfExistingCrate = thisFolder->maybeIndexOfExistingSubCrateWithSameNameAs(*maybeDraggedCrate);
            if (maybeIndexOfExistingCrate.isValid()) {
                toIndex = *maybeIndexOfExistingCrate;

                // -- We are replacing an existing crate so let's remove it first.
                model->beginRemoveRows(toParentIndex, toIndex, toIndex);

                this->removeChildAtIndex(toIndex);

                model->endRemoveRows();
            }
        }

        model->beginInsertRows(toParentIndex, toIndex, toIndex);

        thisFolder->addSubCrateAtIndexAsCopyOfWithPerItemProgressCallBack(toIndex, *maybeDraggedCrate, callback);

        model->endInsertRows();

        callback();

        auto tracksThatHaveChanged = this->unionOfAllTrackItemsContainedWithin();
        model->maybeUpdateBecauseTracksChanged(Array<NotNull<AbstractTrackItem*>>{ std::move(tracksThatHaveChanged) });
    }

    model->notifyLayoutChangedAt(toParentIndex);

    return toIndex;
}

Optional<QIcon> MutableFolderItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Folder Icon/GenericFolder Invert.png" };
    return cachedIcon;
}

Optional<QString> MutableFolderItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_folder->collection()->type())) };
}

Optional<QString> MutableFolderItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_folder->collection()->fullName()) };
}

Optional<Volume> MutableFolderItem::maybeParentCollectionVolume() const
{
    return this->p_folder->collection()->volume();
}

boolean MutableFolderItem::parentCollectionMustHaveTracksOnTheSameVolume() const
{
    return this->p_folder->collection()->mustHaveTracksOnTheSameVolume();
}

boolean MutableFolderItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_folder->hasTracksNotOnVolume(volume);
}

boolean MutableFolderItem::parentCollectionAllowsMovieTracksInPlaylists() const
{
    return this->p_folder->collection()->allowsMovieTracksInPlaylists();
}

boolean MutableFolderItem::hasMovieTracks() const
{
    return this->p_folder->hasMovieTracks();
}

count MutableFolderItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_folder->numberOfCratesAndTrackEntriesContainedWithin();
}

count MutableFolderItem::numberOfTracks() const
{
    return this->p_folder->tracks().length();
}

count MutableFolderItem::crateCount() const
{
    return this->p_folder->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutableFolderItem::maybeTrackTree()
{
    return std::async([]() {
        return Optional<Shared<AbstractTrackListNode>>{ };
    });
}
