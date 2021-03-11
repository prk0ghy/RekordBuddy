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

#include <MutableViewCollection/Sidebar/MutableViewCollection.hpp>
#include <MutableViewCollection/Sidebar/MutableAllTracksItem.hpp>
#include <MutableViewCollection/Sidebar/MutableAllTracksHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableArtistsHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableKeysHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableTagsHeader.hpp>
#include <MutableViewCollection/Sidebar/MutablePlaylistsHeader.hpp>
#include <MutableViewCollection/Sidebar/MutableFolderItem.hpp>

#include <ViewCollection/Sidebar/FolderItem.hpp>
#include <ViewCollection/Sidebar/ViewCollection.hpp>

#include <RekordboxCollection/Rekordbox.hpp>
#include <RekordboxCollection/Collection.hpp>

#include <TraktorCollection/Traktor.hpp>
#include <TraktorCollection/Collection.hpp>

#include <AppleMusicCollection/AppleMusic.hpp>
#include <AppleMusicCollection/Collection.hpp>

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/Folder.hpp>

#include <UiMainWindow/NewCollectionDialog.hpp>
#include <UiMainWindow/MainWindow.hpp>

#include <UiModel/SidebarModel.hpp>

#include <UiBase/AlertMessageBox.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/Threading.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

MutableViewCollection::MutableViewCollection(Shared<Common::MutableCollection> withCollection,
                                             NotNull<SidebarModel*> model,
                                             SidebarItem* parent,
                                             WorkQueue& withQueue) : AbstractCollection{ model,
                                                                                         parent,
                                                                                         withQueue },
                                                                     p_collection{ std::move(withCollection) }
{
    this->p_loading = false;
    this->p_loaded = this->p_collection->isOpened();
}

// -- Instance Methods

InternalItemReference MutableViewCollection::p_internalReferenceForChildAtIndex(count index) const
{
    // -- In the case of this collection, p_newChildForIndex doesn't really create a new object so it is ok to use here.
    return { this->p_newChildForIndex(index).asRawPointer() };
}

Shared<SidebarItem> MutableViewCollection::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<MutableViewCollection*>(this);

    if (this->p_collection->type() == Common::Collection::Type::RekordBuddy) {
        switch (index) {
            case 0: {
                if (!this->p_maybeAllTracks.isValid()) {
                    mutableThis->p_maybeAllTracks = Shared<MutableAllTracksItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeAllTracks;
            }
            case 1: {
                if (!this->p_maybeArtists.isValid()) {
                    mutableThis->p_maybeArtists = Shared<MutableArtistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeArtists;
            }
            case 2: {
                if (!this->p_maybeKeys.isValid()) {
                    mutableThis->p_maybeKeys = Shared<MutableKeysHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeKeys;
            }
            case 3: {
                if (!this->p_maybeTags.isValid()) {
                    mutableThis->p_maybeTags = Shared<MutableTagsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeTags;
            }
            case 4: {
                if (!this->p_maybePlaylists.isValid()) {
                    mutableThis->p_maybePlaylists = Shared<MutablePlaylistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection->rootFolder());
                }

                return *this->p_maybePlaylists;
            }
        }
    }
    else {
        switch (index) {
            case 0: {
                if (!this->p_maybeAllTracks.isValid()) {
                    mutableThis->p_maybeAllTracks = Shared<MutableAllTracksItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeAllTracks;
            }
            case 1: {
                if (!this->p_maybePlaylists.isValid()) {
                    mutableThis->p_maybePlaylists = Shared<MutablePlaylistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection->rootFolder());
                }

                return *this->p_maybePlaylists;
            }
        }
    }

    NXA_ALOG("Invalid index");
}

NotNull<const Common::MutableCollection*> MutableViewCollection::getCollection() const
{
    return { this->p_collection.asRawPointer() };
}

NotNull<Common::MutableCollection*> MutableViewCollection::getCollection()
{
    return { this->p_collection.asRawPointer() };
}

count MutableViewCollection::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_collection->rootFolder()->numberOfCratesAndTrackEntriesContainedWithin();
}

count MutableViewCollection::numberOfTracks() const
{
    return this->p_collection->tracks()->numberOfTracks();
}

Optional<Volume> MutableViewCollection::maybeParentCollectionVolume() const
{
    return this->p_collection->volume();
}

Optional<QString> MutableViewCollection::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_collection->type())) };
}

Optional<QString> MutableViewCollection::maybeParentCollectionFullName() const
{
    return { fromString(this->p_collection->fullName()) };
}

