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

#include <MutableViewCollection/Sidebar/Traits.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>
#include <MutableViewCollection/TrackList/MutableTrackItem.hpp>

#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <UiMainWindow/MainWindow.hpp>
#include <UiBase/variantInterop.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Tracks/Track.hpp>

#include <Base/Types.hpp>

#include <qicon.h>
#include <qstring.h>

#include <type_traits>

using namespace NxA;
using namespace RekordBuddy;

MutableSmartPlaylistItem::MutableSmartPlaylistItem(NotNull<SidebarModel*> model,
                                                   AbstractCrateItem* parent,
                                                   WorkQueue& withQueue,
                                                   NotNull<Common::MutableSmartPlaylist*> withPlaylist) : AbstractCrateItem{ model,
                                                                                                                             parent,
                                                                                                                             withQueue },
                                                                                                          p_smartPlaylist{ std::move(withPlaylist) } { }

InternalItemReference MutableSmartPlaylistItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<MutableSmartPlaylistItem*>(this);
    return mutableThis->p_smartPlaylist->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> MutableSmartPlaylistItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutableSmartPlaylistItem*>(this);
    return mutableThis->p_smartPlaylist->subCrateAtIndex(index).apply(CrateVisitor<MutableSmartPlaylistItem>{ mutableThis, mutableThis->p_queue, mutableThis->getModel() });
}

NotNull<const Common::MutableSmartPlaylist*> MutableSmartPlaylistItem::getPlaylist() const
{
    return this->p_smartPlaylist;
}

NotNull<Common::MutableSmartPlaylist*> MutableSmartPlaylistItem::getPlaylist()
{
    return this->p_smartPlaylist;
}

boolean MutableSmartPlaylistItem::canBeRenamed() const
{
    return this->p_smartPlaylist->canBeRenamed();
}

boolean MutableSmartPlaylistItem::canEditPredicates() const
{
    return this->p_smartPlaylist->canHavePredicates();
}

boolean MutableSmartPlaylistItem::canBeDeleted() const
{
    return true;
}

boolean MutableSmartPlaylistItem::canBeCloned() const
{
    return this->p_smartPlaylist->canBeCloned();
}

boolean MutableSmartPlaylistItem::canBeDragged() const
{
    return false;
}

boolean MutableSmartPlaylistItem::canReceiveTracks() const
{
    return false;
}

boolean MutableSmartPlaylistItem::canAddOrganization() const
{
    return true;
}

boolean MutableSmartPlaylistItem::isSelectable() const
{
    return true;
}

boolean MutableSmartPlaylistItem::canReceiveCrates() const
{
    return false;
}

Optional<count> MutableSmartPlaylistItem::addTrackAtIndex(NotNull<AbstractTrackItem*>, count)
{
    NXA_ALOG("Unsupported.");
}

void MutableSmartPlaylistItem::cloneCrate()
{
    NXA_ALOG("Unsupported.");
}

void MutableSmartPlaylistItem::deleteCrate()
{
    NXA_ALOG("Unsupported.");
}

Optional<QIcon> MutableSmartPlaylistItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Playlist Icon/MusicPlaylist invert.png" };
    return cachedIcon;
}

Optional<QString> MutableSmartPlaylistItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_smartPlaylist->collection()->type())) };
}

Optional<QString> MutableSmartPlaylistItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_smartPlaylist->collection()->fullName()) };
}

Optional<Volume> MutableSmartPlaylistItem::maybeParentCollectionVolume() const
{
    return this->p_smartPlaylist->collection()->volume();
}

boolean MutableSmartPlaylistItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_smartPlaylist->hasTracksNotOnVolume(volume);
}

void MutableSmartPlaylistItem::setName(const QString& name)
{
    NXA_ALOG("Unsupported.");
}

QString MutableSmartPlaylistItem::getName() const
{
    return fromString(this->p_smartPlaylist->name());
}

Optional<std::unique_ptr<AbstractGroupItem>> MutableSmartPlaylistItem::predicates()
{
    return { };
}

count MutableSmartPlaylistItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_smartPlaylist->numberOfCratesAndTrackEntriesContainedWithin();
}

count MutableSmartPlaylistItem::numberOfTracks() const
{
    return this->p_smartPlaylist->numberOfTracks();
}

