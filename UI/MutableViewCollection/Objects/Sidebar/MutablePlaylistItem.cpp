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

#include <UiBase/AlertMessageBox.hpp>
#include <UiBase/variantInterop.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <ViewCollection/TrackList/TrackItem.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Tracks/Track.hpp>

#include <Base/Types.hpp>

#include <qicon.h>
#include <qstring.h>

#include <type_traits>

using namespace NxA;
using namespace RekordBuddy;

MutablePlaylistItem::MutablePlaylistItem(NotNull<SidebarModel*> model,
                                         AbstractCrateItem* parent,
                                         WorkQueue& withQueue,
                                         NotNull<Common::MutablePlaylist*> withPlaylist) : AbstractCrateItem{ model,
                                                                                                              parent,
                                                                                                              withQueue },
                                                                                           p_playlist{ std::move(withPlaylist) }
{

}

InternalItemReference MutablePlaylistItem::p_internalReferenceForChildAtIndex(count index) const
{
    auto mutableThis = const_cast<MutablePlaylistItem*>(this);
    return mutableThis->p_playlist->subCrateAtIndex(index).apply([](auto& crate) -> InternalItemReference { return crate.get(); });
}

Shared<SidebarItem> MutablePlaylistItem::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutablePlaylistItem*>(this);
    return mutableThis->p_playlist->subCrateAtIndex(index).apply(CrateVisitor<MutablePlaylistItem>{ mutableThis, mutableThis->p_queue, mutableThis->getModel() });
}

NotNull<const Common::MutablePlaylist*> MutablePlaylistItem::getPlaylist() const
{
    return this->p_playlist;
}

NotNull<Common::MutablePlaylist*> MutablePlaylistItem::getPlaylist()
{
    return this->p_playlist;
}

boolean MutablePlaylistItem::canBeRenamed() const
{
    return true;
}

boolean MutablePlaylistItem::canEditPredicates() const
{
    return false;
}

boolean MutablePlaylistItem::canBeDeleted() const
{
    return true;
}

boolean MutablePlaylistItem::canBeCloned() const
{
    return this->p_playlist->canBeCloned();
}

boolean MutablePlaylistItem::canBeDragged() const
{
    return true;
}

boolean MutablePlaylistItem::canReceiveTracks() const
{
    return true;
}

boolean MutablePlaylistItem::canAddOrganization() const
{
    return true;
}

boolean MutablePlaylistItem::isSelectable() const
{
    return true;
}

boolean MutablePlaylistItem::canReceiveCrates() const
{
    return false;
}

boolean MutablePlaylistItem::canReceive(NotNull<AbstractTrackItem*> abstractTrack) const
{
    // -- For now we are going to deal with tracks that can't be received by a dialog after the drop is ordered
    return true;

#if 0
    if (auto directTrack = abstractTrack.maybeAs<MutableTrackItem*>(); directTrack.isValid()) {
        return this->p_playlist->canReceive((*directTrack)->getTrack());
    }
    else if (auto directTrack = abstractTrack.maybeAs<TrackItem*>(); directTrack.isValid()) {
        return this->p_playlist->canReceive((*directTrack)->getTrack());
    }

    return false;
#endif
}

Optional<count> MutablePlaylistItem::addTrackAtIndex(NotNull<AbstractTrackItem*> abstractTrack, count index)
{
    if (auto directTrack = abstractTrack.maybeAs<MutableTrackItem*>(); directTrack.isValid()) {
        return this->p_playlist->addTrackAtIndex((*directTrack)->getTrack(), index, Common::MutablePlaylist::AndUpdateTracks::Yes);
    }
    else if (auto directTrack = abstractTrack.maybeAs<TrackItem*>(); directTrack.isValid()) {
        return this->p_playlist->addTrackAtIndex((*directTrack)->getTrack(), index, Common::MutablePlaylist::AndUpdateTracks::Yes);
    }

    return nothing;
}

void MutablePlaylistItem::moveTracksAtIndicesToIndex(Array<count> trackIndices, count toIndex)
{
    this->p_playlist->moveTracksAtIndicesToIndex(trackIndices, toIndex);

    if (this->p_treeCache.isValid()) {
        (*this->p_treeCache)->refreshTrackList();
    }
}

boolean MutablePlaylistItem::maybeConfirmTrackDeletion() const
{
    return ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Confirm Deletion"),
                                                            QObject::tr("Are you sure you want to remove this/these track(s) from '%1'?").arg(this->getName()),
                                                            QObject::tr("You can still choose not to save these changes when you exit the app."));
}

void MutablePlaylistItem::removeTrackAtIndex(count index)
{
    this->p_playlist->removeTrackAtIndex(index);
}

void MutablePlaylistItem::cloneCrate()
{
    this->p_playlist->cloneWithName(String::stringWithFormat("Clone of %s", this->p_playlist->name().asUTF8()));
}

void MutablePlaylistItem::deleteCrate()
{
    Common::MutableFolder::removeFromParent(this->p_playlist);
}