boolean MutableViewCollection::parentCollectionMustHaveTracksOnTheSameVolume() const
{
    return this->p_collection->mustHaveTracksOnTheSameVolume();
}

boolean MutableViewCollection::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_collection->hasTracksNotOnVolume(volume);
}

boolean MutableViewCollection::parentCollectionAllowsMovieTracksInPlaylists() const
{
    return this->p_collection->allowsMovieTracksInPlaylists();
}

boolean MutableViewCollection::hasMovieTracks() const
{
    return this->p_collection->rootFolder()->hasMovieTracks();
}

void MutableViewCollection::forceRefresh()
{
    auto currentNumberOfChildren = this->childCount();
    if (!currentNumberOfChildren) {
        return;
    }

    auto model = this->getModel();
    auto modelIndex = this->getModelIndex();

    model->beginRemoveRows(modelIndex, 0, currentNumberOfChildren - 1);

    this->SidebarItem::forceRefresh();

    this->p_loading = false;
    this->p_loaded = this->p_collection->isOpened();

    this->p_maybeAllTracks = nothing;
    this->p_maybeArtists = nothing;
    this->p_maybeKeys = nothing;
    this->p_maybeTags = nothing;
    this->p_maybePlaylists = nothing;

    model->endRemoveRows();

    model->notifyLayoutChangedAt(modelIndex);
}

QString MutableViewCollection::getName() const
{
    return fromString(this->p_collection->fullName());
}

Optional<QIcon> MutableViewCollection::maybeIcon() const
{
    if (!this->maybeCachedIcon.isValid()) {
        this->maybeCachedIcon = QIcon{ String::stringWithFormat("://Icons/%s.png", this->p_collection->iconName()).asUTF8() };
    }

    return this->maybeCachedIcon;
}

void MutableViewCollection::notifyUserPreferencesHaveChanged()
{
    this->p_collection->notifyUserPreferencesHaveChanged();
}

boolean MutableViewCollection::isOpened() const
{
    return this->p_collection->isOpened();
}

boolean MutableViewCollection::hasChangesToSave() const
{
    return this->p_collection->hasChangesToSave();
}

boolean MutableViewCollection::needsLoading() const
{
    return !this->p_loading && !this->p_loaded;
}

std::future<integer64> MutableViewCollection::loadChildrenIfUnloaded(std::function<integer64()>&& callback)
{
    if (!this->needsLoading()) {
        std::promise<integer64> promise;
        promise.set_value(0);
        this->p_loading = false;
        this->p_loaded = true;
        return promise.get_future();
    }
    this->p_loading = true;

    auto volumeToUse = (this->p_collection->type() == Common::Collection::Type::RekordBuddy) ? this->p_collection->volume() : Volume::musicFolderVolume();
    auto futureResult = this->p_queue.enqueueJobWithStdFuture<integer64>(0, [this, callback](auto job) -> integer64 {
        if (this->p_collection->isOpened()) {
            return 0;
        }

        job->updateStatus(QString("Opening %1.").arg(fromString(this->p_collection->fullName())));
        if (this->p_collection->open().isValid()) {
            Threading::runOnMainThread([](){
                AlertMessageBox alert;
                alert.setButtonLayout(QObject::tr("Ok"));
                alert.setText(QObject::tr("Error opening collection."), QObject::tr("Operation returned an error."), "");
                alert.exec();
            }, Threading::Blocking::Yes);
        }

        p_loading = false;

        if (this->p_collection->isOpened()) {
            p_loaded = true;

            auto indexToNotify = this->getModelIndex();
            QMetaObject::invokeMethod(this->getModel().get(), "notifyLayoutChangedAt", Qt::QueuedConnection, Q_ARG(QModelIndex, indexToNotify));
        }

        return callback();
    });

    return futureResult;
}

Optional<QString> MutableViewCollection::getDescription() const
{
    return fromString(this->p_collection->description());
}

count MutableViewCollection::childCount() const
{
    if (this->p_loaded) {
        return (this->p_collection->type() == Common::Collection::Type::RekordBuddy) ? 5 : 2;
    }

    return 1;
}

boolean MutableViewCollection::canBeDeleted() const
{
    auto collectionType = this->p_collection->type();
    if (collectionType == Common::Collection::Type::rekordbox) {
        auto asRekordboxCollection = dynamic_cast<const Rekordbox::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asRekordboxCollection);

        return asRekordboxCollection->isAtAUserLocation();
    }
    else if (collectionType == Common::Collection::Type::Traktor) {
        auto asTraktorCollection = dynamic_cast<const Traktor::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asTraktorCollection);

        return asTraktorCollection->isAtAUserLocation();
    }
    else if (collectionType == Common::Collection::Type::iTunes) {
#if 0
        // -- Mutable iTunes collections don't exist yet.
        auto asITunesCollection = dynamic_cast<const iTunes::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asITunesCollection);

        return asITunesCollection->isAtAUserLocation();
