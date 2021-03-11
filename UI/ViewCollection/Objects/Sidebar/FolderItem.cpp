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

#include <ViewCollection/Sidebar/FolderItem.hpp>
#include <ViewCollection/Sidebar/PlaylistItem.hpp>
#include <ViewCollection/Sidebar/Traits.hpp>

#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

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

FolderItem::FolderItem(NotNull<SidebarModel*> model,
                       SidebarItem* parent,
                       WorkQueue& withQueue,
                       NotNull<const Common::Folder*> withFolder) : AbstractCrateItem{ model,
                                                                                       parent,
                                                                                       withQueue },
                                                                    p_folder{ std::move(withFolder) }
{

}

InternalItemReference FolderItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<FolderItem*>(this);
    return mutableThis->p_folder->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> FolderItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<FolderItem*>(this);
    return mutableThis->p_folder->subCrateAtIndex(index).apply(CrateVisitor<FolderItem>{ mutableThis, mutableThis->p_queue, mutableThis->getModel() });
}

NotNull<const Common::Folder*> FolderItem::getFolder() const
{
    return this->p_folder;
}

void FolderItem::createChildFolder(const String& name)
{
    NXA_ALOG("Unsupported");
}

void FolderItem::createChildPlaylist(const String& name)
{
    NXA_ALOG("Unsupported");
}

void FolderItem::createChildSmartPlaylist(const String& name)
{
    NXA_ALOG("Unsupported");
}

void FolderItem::setName(const QString& name)
{
    NXA_ALOG("Unsupported");
}

QString FolderItem::getName() const
{
    return fromString(this->p_folder->name());
}

boolean FolderItem::canBeDeleted() const
{
    return false;
}

boolean FolderItem::canBeCloned() const
{
    return false;
}

boolean FolderItem::canAddPlaylistChildren() const
{
    return false;
}

boolean FolderItem::canAddFolderChildren() const
{
    return false;
}

boolean FolderItem::canBeDragged() const
{
    return true;
}

boolean FolderItem::isSelectable() const
{
    return true;
}

void FolderItem::deleteCrate()
{
    NXA_ALOG("Unsupported.");
}

void FolderItem::cloneCrate()
{
    NXA_ALOG("Unsupported");
}

Optional<QIcon> FolderItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Folder Icon/GenericFolder Invert.png" };
    return cachedIcon;
}

Optional<QString> FolderItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_folder->collection()->type())) };
}

Optional<QString> FolderItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_folder->collection()->fullName()) };
}

Optional<Volume> FolderItem::maybeParentCollectionVolume() const
{
    return this->p_folder->collection()->volume();
}

boolean FolderItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_folder->hasTracksNotOnVolume(volume);
}

boolean FolderItem::hasMovieTracks() const
{
    return this->p_folder->hasMovieTracks();
}

count FolderItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_folder->numberOfCratesAndTrackEntriesContainedWithin();
}

count FolderItem::numberOfTracks() const
{
    return this->p_folder->tracks().length();
}

count FolderItem::crateCount() const
{
    return this->p_folder->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> FolderItem::maybeTrackTree()
{
    return std::async([]() {
        return Optional<Shared<AbstractTrackListNode>>{ };
    });
}
