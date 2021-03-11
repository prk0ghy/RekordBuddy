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

#include <ViewCollection/Sidebar/SmartPlaylistItem.hpp>
#include <ViewCollection/Sidebar/Traits.hpp>
#include <ViewCollection/TrackList/TrackListNode.hpp>
#include <ViewCollection/TrackList/TrackItem.hpp>

#include <AbstractViewCollection/Predicate/AbstractPredicateItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <UiMainWindow/MainWindow.hpp>
#include <UiBase/variantInterop.hpp>

#include <CommonCollection/Collection.hpp>

#include <Base/Types.hpp>

#include <qicon.h>
#include <qstring.h>

#include <type_traits>

using namespace NxA;
using namespace RekordBuddy;

SmartPlaylistItem::SmartPlaylistItem(NotNull<SidebarModel*> model,
                                     AbstractCrateItem* parent,
                                     WorkQueue& withQueue,
                                     NotNull<const Common::SmartPlaylist*> withPlaylist) : AbstractCrateItem{ model,
                                                                                                              parent,
                                                                                                              withQueue },
                                                                                           p_smartPlaylist{ std::move(withPlaylist) } { }

InternalItemReference SmartPlaylistItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<SmartPlaylistItem*>(this);
    return mutableThis->p_smartPlaylist->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> SmartPlaylistItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<SmartPlaylistItem*>(this);
    return mutableThis->p_smartPlaylist->subCrateAtIndex(index).apply(CrateVisitor<SmartPlaylistItem>{ mutableThis, mutableThis->p_queue, mutableThis->getModel() });
}

NotNull<const Common::SmartPlaylist*> SmartPlaylistItem::getPlaylist() const
{
    return this->p_smartPlaylist;
}

boolean SmartPlaylistItem::canEditPredicates() const
{
    return false;
}

boolean SmartPlaylistItem::canBeDeleted() const
{
    return false;
}

boolean SmartPlaylistItem::canBeCloned() const
{
    return false;
}

boolean SmartPlaylistItem::canReceiveTracks() const
{
    return false;
}

boolean SmartPlaylistItem::canAddOrganization() const
{
    return false;
}

boolean SmartPlaylistItem::isSelectable() const
{
    return true;
}

Optional<count> SmartPlaylistItem::addTrackAtIndex(NotNull<AbstractTrackItem*>, count)
{
    return nothing;
}

void SmartPlaylistItem::cloneCrate()
{
    NXA_ALOG("Unsupported");
}

void SmartPlaylistItem::deleteCrate()
{
    NXA_ALOG("Unsupported");
}

Optional<QIcon> SmartPlaylistItem::maybeIcon() const
{
    if (!this->maybeCachedIcon.isValid()) {
        this->maybeCachedIcon = QIcon{ String::stringWithFormat("://Icons/Crate List View/%s.png", this->p_smartPlaylist->iconName()).asUTF8() };
    }

    return this->maybeCachedIcon;
}

Optional<QString> SmartPlaylistItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_smartPlaylist->collection()->type())) };
}

Optional<QString> SmartPlaylistItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_smartPlaylist->collection()->fullName()) };
}

Optional<Volume> SmartPlaylistItem::maybeParentCollectionVolume() const
{
    return this->p_smartPlaylist->collection()->volume();
}

boolean SmartPlaylistItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_smartPlaylist->hasTracksNotOnVolume(volume);
}

void SmartPlaylistItem::setName(const QString& name)
{
    NXA_ALOG("Unsupported");
}

QString SmartPlaylistItem::getName() const
{
    return fromString(this->p_smartPlaylist->name());
}

Optional<std::unique_ptr<AbstractGroupItem>> SmartPlaylistItem::predicates()
{
    return {};
}

count SmartPlaylistItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_smartPlaylist->numberOfCratesAndTrackEntriesContainedWithin();
}

count SmartPlaylistItem::numberOfTracks() const
{
    return this->p_smartPlaylist->numberOfTracks();
}

count SmartPlaylistItem::crateCount() const
{
    return this->p_smartPlaylist->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> SmartPlaylistItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<TrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) }, nothing,
                                                [this]() -> TrackListNode::RefreshType {
                                                    if (this->childCount() == 0) {
                                                        return { this->p_smartPlaylist->tracks() };
                                                    }

                                                    MutableArray<Common::SubCrate> subCrates;
                                                    for (count i = 0; i < this->p_smartPlaylist->numberOfSubCrates(); ++i) {
                                                        subCrates.emplaceAppend(this->p_smartPlaylist->subCrateAtIndex(i));
                                                    }
                                                    return { Array<Common::SubCrate>(std::move(subCrates)) };
                                                },
                                                fromString(this->p_smartPlaylist->name())));
        }

        return this->p_treeCache;
    });
}

boolean SmartPlaylistItem::isOrganizedBy(Common::Property::TypeID typeID) const
{
    return this->p_smartPlaylist->isOrganizedBy(typeID);
}

void SmartPlaylistItem::organizeBy(Common::Property::TypeID typeID)
{
    NXA_ALOG("Unsupported");
}

void SmartPlaylistItem::removeOrganizationBy(Common::Property::TypeID typeID)
{
    NXA_ALOG("Unsupported");
}

boolean SmartPlaylistItem::isOrganizedBy(const Common::Tag& tag) const
{
    return this->p_smartPlaylist->isOrganizedBy(tag);
}

void SmartPlaylistItem::organizeBy(Common::MutableTag& tag)
{
    NXA_ALOG("Unsupported");
}

void SmartPlaylistItem::removeOrganizationBy(Common::MutableTag& tag)
{
    NXA_ALOG("Unsupported");
}

void SmartPlaylistItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
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
                    if (this->isOrganizedBy(*trackTag.asRawPointer())) {
                        continue;
                    }

                    // -- Set Context Menu as parent so these actions are deleted with destruction of Context Menu
                    QAction* action = new QAction{ trackTag->value().asUTF8(), &contextMenu };
                    action->setData(QVariant::fromValue(Optional<const Common::Tag*>{ trackTag.asRawPointer() }));
                    QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::handleOrganizeCrateCommand);
                    organizeMenu->addAction(action);
                }
            }
        }
    }*/
}