#endif
    }

    return false;
}

boolean MutableViewCollection::canReceiveCollection() const
{
    return true;
}

boolean MutableViewCollection::canReceive(NotNull<AbstractCollection*> item) const
{
    if (item.get() == this) {
        // -- We can't drop a collection on itself.
        return false;
    }

    // -- We are only testing to see if this can receive the item so casting here is fine in order to get mutable items back.
    auto thisCollection = const_cast<MutableViewCollection*>(this)->p_collection;
    if (!thisCollection->isOpened()) {
        // -- We can only receive in opened collections.
        return false;
    }

    auto thisRootFolder = thisCollection->rootFolder();

    auto maybeCollectionItem = dynamic_cast<ViewCollection*>(item.get());
    if (maybeCollectionItem != nullptr) {
        auto otherRootFolder = maybeCollectionItem->getCollection()->rootFolder();
        for (count index = 0; index < otherRootFolder->numberOfSubCrates(); ++index) {
            if (!otherRootFolder->subCrateAtIndex(index).apply([&thisRootFolder](auto& subCrate) {
                return thisRootFolder->canReceive(subCrate);
            })) {
                return false;
            }
        }

        return true;
    }

    auto maybeMutableCollectionItem = dynamic_cast<MutableViewCollection*>(item.get());
    if (maybeMutableCollectionItem != nullptr) {
        auto otherRootFolder = maybeMutableCollectionItem->getCollection()->rootFolder();
        for (count index = 0; index < otherRootFolder->numberOfSubCrates(); ++index) {
            if (!otherRootFolder->subCrateAtIndex(index).apply([&thisRootFolder](auto& subCrate) {
                return thisRootFolder->canReceive(subCrate);
            })) {
                return false;
            }
        }

        return true;
    }

    return false;
}

void MutableViewCollection::receiveDraggedCollectionWithPerItemProgressCallBack(NotNull<AbstractCollection*> item,
                                                                                std::function<void(void)>&& callback)
{
    auto thisCollection = this->p_collection;
    NXA_ASSERT_TRUE(thisCollection->isOpened());

    auto thisAllTrackPlaylist = thisCollection->tracks();
    auto thisRootFolder = thisCollection->rootFolder();

    auto maybeCollectionItem = dynamic_cast<ViewCollection*>(item.get());
    if (maybeCollectionItem != nullptr) {
        auto otherCollection = maybeCollectionItem->getCollection();
        auto otherAllTrackPlaylist = otherCollection->tracks();
        auto otherRootFolder = otherCollection->rootFolder();

        thisAllTrackPlaylist->setWithSameTracksAsWithPerItemProgressCallBack(*otherAllTrackPlaylist,
                                                                             callback,
                                                                             Common::MutablePlaylist::AndUpdateTracks::Yes);
        thisRootFolder->setWithSameSubCratesAsWithPerItemProgressCallBack(*otherRootFolder,
                                                                          callback,
                                                                          Common::MutableFolder::AndUpdateTracks::No);
    }
    else {
        auto maybeMutableCollectionItem = dynamic_cast<MutableViewCollection*>(item.get());
        if (maybeMutableCollectionItem != nullptr) {
            auto otherCollection = maybeMutableCollectionItem->getCollection();
            auto otherAllTrackPlaylist = otherCollection->tracks();
            auto otherRootFolder = otherCollection->rootFolder();

            thisAllTrackPlaylist->setWithSameTracksAsWithPerItemProgressCallBack(*otherAllTrackPlaylist,
                                                                                 callback,
                                                                                 Common::MutablePlaylist::AndUpdateTracks::Yes);
            thisRootFolder->setWithSameSubCratesAsWithPerItemProgressCallBack(*otherRootFolder,
                                                                              callback,
                                                                              Common::MutableFolder::AndUpdateTracks::No);
        }
    }

    this->forceRefresh();

    auto tracksThatHaveChanged = this->unionOfAllTrackItemsContainedWithin();
    this->maybeUpdateBecauseTracksChanged(Array<NotNull<AbstractTrackItem*>>{ std::move(tracksThatHaveChanged) });

    this->getModel()->notifyLayoutChangedAt(this->getModelIndex().parent());
}

boolean MutableViewCollection::canReceiveFiles() const
{
    return this->p_collection->canReceiveFiles();
}

