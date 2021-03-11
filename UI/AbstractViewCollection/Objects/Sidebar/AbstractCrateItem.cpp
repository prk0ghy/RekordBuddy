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
#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <UiMainWindow/MainWindow.hpp>

#include <WorkQueue/WorkQueue.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Globals

Optional<Shared<MutableArray<NxADetail::CrateType>>> NxADetail::maybeDraggedCrateObjects;

// -- Constructors & Destructors

AbstractCrateItem::AbstractCrateItem(NotNull<SidebarModel*> model,
                                     SidebarItem* parent,
                                     WorkQueue& withQueue) : SidebarItem{ model,
                                                                          parent,
                                                                          withQueue }
{

}

// -- Overridden Instance Methods

count AbstractCrateItem::childCount() const
{
    return this->crateCount();
}

void AbstractCrateItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
{
    // Add items from parent first (order matters)
    SidebarItem::populateContextMenu(contextMenu, ownerWindow, onlyAdd);

    auto itemName = this->getName();

    if (this->canAddFolderChildren()) {
        QAction* action = new QAction{ QObject::tr("New Folder"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::addSubfolderToSelectedCrate);
        contextMenu.addAction(action);
    }

    if (this->canAddPlaylistChildren()) {
        QAction* addPlaylistAction = new QAction{ QObject::tr("New Playlist"), &contextMenu };
        QObject::connect(addPlaylistAction, &QAction::triggered, ownerWindow.get(), &MainWindow::addPlaylistToSelectedCrate);
        contextMenu.addAction(addPlaylistAction);

#if 0
        // -- We turn off smart playlists for the time being
        QAction* addSmartPlaylistAction = new QAction{ QObject::tr("New Smart Playlist"), &contextMenu };
        QObject::connect(addSmartPlaylistAction, &QAction::triggered, ownerWindow.get(), &MainWindow::addSmartPlaylistToSelectedCrate);
        contextMenu.addAction(addSmartPlaylistAction);
#endif
    }

    if (!onlyAdd && this->canBeRenamed()) {
        QAction* action = new QAction{ QObject::tr("Rename ‘%1’").arg(itemName), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::renameSelectedCrate);
        contextMenu.addAction(action);
    }

    if (!onlyAdd && this->canBeDeleted()) {
        QAction* action = new QAction{ QObject::tr("Delete ‘%1’").arg(itemName), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::deleteSelectedCrates);
        contextMenu.addAction(action);
    }

#if 0
    // -- We turn off cloning for the time being
    if (!onlyAdd && this->canBeCloned()) {
        QAction* action = new QAction{ QObject::tr("Clone ‘%1’").arg(itemName), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::cloneSelectedCrate);
        contextMenu.addAction(action);
    }
#endif

    if (!onlyAdd && this->canEditPredicates()) {
        QAction* action = new QAction{ QObject::tr("Edit Predicates"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::editSelectedCratePredicates);
        contextMenu.addAction(action);
    }

    if (!onlyAdd && this->canDeleteFilter()) {
        QAction* action = new QAction{ QObject::tr("Remove Organization"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), &MainWindow::deleteSelectedCrates);
        contextMenu.addAction(action);
    }
}

// -- Instance Methods

void AbstractCrateItem::deleteCrate()
{
    // no action by default
}

void AbstractCrateItem::cloneCrate()
{
    // no action by default
}

void AbstractCrateItem::createChildFolder(const String& name)
{
    // no action by default
}

void AbstractCrateItem::createChildSmartPlaylist(const String& name)
{
    // no action by default
}

void AbstractCrateItem::createChildPlaylist(const String& name)
{
    // no action by default
}

boolean AbstractCrateItem::canBeRenamed() const
{
    return false;
}

boolean AbstractCrateItem::isOrganizedBy(Common::Property::TypeID typeID) const
{
    // -- No action by default
    return false;
}

void AbstractCrateItem::organizeBy(Common::Property::TypeID typeID)
{
    // -- No action by default
}

void AbstractCrateItem::removeOrganizationBy(Common::Property::TypeID typeID)
{
    // -- No action by default
}

boolean AbstractCrateItem::isOrganizedBy(const Common::Tag& tag) const
{
    // -- No action by default
    return false;
}

void AbstractCrateItem::organizeBy(Common::MutableTag& tag)
{
    // -- No action by default
}

void AbstractCrateItem::removeOrganizationBy(Common::MutableTag& tag)
{
    // -- No action by default
}

void AbstractCrateItem::setPredicateGroup(NotNull<AbstractGroupItem*>)
{
    NXA_ALOG("Can not set predicates on non-smartplaylist crate");
}
