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

#include <AbstractViewCollection/TrackList/AbstractTrackListNode.hpp>
#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>

#include <UiBase/InternalItemReference.hpp>

#include <Base/Array.hpp>
#include <Base/Flags.hpp>
#include <Base/Map.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

#include <QVariant>
#include <QIcon>
#include <QMenu>

#include <memory>
#include <future>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class MainWindow;
class WorkQueue;
class SidebarModel;
class AbstractTrackItem;
class AbstractCrateItem;
class AbstractCollection;

namespace NxADetail {

extern Optional<QIcon> loadingIcon;

}

class SidebarItem : public std::enable_shared_from_this<SidebarItem>
{
    Pointer<SidebarModel> p_model;
    SidebarItem* p_parentNode{ nullptr };

    mutable MutableMap<InternalItemReference, Shared<SidebarItem>> p_childCache;

protected:
    WorkQueue& p_queue;
    boolean p_loading{ false };
    boolean p_loaded{ true };

    virtual InternalItemReference p_internalReferenceForChildAtIndex(count) const
    {
        NXA_ALOG("No child available.");
    }
    Shared<SidebarItem> p_cachedChildAtIndex(count index) const;
    virtual Shared<SidebarItem> p_newChildForIndex(count) const
    {
        NXA_ALOG("No child available.");
    }

public:
    // -- Types
    enum class AndOverWriteExisting
    {
        No,
        Yes
    };
    enum class TypeOfSelection
    {
        Crate,
        Collection,
        AllTracks,
        Playlists,
        Artist,
        Tag,
        Key,

        // -- This is required for static assertions.
        LastFlag
    };
    using TypeOfThingsSelectable = Flags<TypeOfSelection>;

    // -- Constructors & Destructors
    explicit SidebarItem(NotNull<SidebarModel*>, SidebarItem*, WorkQueue&);
    virtual ~SidebarItem();

    // -- Instance Methods
    virtual void forceRefresh()
    {
        this->p_childCache.removeAll();
    }

    virtual void setName(const QString&);
    virtual QString getName() const = 0;
    virtual Optional<QString> getDescription() const;

    virtual count numberOfCratesAndTrackEntriesContainedWithin() const
    {
        NXA_ALOG("Unsupported on this object.");
    }
    virtual boolean hasTracksNotOnVolume(const Volume&) const
    {
        NXA_ALOG("Unsupported on this object.");
    }
    virtual boolean hasMovieTracks() const
    {
        NXA_ALOG("Unsupported on this object.");
    }

    virtual count numberOfTracks() const;
    virtual Set<NotNull<AbstractTrackItem*>> unionOfAllTrackItemsContainedWithin();
    virtual std::future<Optional<Shared<AbstractTrackListNode>>> maybeTrackTree();
    virtual std::future<Array<NotNull<AbstractTrackItem*>>> tracks();

    Pointer<const SidebarItem> getChildAtIndex(count index) const
    {
        return { this->p_cachedChildAtIndex(index).asRawPointer() };
    }
    Pointer<SidebarItem> getChildAtIndex(count index)
    {
        return { const_cast<SidebarItem*>(const_cast<const SidebarItem*>(this)->getChildAtIndex(index).asRawPointer()) };
    }
    virtual boolean insertChildAtIndexByCalling(count index, std::function<boolean(Pointer<SidebarItem>&& parent,
                                                                                   count index)> function)
    {
        return function(this, index);
    }
    virtual void removeChildAtIndex(count index)
    {
        this->p_childCache.removeValueForKey(this->p_internalReferenceForChildAtIndex(index));
    }

    virtual count childCount() const;
    QModelIndex getModelIndex() const;

    virtual void populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*>, boolean onlyAdd);

    // -- start loading children, and call the callback when done. also returns a future that we can use as a barrier if needed.
    virtual std::future<integer64> loadChildrenIfUnloaded(std::function<integer64()>&&);

    virtual boolean needsLoading() const;
    virtual boolean canEditPredicates() const;
    virtual boolean canBeDeleted() const;
    virtual boolean canBeRenamed() const;
    virtual boolean canBeCloned() const;
    virtual boolean canAddOrganization() const;
    virtual boolean canDeleteFilter() const;
    virtual boolean canAddPlaylistChildren() const;
    virtual boolean canAddFolderChildren() const;
    virtual boolean isSelectable() const
    {
        return false;
    }
    virtual TypeOfSelection typeOfSelectionForThis() const
    {
        NXA_ALOG("This is not selectable.");
    }

    virtual boolean canReceiveTracks() const
    {
        return false;
    }
    virtual boolean canReceiveCrates() const
    {
        return false;
    }
    virtual boolean canReceiveCollection() const
    {
        return false;
    }
    virtual boolean canReceiveFiles() const
    {
        return false;
    }
    virtual boolean receiveFileAt(const FilePath&)
    {
        NXA_ALOG("Calling unsupported method.");
    }
    virtual boolean canBeDragged() const;
    virtual boolean canReceive(NotNull<AbstractCrateItem*>) const
    {
        return false;
    }
    virtual boolean canReceive(NotNull<AbstractTrackItem*>) const
    {
        return false;
    }
    virtual boolean canReceive(NotNull<AbstractCollection*>) const
    {
        return false;
    }
    virtual boolean needsConfirmationToDelete() const;

    virtual Optional<count> addTrackAtIndex(NotNull<AbstractTrackItem*>, count);
    virtual void receiveDraggedCollectionWithPerItemProgressCallBack(NotNull<AbstractCollection*>, std::function<void(void)>&&);
    virtual Optional<count> receiveDraggedCrateAtIndexWithPerItemProgressCallBack(NotNull<AbstractCrateItem*>,
                                                                                  count,
                                                                                  std::function<void(void)>&&,
                                                                                  AndOverWriteExisting = AndOverWriteExisting::Yes);

    virtual Optional<std::unique_ptr<AbstractGroupItem>> predicates();
    virtual Optional<QIcon> maybeIcon() const;
    virtual Optional<QString> maybeParentCollectionTypeName() const;
    virtual Optional<QString> maybeParentCollectionFullName() const;
    virtual Optional<Volume> maybeParentCollectionVolume() const;
    virtual boolean parentCollectionMustHaveTracksOnTheSameVolume() const
    {
        return true;
    }
    virtual boolean parentCollectionAllowsMovieTracksInPlaylists() const
    {
        return true;
    }

    virtual void maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>&)
    {
        // -- By default nothing is done with this notification.
    }

    boolean hasParentItem() const;
    count getRowInParent() const;
    const SidebarItem* parentItem() const;
    SidebarItem* parentItem();

    NotNull<SidebarModel*> getModel()
    {
        return this->p_model.asNotNull();
    }
    Pointer<const SidebarModel> getModel() const
    {
        return this->p_model;
    }
    void setLoading(boolean);
    boolean isLoading() const;
    boolean isLoaded() const;
};

} }
