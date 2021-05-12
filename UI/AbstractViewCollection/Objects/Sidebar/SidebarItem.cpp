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
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCollection.hpp>

#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <UiModel/SidebarModel.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>

#include <future>

namespace NxA { namespace RekordBuddy {

class MainWindow;

namespace NxADetail {

Optional<QIcon> loadingIcon{ nothing };

}

} }

using namespace NxA;
using namespace RekordBuddy;

SidebarItem::SidebarItem(NotNull<SidebarModel*> withModel,
                         SidebarItem* withParent,
                         WorkQueue& withQueue) : p_model{ withModel },
                                                 p_parentNode{ withParent },
                                                 p_queue{ withQueue }
{
    if (!NxADetail::loadingIcon.isValid()) {
        NxADetail::loadingIcon = QIcon{ ":/Icons/Crate List View/Loading/loader.gif" };
    }
}

SidebarItem::~SidebarItem()
{
    p_parentNode = nullptr;
}

Shared<SidebarItem> SidebarItem::p_cachedChildAtIndex(count index) const
{
    auto childReference = this->p_internalReferenceForChildAtIndex(index);

    auto maybeCachedChildItem = this->p_childCache.maybeValueForKey(childReference);
    if (maybeCachedChildItem.isValid()) {
        return *maybeCachedChildItem;
    }

    auto child = this->p_newChildForIndex(index);
    this->p_childCache.setValueForKey(child, childReference);

    return child;
}

void SidebarItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> window, boolean onlyAdd)
{
    // -- no default context menu items
}

Optional<QString> SidebarItem::getDescription() const
{
    return nothing;
}

void SidebarItem::setName(const QString& newName)
{
    NXA_ALOG("Unimplemented method");
}

count SidebarItem::numberOfTracks() const
{
    NXA_ALOG("Unimplemented method.");
}

Set<NotNull<AbstractTrackItem*>> SidebarItem::unionOfAllTrackItemsContainedWithin()
{
    MutableSet<NotNull<AbstractTrackItem*>> tracksFound;

    auto maybeTracks = this->maybeTrackTree().get();
    if (maybeTracks.isValid()) {
        auto& tracks = *maybeTracks;

        for (count childIndex = 0; childIndex < tracks->childCount(); ++childIndex) {
            auto maybeItem = (*tracks->maybeChildNode(childIndex))->maybeTrackItem();
            if (maybeItem.isValid()) {
                tracksFound.add(*maybeItem);
            }
        }
    }
    else {
        for (count childIndex = 0; childIndex < this->childCount(); ++childIndex) {
            tracksFound.add((*this->getChildAtIndex(childIndex)).unionOfAllTrackItemsContainedWithin());
        }
    }

    return { std::move(tracksFound) };
}

std::future<Optional<Shared<AbstractTrackListNode>>> SidebarItem::maybeTrackTree()
{
    return std::async([]() {
        return Optional<Shared<AbstractTrackListNode>>{ };
    });
}

std::future<Array<NotNull<AbstractTrackItem*>>> SidebarItem::tracks()
{
    return std::async([this]() -> Array<NotNull<AbstractTrackItem*>> {
        MutableArray<NotNull<AbstractTrackItem*>> result;

        auto maybeTracks = this->maybeTrackTree().get();
        if (maybeTracks.isValid()) {
            auto& tracks = *maybeTracks;

            for (count i = 0; i < tracks->childCount(); ++i) {
                auto maybeItem = (*tracks->maybeChildNode(i))->maybeTrackItem();
                if (maybeItem.isValid()) {
                    result.append(*maybeItem);
                }
            }
        }

        return { std::move(result) };
    });
}

boolean SidebarItem::needsLoading() const
{
    return false;
}

count SidebarItem::childCount() const
{
    return 0;
}

Optional<std::unique_ptr<AbstractGroupItem>> SidebarItem::predicates()
{
    return { };
}