count MutableSmartPlaylistItem::crateCount() const
{
    return this->p_smartPlaylist->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutableSmartPlaylistItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<MutableTrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) }, nothing,
                                                       [this]() -> MutableTrackListNode::RefreshType {
                                                           if (this->childCount() == 0) {
                                                               MutableArray<NotNull<Common::MutableTrack*>> tracks;

                                                               for (count i = 0; i < this->p_smartPlaylist->numberOfTracks(); ++i) {
                                                                   tracks.emplaceAppend(this->p_smartPlaylist->trackAtIndex(i));
                                                               }

                                                               return { Array<NotNull<Common::MutableTrack*>>(std::move(tracks)) };
                                                           }

                                                           MutableArray<Common::MutableSubCrate> subCrates;

                                                           for (count i = 0; i < this->p_smartPlaylist->numberOfSubCrates(); ++i) {
                                                               subCrates.emplaceAppend(this->p_smartPlaylist->subCrateAtIndex(i));
                                                           }

                                                           return { Array<Common::MutableSubCrate>(std::move(subCrates)) };
                                                       },
                                                       fromString(this->p_smartPlaylist->name())));
        }

        return this->p_treeCache;
    });
}

boolean MutableSmartPlaylistItem::isOrganizedBy(Common::Property::TypeID typeID) const
{
    return this->p_smartPlaylist->isOrganizedBy(typeID);
}

void MutableSmartPlaylistItem::organizeBy(Common::Property::TypeID typeID)
{
    this->p_smartPlaylist->organizeBy(typeID);
}

void MutableSmartPlaylistItem::removeOrganizationBy(Common::Property::TypeID typeID)
{
    this->p_smartPlaylist->removeOrganizationBy(typeID);
}

boolean MutableSmartPlaylistItem::isOrganizedBy(const Common::Tag& tag) const
{
    return this->p_smartPlaylist->isOrganizedBy(tag);
}

void MutableSmartPlaylistItem::organizeBy(Common::MutableTag& tag)
{
    this->p_smartPlaylist->organizeBy(tag);
}

void MutableSmartPlaylistItem::removeOrganizationBy(Common::MutableTag& tag)
{
    this->p_smartPlaylist->removeOrganizationBy(tag);
}

void MutableSmartPlaylistItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
{
    // -- Add items from parent first (order matters)
    AbstractCrateItem::populateContextMenu(contextMenu, ownerWindow, onlyAdd);

    auto itemName = this->getName();

    // -- Add "Organize By" section

    // Turn off Organize By for now.
/*    auto propertyTypesForSorting = this->p_smartPlaylist->collection()->propertyTypes();
    auto trackTagsForSorting = this->p_smartPlaylist->collection()->tags();
    if (!onlyAdd && (propertyTypesForSorting.length() != 0u || trackTagsForSorting.length() != 0u)) {
        if (!contextMenu.isEmpty()) {
            contextMenu.addSeparator();
        }

        // -- Add Properties
        if (propertyTypesForSorting.length() != 0u) {
            QMenu* organizeMenu = contextMenu.addMenu(QObject::tr("Organize ‘%1’ By").arg(itemName));
            if (organizeMenu != nullptr) {
                for (auto&& propertyType : propertyTypesForSorting) {
                    if (this->isOrganizedBy(propertyType)) {
                        continue;
                    }

                    // -- Set Context Menu as parent so these actions are deleted with destruction of Context Menu
                    QAction* action = new QAction{ Common::Property::nameForType(propertyType).asUTF8(), &contextMenu };
                    action->setData(QVariant::fromValue(Optional<Common::Property::TypeID>{ propertyType }));
                    QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::handleOrganizeCrateCommand);
                    organizeMenu->addAction(action);
                }
            }
        }

        // -- Add Tags
        if (trackTagsForSorting.length()) {
            QMenu* organizeMenu = contextMenu.addMenu(QObject::tr("Organize ‘%1’ By").arg(itemName));
            if (organizeMenu != nullptr) {
                for (auto&& trackTag : trackTagsForSorting) {
                    if (this->isOrganizedBy(trackTag->asImmutableReference())) {
                        continue;
                    }

                    // -- Set Context Menu as parent so these actions are deleted with destruction of Context Menu
                    QAction* action = new QAction{ trackTag->value().asUTF8(), &contextMenu };
                    action->setData(QVariant::fromValue(Optional<Common::MutableTag*>{ trackTag.asRawPointer() }));
                    QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::handleOrganizeCrateCommand);
                    organizeMenu->addAction(action);
                }
            }
        }
    }*/
}
