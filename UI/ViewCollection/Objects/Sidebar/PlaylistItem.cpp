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

PlaylistItem::PlaylistItem(NotNull<SidebarModel*> model,
                           AbstractCrateItem* parent,
                           WorkQueue& withQueue,
                           NotNull<const Common::Playlist*> withPlaylist) : AbstractCrateItem{ model,
                                                                                               parent,
                                                                                               withQueue },
                                                                            p_playlist{ std::move(withPlaylist) } { }

InternalItemReference PlaylistItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<PlaylistItem*>(this);
    return mutableThis->p_playlist->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> PlaylistItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<PlaylistItem*>(this);
    return mutableThis->p_playlist->subCrateAtIndex(index).apply(CrateVisitor<PlaylistItem>{ mutableThis, mutableThis->p_queue, mutableThis->getModel() });
}

NotNull<const Common::Playlist*> PlaylistItem::getPlaylist() const
{
    return this->p_playlist;
}

boolean PlaylistItem::canEditPredicates() const
{
    return false;
}

boolean PlaylistItem::canBeDeleted() const
{
    return false;
}

boolean PlaylistItem::canBeCloned() const
{
    return false;
}

boolean PlaylistItem::canReceiveTracks() const
{
    return false;
}

boolean PlaylistItem::canAddOrganization() const
{
    return false;
}

boolean PlaylistItem::canBeDragged() const
{
    return true;
}

boolean PlaylistItem::isSelectable() const
{
    return true;
}

Optional<count> PlaylistItem::addTrackAtIndex(NotNull<AbstractTrackItem*>, count)
{
    return nothing;
}

void PlaylistItem::cloneCrate()
{
    NXA_ALOG("Unsupported");
}

void PlaylistItem::deleteCrate()
{
    NXA_ALOG("Unsupported");
}

Optional<QIcon> PlaylistItem::maybeIcon() const
{
    static QIcon cachedIcon{ String::stringWithFormat(":/Icons/Crate List View/%s.png", this->p_playlist->iconName()).asUTF8() };
    return cachedIcon;
}

Optional<QString> PlaylistItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_playlist->collection()->type())) };
}

Optional<QString> PlaylistItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_playlist->collection()->fullName()) };
}

Optional<Volume> PlaylistItem::maybeParentCollectionVolume() const
{
    return this->p_playlist->collection()->volume();
}

boolean PlaylistItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_playlist->hasTracksNotOnVolume(volume);
}

boolean PlaylistItem::hasMovieTracks() const
{
    return this->p_playlist->hasMovieTracks();
}

void PlaylistItem::setName(const QString& name)
{
    NXA_ALOG("Unsupported");
}

QString PlaylistItem::getName() const
{
    return fromString(this->p_playlist->name());
}

Optional<std::unique_ptr<AbstractGroupItem>> PlaylistItem::predicates()
{
    return {};
}

count PlaylistItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_playlist->numberOfCratesAndTrackEntriesContainedWithin();
}

count PlaylistItem::numberOfTracks() const
{
    return this->p_playlist->numberOfTracks();
}

count PlaylistItem::crateCount() const
{
    return this->p_playlist->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> PlaylistItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<TrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) }, nothing,
                                                [this]() -> TrackListNode::RefreshType {
                                                    if (this->childCount() == 0) {
                                                        return { this->p_playlist->tracks() };
                                                    }

                                                    MutableArray<Common::SubCrate> subCrates;
                                                    for (count i = 0; i < this->p_playlist->numberOfSubCrates(); ++i) {
                                                        subCrates.emplaceAppend(this->p_playlist->subCrateAtIndex(i));
                                                    }
                                                    return { Array<Common::SubCrate>(std::move(subCrates)) };
                                                },
                                                fromString(this->p_playlist->name())));
        }

        return this->p_treeCache;
    });
}

boolean PlaylistItem::isOrganizedBy(Common::Property::TypeID typeID) const
{
    return this->p_playlist->isOrganizedBy(typeID);
}

void PlaylistItem::organizeBy(Common::Property::TypeID typeID)
{
    NXA_ALOG("Unsupported");
}

void PlaylistItem::removeOrganizationBy(Common::Property::TypeID typeID)
{
    NXA_ALOG("Unsupported");
}

boolean PlaylistItem::isOrganizedBy(const Common::Tag& tag) const
{
    return this->p_playlist->isOrganizedBy(tag);
}

void PlaylistItem::organizeBy(Common::MutableTag& tag)
{
    NXA_ALOG("Unsupported");
}

void PlaylistItem::removeOrganizationBy(Common::MutableTag& tag)
{
    NXA_ALOG("Unsupported");
}

void PlaylistItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
{
    // -- Add items from parent first (order matters)
    AbstractCrateItem::populateContextMenu(contextMenu, ownerWindow, onlyAdd);

    auto itemName = this->getName();

    // -- Add "Organize By" section
    // Turn off Organize By for now.
/*    auto propertyTypesForSorting = this->p_playlist->collection()->propertyTypes();
    auto trackTagsForSorting = this->p_playlist->collection()->tags();
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