Optional<QIcon> SidebarItem::maybeIcon() const
{
    return { };
}

Optional<QString> SidebarItem::maybeParentCollectionTypeName() const
{
    return nothing;
}

Optional<QString> SidebarItem::maybeParentCollectionFullName() const
{
    return nothing;
}

Optional<Volume> SidebarItem::maybeParentCollectionVolume() const
{
    return nothing;
}

boolean SidebarItem::canBeRenamed() const
{
    return false;
}

boolean SidebarItem::canEditPredicates() const
{
    return false;
}

boolean SidebarItem::canBeDeleted() const
{
    return false;
}

boolean SidebarItem::canBeCloned() const
{
    return false;
}

boolean SidebarItem::canAddOrganization() const
{
    return false;
}

boolean SidebarItem::canDeleteFilter() const
{
    return false;
}

boolean SidebarItem::canBeDragged() const
{
    return false;
}

boolean SidebarItem::canAddPlaylistChildren() const
{
    return false;
}

boolean SidebarItem::canAddFolderChildren() const
{
    return false;
}

Optional<count> SidebarItem::addTrackAtIndex(NotNull<AbstractTrackItem*>, count)
{
    return false;
}

boolean SidebarItem::needsConfirmationToDelete() const
{
    return true;
}

void SidebarItem::receiveDraggedCollectionWithPerItemProgressCallBack(NotNull<AbstractCollection*>, std::function<void(void)>&&)
{
    NXA_ALOG("Can not drag collection onto this item");
}

Optional<count> SidebarItem::receiveDraggedCrateAtIndexWithPerItemProgressCallBack(NotNull<AbstractCrateItem*>,
                                                                                   count,
                                                                                   std::function<void(void)>&&,
                                                                                   AndOverWriteExisting)
{
#if defined(NXA_BUGSNAG_APP_ID)
    boolean canDragCrateOnItem = false;
    NXA_ASSERT_TRUE_WITH_BLOCK(canDragCrateOnItem, [this]() {
        CrashLog::addUserInfoWithKey({ this->getName().toUtf8().data() }, "itemName");
        auto maybeDescription = this->getDescription();
        if (maybeDescription.isValid()) {
            CrashLog::addUserInfoWithKey({ maybeDescription->toUtf8().data() }, "itemDescription");
        }
    });
#endif

    NXA_ALOG("Can not drag crate onto this item");
}

std::future<integer64> SidebarItem::loadChildrenIfUnloaded(std::function<integer64()>&& callback)
{
    std::promise<integer64> promise;
    if (this->p_loaded) {
        promise.set_value(0);
    }
    else {
        this->p_loaded = true;
        auto result = callback();
        promise.set_value(result);
    }

    return promise.get_future();
}

QModelIndex SidebarItem::getModelIndex() const
{
    return this->getModel().asReference().indexForItem(this);
}

boolean SidebarItem::hasParentItem() const
{
    return this->p_parentNode != nullptr;
}

count SidebarItem::getRowInParent() const
{
    auto parent = this->p_parentNode;
    NXA_ASSERT_NOT_NULL(parent);

    for (count index = 0; index < parent->childCount(); ++index) {
        if (parent->getChildAtIndex(index).asRawPointer() == this) {
            return index;
        }
    }

    NXA_ALOG("Can't find us in our parent.");
}

const SidebarItem* SidebarItem::parentItem() const
{
    NXA_ASSERT_NOT_NULL(this->p_parentNode);
    return this->p_parentNode;
}

SidebarItem* SidebarItem::parentItem()
{
    NXA_ASSERT_NOT_NULL(this->p_parentNode);
    return this->p_parentNode;
}

void SidebarItem::setLoading(boolean value)
{
    this->p_loading = value;
    this->p_loaded = !value;
}

boolean SidebarItem::isLoading() const
{
    return this->p_loading;
}

boolean SidebarItem::isLoaded() const
{
    return this->p_loaded;
}
