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

#include <ViewCollection/Sidebar/ViewCollection.hpp>
#include <ViewCollection/Sidebar/AllTracksItem.hpp>
#include <ViewCollection/Sidebar/AllTracksHeader.hpp>
#include <ViewCollection/Sidebar/ArtistsHeader.hpp>
#include <ViewCollection/Sidebar/KeysHeader.hpp>
#include <ViewCollection/Sidebar/TagsHeader.hpp>
#include <ViewCollection/Sidebar/PlaylistsHeader.hpp>

#include <AppleMusicCollection/Collection.hpp>
#include <AppleMusicCollection/AppleMusic.hpp>

#include <UiMainWindow/MainWindow.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <UiModel/SidebarModel.hpp>

#include <WorkQueue/WorkQueue.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

ViewCollection::ViewCollection(Shared<Common::Collection> withCollection,
                               NotNull<SidebarModel*> model,
                               SidebarItem* parent,
                               WorkQueue& withQueue) : AbstractCollection{ model, parent, withQueue },
                                                       p_collection{ std::move(withCollection) }
{
    this->p_loading = false;
    this->p_loaded = this->p_collection->isOpened();
}

// -- Instance Methods

InternalItemReference ViewCollection::p_internalReferenceForChildAtIndex(count index) const
{
    // -- In the case of this collection, p_newChildForIndex doesn't really create a new object so it is ok to use here.
    return { this->p_newChildForIndex(index).asRawPointer() };
}