boolean MutableViewCollection::receiveFileAt(const FilePath& filePath)
{
    Optional<String> maybeError;

    if (!this->p_collection->canReceiveFileAt(filePath, maybeError)) {
        if (maybeError.isValid()) {
            AlertMessageBox alert;
            alert.setButtonLayout(QObject::tr("Ok"));
            alert.setText(QObject::tr("Can't use this XML file."), QObject::tr("Operation returned an error."), fromString(*maybeError));
            alert.exec();
        }

        return false;
    }

    if (!ConfirmationMessageBox::promptForConfirmYesNo(QObject::tr("Update the %1 collection?").arg(Common::Collection::nameForType(this->p_collection->type()).asUTF8()),
                                                       QObject::tr("Choose YES if you want to replace the content of that collection with the content of this file."),
                                                       QObject::tr("You can still choose not to save these changes when you exit the app."))) {
        return false;
    }

    if (!this->p_collection->receiveFileAt(filePath)) {
        return false;
    }

    this->forceRefresh();

    return true;
}

boolean MutableViewCollection::saveChangesWithProgress(std::function<void(double)>&& progressCallBack)
{
    this->p_collection->saveWithProgress(std::move(progressCallBack));

    // -- This has no specific meaning right now. We don't want receipt testing failure to return something different.
    return true;
}

boolean MutableViewCollection::offerToRemove()
{
    if (!ConfirmationMessageBox::promptForConfirmYesNo(QObject::tr("Are you sure you want to remove this collection?"),
                                                       QObject::tr("You will have to manually add it again in the future if you wish to modify it or import data from it."),
                                                       QObject::tr(""))) {
        return false;
    }

    if (this->hasChangesToSave()) {
        if (ConfirmationMessageBox::promptForConfirmYesNo(QObject::tr("Save changes before removing this collection?"),
                                                          QObject::tr("This collection has changes to save. If you choose to NO, those changes will be lost."),
                                                          QObject::tr("This cannot be undone."))) {
            this->saveChangesWithProgress([](double) { });
        }
    }

    auto collectionType = this->p_collection->type();
    if (collectionType == Common::Collection::Type::rekordbox) {
        auto asRekordboxCollection = dynamic_cast<Rekordbox::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asRekordboxCollection);

        Rekordbox::removeUserCollectionAt(asRekordboxCollection->xmlFilePath());
    }
    else if (collectionType == Common::Collection::Type::Traktor) {
        auto asTraktorCollection = dynamic_cast<Traktor::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asTraktorCollection);

        Traktor::removeUserCollectionAt(asTraktorCollection->nmlFilePath());
    }
    else if (collectionType == Common::Collection::Type::iTunes) {
#if 0
        // -- Mutable iTunes collections don't exist yet.
        auto asITunesCollection = dynamic_cast<iTunes::MutableCollection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asITunesCollection);

        iTunes::removeUserCollectionAt(asITunesCollection->xmlFilePath());
#endif
    }

    this->p_queue.enqueueJob(0, [this](auto) {
        this->getModel()->refreshCollections();
    });

    return true;
}

void MutableViewCollection::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
{
    // -- Add items from parent first (order matters)
    SidebarItem::populateContextMenu(contextMenu, ownerWindow, onlyAdd);

    auto collectionType = this->p_collection->type();
    if (collectionType == Common::Collection::Type::RekordBuddy) {
        QAction* action = new QAction{ QObject::tr("New Collection"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), [this, ownerWindow]() {
            NewCollectionDialog nc{ ownerWindow.get() };

            nc.execThenCall([this]() {
                this->p_queue.enqueueJob(0, [this](auto) {
                    this->getModel()->refreshCollections();
                });
            });
        });

        contextMenu.addAction(action);
    }
    else if (this->canBeDeleted()) {
        QAction* action = new QAction{ QObject::tr("Remove this Collection"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), [this, ownerWindow]() {
            ownerWindow->offerToRemoveCollectionAtIndex(this->getModelIndex());
        });

        contextMenu.addAction(action);
    }
}

void MutableViewCollection::maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>& tracks)
{
    if (this->p_maybeArtists.isValid()) {
        (*this->p_maybeArtists)->maybeUpdateBecauseTracksChanged(tracks);
    }

    if (this->p_maybeKeys.isValid()) {
        (*this->p_maybeKeys)->maybeUpdateBecauseTracksChanged(tracks);
    }

    if (this->p_maybeTags.isValid()) {
        (*this->p_maybeTags)->maybeUpdateBecauseTracksChanged(tracks);
    }
}