Optional<QIcon> MutablePlaylistItem::maybeIcon() const
{
    static QIcon cachedIcon{ ":/Icons/Crate List View/Playlist Icon/MusicPlaylist invert.png" };
    return cachedIcon;
}

Optional<QString> MutablePlaylistItem::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_playlist->collection()->type())) };
}

Optional<QString> MutablePlaylistItem::maybeParentCollectionFullName() const
{
    return { fromString(this->p_playlist->collection()->fullName()) };
}

Optional<Volume> MutablePlaylistItem::maybeParentCollectionVolume() const
{
    return this->p_playlist->collection()->volume();
}

boolean MutablePlaylistItem::parentCollectionMustHaveTracksOnTheSameVolume() const
{
    return this->p_playlist->collection()->mustHaveTracksOnTheSameVolume();
}

boolean MutablePlaylistItem::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_playlist->hasTracksNotOnVolume(volume);
}

boolean MutablePlaylistItem::parentCollectionAllowsMovieTracksInPlaylists() const
{
    return this->p_playlist->collection()->allowsMovieTracksInPlaylists();
}

boolean MutablePlaylistItem::hasMovieTracks() const
{
    return this->p_playlist->hasMovieTracks();
}

void MutablePlaylistItem::setName(const QString& name)
{
    this->p_playlist->setName(fromString(name));
}

QString MutablePlaylistItem::getName() const
{
    return fromString(this->p_playlist->name());
}

Optional<std::unique_ptr<AbstractGroupItem>> MutablePlaylistItem::predicates()
{
    return { };
}

count MutablePlaylistItem::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_playlist->numberOfCratesAndTrackEntriesContainedWithin();
}

count MutablePlaylistItem::numberOfTracks() const
{
    return this->p_playlist->numberOfTracks();
}

count MutablePlaylistItem::crateCount() const
{
    return this->p_playlist->numberOfSubCrates();
}

std::future<Optional<Shared<AbstractTrackListNode>>> MutablePlaylistItem::maybeTrackTree()
{
    return std::async([this]() {
        if (!this->p_treeCache.isValid()) {
            this->p_treeCache = Shared<AbstractTrackListNode>(
                    Shared<MutableTrackListNode>::with(NotNull<AbstractCrateItem*>{ dynamic_cast<AbstractCrateItem*>(this) }, nothing,
                                                       [this]() -> MutableTrackListNode::RefreshType {
                                                           auto initialModificationTime = this->p_playlist->lastModificationTime();

                                                           MutableTrackListNode::RefreshType result;

                                                           if (this->childCount() == 0) {
                                                               result = { this->p_playlist->tracks() };
                                                           }
                                                           else {
                                                               MutableArray<Common::MutableSubCrate> subCrates;

                                                               for (count i = 0; i < this->p_playlist->numberOfSubCrates(); ++i) {
                                                                   subCrates.emplaceAppend(this->p_playlist->subCrateAtIndex(i));
                                                               }

                                                               result = { Array<Common::MutableSubCrate>(subCrates) };
                                                           }

                                                           if (this->p_playlist->lastModificationTime() != initialModificationTime) {
                                                               static MutableSet<NotNull<Common::MutableCollection*>> collectionsAlreadyWarnedAbout;

                                                               if (collectionsAlreadyWarnedAbout.addingObjectCausedAnInsertion(this->p_playlist->collection())) {
                                                                   // -- If accessing the tracks modified them then this means something was fixed.
                                                                   Threading::runOnMainThread([]() {
                                                                       AlertMessageBox alert;
                                                                       alert.setText(QObject::tr(""),
                                                                                     QObject::tr("Looks like at least one playlist in this collection contains some corrupted data."),
                                                                                     QObject::tr("Any such playlist you browse will been fixed and saved if you choose to do so when you exit the app."));
                                                                       alert.setButtonLayout(QObject::tr("Ok"));
                                                                       alert.exec();
                                                                   }, Threading::Blocking::No);
                                                               }
                                                           }

                                                           return result;
                                                       },
                                                       fromString(this->p_playlist->name())));
        }

        return this->p_treeCache;
    });
}

boolean MutablePlaylistItem::isOrganizedBy(Common::Property::TypeID typeID) const
{
    return this->p_playlist->isOrganizedBy(typeID);
}

void MutablePlaylistItem::organizeBy(Common::Property::TypeID typeID)
{
    this->p_playlist->organizeBy(typeID);
}

void MutablePlaylistItem::removeOrganizationBy(Common::Property::TypeID typeID)
{
    this->p_playlist->removeOrganizationBy(typeID);
}

boolean MutablePlaylistItem::isOrganizedBy(const Common::Tag& tag) const
{
    return this->p_playlist->isOrganizedBy(tag);
}

void MutablePlaylistItem::organizeBy(Common::MutableTag& tag)
{
    this->p_playlist->organizeBy(tag);
}

void MutablePlaylistItem::removeOrganizationBy(Common::MutableTag& tag)
{
    this->p_playlist->removeOrganizationBy(tag);
}

void MutablePlaylistItem::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
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