Shared<SidebarItem> ViewCollection::p_newChildForIndex(count index) const
{
    auto mutableThis = const_cast<ViewCollection*>(this);

    if (this->p_collection->type() == Common::Collection::Type::RekordBuddy) {
        switch (index) {
            case 0: {
                if (!this->p_maybeAllTracks.isValid()) {
                    mutableThis->p_maybeAllTracks = Shared<AllTracksItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeAllTracks;
            }
            case 1: {
                if (!this->p_maybeArtists.isValid()) {
                    mutableThis->p_maybeArtists = Shared<ArtistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeArtists;
            }
            case 2: {
                if (!this->p_maybeKeys.isValid()) {
                    mutableThis->p_maybeKeys = Shared<KeysHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeKeys;
            }
            case 3: {
                if (!this->p_maybeTags.isValid()) {
                    mutableThis->p_maybeTags = Shared<TagsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeTags;
            }
            case 4: {
                if (!this->p_maybePlaylists.isValid()) {
                    mutableThis->p_maybePlaylists = Shared<PlaylistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection->rootFolder());
                }

                return *this->p_maybePlaylists;
            }
        }
    }
    else {
        switch (index) {
            case 0: {
                if (!this->p_maybeAllTracks.isValid()) {
                    mutableThis->p_maybeAllTracks = Shared<AllTracksItem>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection.asRawPointer());
                }

                return *this->p_maybeAllTracks;
            }
            case 1: {
                if (!this->p_maybePlaylists.isValid()) {
                    mutableThis->p_maybePlaylists = Shared<PlaylistsHeader>::with(mutableThis->getModel(), mutableThis, mutableThis->p_queue, mutableThis->p_collection->rootFolder());
                }

                return *this->p_maybePlaylists;
            }
        }
    }

    NXA_ALOG("Invalid index");
}

NotNull<const Common::Collection*> ViewCollection::getCollection() const
{
    return { this->p_collection.asRawPointer() };
}

NotNull<Common::Collection*> ViewCollection::getCollection()
{
    return { this->p_collection.asRawPointer() };
}

count ViewCollection::numberOfCratesAndTrackEntriesContainedWithin() const
{
    return this->p_collection->rootFolder()->numberOfCratesAndTrackEntriesContainedWithin();
}

count ViewCollection::numberOfTracks() const
{
    return this->p_collection->tracks()->numberOfTracks();
}

Optional<Volume> ViewCollection::maybeParentCollectionVolume() const
{
    return this->p_collection->volume();
}

Optional<QString> ViewCollection::maybeParentCollectionTypeName() const
{
    return { fromString(Common::Collection::nameForType(this->p_collection->type())) };
}

Optional<QString> ViewCollection::maybeParentCollectionFullName() const
{
    return { fromString(this->p_collection->fullName()) };
}

boolean ViewCollection::hasTracksNotOnVolume(const Volume& volume) const
{
    return this->p_collection->hasTracksNotOnVolume(volume);
}

boolean ViewCollection::hasMovieTracks() const
{
    return this->p_collection->rootFolder()->hasMovieTracks();
}

QString ViewCollection::getName() const
{
    return fromString(this->p_collection->fullName());
}

Optional<QIcon> ViewCollection::maybeIcon() const
{
    if (!this->maybeCachedIcon.isValid()) {
        this->maybeCachedIcon = QIcon{ String::stringWithFormat("://Icons/%s.png", this->p_collection->iconName()).asUTF8() };
    }

    return this->maybeCachedIcon;
}

void ViewCollection::notifyUserPreferencesHaveChanged()
{
    this->p_collection->notifyUserPreferencesHaveChanged();
}

boolean ViewCollection::isOpened() const
{
    return this->p_collection->isOpened();
}

boolean ViewCollection::hasChangesToSave() const
{
    // -- Read-only collections will never have any changes to save
    return false;
}

boolean ViewCollection::needsLoading() const
{
    return !this->p_loading && !this->p_loaded;
}

std::future<integer64> ViewCollection::loadChildrenIfUnloaded(std::function<integer64()>&& callback)
{
    if (!this->needsLoading()) {
        std::promise<integer64> promise;
        promise.set_value(0);
        this->p_loading = false;
        this->p_loaded = true;
        return promise.get_future();
    }

    this->p_loading = true;

    auto futureResult = this->p_queue.enqueueJobWithStdFuture<integer64>(0, [this, callback](auto job) -> integer64 {
        if (this->p_collection->isOpened()) {
            return 0;
        }

        this->p_collection->open();

        if (this->p_collection->isOpened()) {
            p_loading = false;
            p_loaded = true;
        }

        auto indexToNotify = this->getModelIndex();
        QMetaObject::invokeMethod(this->getModel().get(), "notifyLayoutChangedAt", Qt::QueuedConnection, Q_ARG(QModelIndex, indexToNotify));

        return callback();
    });

    return futureResult;
}

Optional<QString> ViewCollection::getDescription() const
{
    return fromString(this->p_collection->description());
}

count ViewCollection::childCount() const
{
    if (this->p_loaded) {
        return (this->p_collection->type() == Common::Collection::Type::RekordBuddy) ? 5 : 2;
    }

    return 1;
}

boolean ViewCollection::canBeDeleted() const
{
    auto collectionType = this->p_collection->type();
    if (collectionType == Common::Collection::Type::iTunes) {
        auto asITunesCollection = dynamic_cast<const AppleMusic::Collection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asITunesCollection);

        return asITunesCollection->isAtAUserLocation();
    }

    return false;
}

boolean ViewCollection::saveChangesWithProgress(std::function<void(double)>&& progressCallBack)
{
    NXA_ALOG("Unsupported");

    // -- This has no specific meaning right now. We don't want receipt testing failure to return something different.
    return true;
}

boolean ViewCollection::offerToRemove()
{
    if (!ConfirmationMessageBox::promptForConfirmYesNo(QObject::tr("Are you sure you want to remove this collection?"),
                                                       QObject::tr("You will have to manually add it again in the future if you wish to modify it or import data from it."),
                                                       QObject::tr(""))) {
        return false;
    }

    auto collectionType = this->p_collection->type();
    if (collectionType == Common::Collection::Type::iTunes) {
        auto asITunesCollection = dynamic_cast<AppleMusic::Collection*>(this->p_collection.asRawPointer());
        NXA_ASSERT_NOT_NULL(asITunesCollection);

        AppleMusic::removeUserCollectionAt(asITunesCollection->xmlFilePath());
    }

    this->p_queue.enqueueJob(0, [this](auto) {
        this->getModel()->refreshCollections();
    });

    return true;
}

void ViewCollection::populateContextMenu(QMenu& contextMenu, NotNull<MainWindow*> ownerWindow, boolean onlyAdd)
{
    // -- Add items from parent first (order matters)
    SidebarItem::populateContextMenu(contextMenu, ownerWindow, onlyAdd);

    if (this->canBeDeleted()) {
        QAction* action = new QAction{ QObject::tr("Remove this Collection"), &contextMenu };
        QObject::connect(action, &QAction::triggered, ownerWindow.get(), [this]() {
            this->offerToRemove();
        });

        contextMenu.addAction(action);
    }
}
