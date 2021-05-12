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

#include <MutableViewCollection/Sidebar/MutableCollectionList.hpp>
#include <MutableViewCollection/Sidebar/MutableViewCollection.hpp>

#include <MutableViewCollection/TrackList/MutableTrackItem.hpp>

#include <ViewCollection/Sidebar/ViewCollection.hpp>

#include <AbstractViewCollection/Sidebar/CollectionOpenError.hpp>

#include <Collections/Collections.hpp>

#include <RekordboxCollection/Rekordbox.hpp>
#include <RekordboxCollection/Collection.hpp>

#include <AppleMusicCollection/AppleMusic.hpp>
#include <AppleMusicCollection/Collection.hpp>

#include <TraktorCollection/Traktor.hpp>

#include <UiModel/SidebarModel.hpp>
#include <UiBase/AlertMessageBox.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/CrashLog.hpp>
#include <Base/Utilities.hpp>

#include <algorithm>

using namespace NxA;
using namespace RekordBuddy;

// -- Static Methods

static Array<count> p_indicesOfCollectionsToRemoveBetween(const MutableArray<Common::CollectionOfSomeSort>& oldCollections,
                                                          const Array<Common::CollectionOfSomeSort>& newCollections)
{
    MutableArray<count> indicesInOldCollections;

    count indexInNewCollections = 0;
    count newCollectionsLength = newCollections.length();

    for (count indexInOldCollections = 0; indexInOldCollections < oldCollections.length(); ++indexInOldCollections) {
        auto oldCollection = oldCollections[indexInOldCollections];

        if ((indexInNewCollections < newCollectionsLength) &&
            (oldCollection == newCollections[indexInNewCollections])) {
            // -- If the collections are the same we move onto the next one.
            ++indexInNewCollections;
        }
        else {
            auto maybePosInNewCollection = newCollections.findStartingFromIndex(oldCollection, indexInNewCollections);

            if (maybePosInNewCollection == newCollections.end()) {
                // -- If we can't find this collection further down in the new collections it needs to be removed.
                indicesInOldCollections.append(indexInOldCollections);
            }
            else {
                // -- Otherwise we skip to the one we found further down in the new collections.
                indexInNewCollections = maybePosInNewCollection - newCollections.begin() + 1;
            }
        }
    }

    return std::move(indicesInOldCollections);
}

static Array<count> p_indicesOfCollectionsToAddBetween(const MutableArray<Common::CollectionOfSomeSort>& oldCollections,
                                                       const Array<Common::CollectionOfSomeSort>& newCollections)
{
    // -- Algorithm requires that we have more or the same amount of new collections than old ones.
    NXA_ASSERT_TRUE(newCollections.length() >= oldCollections.length());

    MutableArray<count> indicesInNewCollections;

    count indexInOldCollections = 0;
    count oldCollectionsLength = oldCollections.length();

    for (count indexInNewCollections = 0; indexInNewCollections < newCollections.length(); ++indexInNewCollections) {
        if ((indexInOldCollections < oldCollectionsLength) &&
            (oldCollections[indexInOldCollections] == newCollections[indexInNewCollections])) {
            // -- If the collections are the same we move onto the next one.
            ++indexInOldCollections;
        }
        else {
            indicesInNewCollections.append(indexInNewCollections);
        }
    }

    return std::move(indicesInNewCollections);
}

// -- Constructors & Destructors

MutableCollectionList::MutableCollectionList(NotNull<SidebarModel*> model,
                                             WorkQueue& withQueue) : AbstractCollectionList{ model,
                                                                               nullptr,
                                                                               withQueue }
{
    model->setCollectionList(this);
}

// -- Instance Methods

Shared<AbstractCollection> MutableCollectionList::p_viewCollectionWith(Shared<Common::Collection> withCollection)
{
    return Shared<ViewCollection>::with(withCollection, this->getModel(), this, this->p_queue);
}

Shared<AbstractCollection> MutableCollectionList::p_viewCollectionWith(Shared<Common::MutableCollection> withCollection)
{
    return Shared<MutableViewCollection>::with(withCollection, this->getModel(), this, this->p_queue);
}

InternalItemReference MutableCollectionList::p_internalReferenceForChildAtIndex(count index) const
{
    return { this->p_collectionsInSidebar[index].asRawPointer() };
}

Shared<SidebarItem> MutableCollectionList::p_newChildForIndex(count index) const
{
    return { this->p_collectionsInSidebar[index] };
}

Array<CollectionOpenError> MutableCollectionList::refreshCollectionsWithPerCollectionProgressCallBack(const std::function<void(void)>& callback)
{
    // -- This relies on the main thread being available in order to avoid deadlocks.
    NXA_ASSERT_FALSE_DEBUG(Threading::isOnMainThread());

    // -- We want to represent progress for finding then returning a collection so
    // -- we need to count both these actions and only callback every two actions.
    count overallProgress = 0;

    using FutureResult = Variant<CollectionOpenError, Shared<AbstractCollection>>;
    MutableArray<Optional<std::future<FutureResult>>> maybeFutureCollections;
    Array<Common::CollectionOfSomeSort> newCollections;

    auto model = this->getModel();
    auto collectionListIndex = this->getModelIndex();

    boolean checkMe = false;

    // -- We keep the previous collections around until we're done replacing them in the sidebar in case one is being removed by this operation.
    MutableArray<Common::CollectionOfSomeSort> previousCollections = this->p_currentCollections;

    Threading::runOnMainThread([this, &checkMe, &model, &newCollections, &collectionListIndex, &callback, &overallProgress, &maybeFutureCollections]() {
        newCollections = Common::availableCollectionsWithPerCollectionProgressCallBack([&callback, &overallProgress]() {
            if ((++overallProgress % 2) == 0) {
                callback();
            }
        });

        MutableSet<String> collectionNames;
        for (auto&& collection : newCollections) {
            withVariant(collection, [&collectionNames](auto collection) {
                auto type = collection->type();
                if (type != Common::Collection::Type::RekordBuddy) {
                    collectionNames.add(Common::Collection::nameForType(type));
                }
            });
        }

        auto indicesOfCollectionsToRemove = p_indicesOfCollectionsToRemoveBetween(this->p_currentCollections, newCollections);

        count numberOfItemsRemoved = 0;
        count currentIndex;

        for (currentIndex = 0; currentIndex < indicesOfCollectionsToRemove.length(); ++currentIndex) {
            auto index = indicesOfCollectionsToRemove[currentIndex];

            // -- If we have already removed collections, we need to adjust the index accordingly.
            index -= numberOfItemsRemoved;

            model->beginRemoveRows(collectionListIndex, index, index);

            this->removeChildAtIndex(index);

            model->endRemoveRows();

            ++numberOfItemsRemoved;
        }

        auto indicesOfCollectionsToAdd = p_indicesOfCollectionsToAddBetween(this->p_currentCollections, newCollections);
        this->p_currentCollections = newCollections;
        count newCollectionIndex;

        for (newCollectionIndex = 0; newCollectionIndex < this->p_currentCollections.length(); ++newCollectionIndex) {
            if (!indicesOfCollectionsToAdd.contains(newCollectionIndex)) {
                maybeFutureCollections.append(nothing);
            }
            else {
                maybeFutureCollections.emplaceAppend(this->p_queue.enqueueJobWithStdFuture<FutureResult>(0, [this, newCollectionIndex](auto job) mutable -> FutureResult {
                    return withVariant(this->p_currentCollections[newCollectionIndex], [this](auto collection) -> FutureResult {
                        auto foundString = String::stringWithFormat("Found %s database on volume '%s' (%s)",
                                                                    collection->name().asUTF8(),
                                                                    collection->volume().name().asUTF8(),
                                                                    collection->volume().asFilePath().asEncodedString().asUTF8());
                        NXA_DLOG_WITH_FORMAT("%s", foundString.asUTF8());
#if defined(NXA_BUGSNAG_APP_ID)
                        CrashLog::addBreadCrumb(foundString);
#endif

                        if (collection->shouldBeOpenedLazily()) {
                            // -- Return it unopened so it maybe can be opened lazy
                            return this->p_viewCollectionWith(collection);
                        }

                        auto maybeError = collection->open();
                        if (!maybeError.isValid()) {
                            auto openedString = String::stringWithFormat("Opened %s database on volume '%s' (%s)",
                                                                         collection->name().asUTF8(),
                                                                         collection->volume().name().asUTF8(),
                                                                         collection->volume().asFilePath().asEncodedString().asUTF8());
                            NXA_DLOG_WITH_FORMAT("%s", openedString.asUTF8());
#if defined(NXA_BUGSNAG_APP_ID)
                            CrashLog::addBreadCrumb(openedString);
#endif
                            return this->p_viewCollectionWith(collection);
                        }

                        switch (*maybeError) {
                            case Common::Collection::Error::NotFound: {
                                NXA_DLOG_WITH_FORMAT("Ignoring %s database on volume '%s' (not found).", collection->name().asUTF8(), collection->volume().name().asUTF8());
                                break;
                            }
                            case Common::Collection::Error::InvalidLicense: {
                                NXA_DLOG_WITH_FORMAT("Error opening %s database on volume '%s' (invalid license).", collection->name().asUTF8(), collection->volume().name().asUTF8());
                                return CollectionOpenError{ CollectionOpenError::Type::InvalidLicence,
                                                            collection->type(),
                                                            collection->name(),
                                                            collection->volume() };
                            }
                            case Common::Collection::Error::Corrupted:
                            case Common::Collection::Error::NeedsUpgrading:
                            case Common::Collection::Error::CannotOpen: {
                                NXA_DLOG_WITH_FORMAT("Error opening %s database on volume '%s' (corrupted or needs upgrading).", collection->name().asUTF8(), collection->volume().name().asUTF8());
                                return CollectionOpenError{ CollectionOpenError::Type::ResetCollection,
                                                            collection->type(),
                                                            collection->name(),
                                                            collection->volume() };
                            }
                        }

                        return CollectionOpenError{ CollectionOpenError::Type::IgnoreCollection,
                                                    collection->type(),
                                                    collection->name(),
                                                    collection->volume() };
                    });
                }));
            }
        }

        checkMe = true;
    }, Threading::Blocking::Yes);

    NXA_ASSERT_TRUE(checkMe);

    MutableArray<CollectionOpenError> collectionErrors;
    count sidebarIndex = 0;
    count collectionIndex = 0;

    for (auto&& maybeFutureCollection : maybeFutureCollections) {
        if (!maybeFutureCollection.isValid()) {
            // -- This collection must already exists.
            auto existingCollection = this->p_currentCollections[collectionIndex];
            withVariant(existingCollection, [&sidebarIndex](auto&& collection) {
                if (!collection->lastOpenResult().isValid()) {
                    // -- It does exist and it did not fail to open last time so it's in our sidebar.
                    ++sidebarIndex;
                }
            });
        }
        else {
            auto collectionOrError = (*maybeFutureCollection).get();
            if (collectionOrError.isType<CollectionOpenError>()) {
                // -- This is an error caused by opening this collection, we save it.
                collectionErrors.append(collectionOrError.get<CollectionOpenError>());
            }
            else {
                Threading::runOnMainThread([this, model, collectionListIndex, &collectionOrError, &sidebarIndex]() {
                                               // -- This collection is new, we need to insert it in our collections.
                                               model->beginInsertRows(collectionListIndex, sidebarIndex, sidebarIndex);

                                               this->p_collectionsInSidebar.insertObjectAtIndex(collectionOrError.get<Shared<AbstractCollection>>(), sidebarIndex++);

                                               model->endInsertRows();
                                           }, Threading::Blocking::Yes);
            }
        }

        ++collectionIndex;

        if ((++overallProgress % 2) == 0) {
            callback();
        }
    }

    model->notifyLayoutChangedAt(collectionListIndex);

    return { collectionErrors };
}

count MutableCollectionList::childCount() const
{
    return this->p_collectionsInSidebar.length();
}

QString MutableCollectionList::getName() const
{
    // -- Not shown by default tree view
    return "[Root]";
}

void MutableCollectionList::notifyUserPreferencesHaveChanged()
{
    for (auto&& collection : this->p_collectionsInSidebar) {
        collection->notifyUserPreferencesHaveChanged();
    }
}

boolean MutableCollectionList::hasChangesToSave() const
{
    for (auto&& collection : this->p_collectionsInSidebar) {
        if (collection->hasChangesToSave()) {
            return true;
        }
    }

    return false;
}

boolean MutableCollectionList::saveChangesWithProgress(std::function<void(double)>&& progressCallBack)
{
    auto numberOfCollections = this->p_collectionsInSidebar.length();
    double total = 0.0;

    for (count index = 0; index < numberOfCollections; ++index) {
        auto& collection = this->p_collectionsInSidebar[index];

        if (collection->hasChangesToSave()) {
#if defined(NXA_BUGSNAG_APP_ID)
            CrashLog::addBreadCrumb(String::stringWithFormat("Saving %s.", collection->getName().toUtf8().constData()));
#endif

            collection->saveChangesWithProgress([numberOfCollections, progressCallBack, &total](double a) {
                total += a / numberOfCollections;
                progressCallBack(total);
            });

#if defined(NXA_BUGSNAG_APP_ID)
            CrashLog::addBreadCrumb("Done.");
#endif
        }
        else {
            total += 1.0 / numberOfCollections;
        }
    }

    // -- This has no specific meaning right now.
    return true;
}

boolean MutableCollectionList::receiveFileAt(const FilePath& path)
{
    boolean result = false;
    Optional<String> maybeError;

    if (path.hasExtension(NXA_FILEPATH("xml"), FilePath::CaseSensitivity::None)) {
        boolean fileIsValidRekordboxXML = false;
        boolean fileIsValidITunesXML = false;

        auto maybeFileContent = File::maybeContentOfFileAtAsString(path);
        if (maybeFileContent.isValid()) {
            auto maybeXMLDocument = XMLDocument::maybeWithString(*maybeFileContent);
            if (maybeXMLDocument.isValid()) {
                maybeError = Rekordbox::MutableCollection::maybeErrorIfNotValidRekordboxXML(**maybeXMLDocument);
                if (!maybeError.isValid()) {
                    fileIsValidRekordboxXML = true;
                }
                else if (AppleMusic::Collection::isAValidAndSupportedITunesXML(**maybeXMLDocument)) {
                    fileIsValidITunesXML = true;
                }
            }
        }

        if (fileIsValidRekordboxXML) {
            result = Rekordbox::addUserCollectionAt(path);
        }
        else if (fileIsValidITunesXML) {
            result = AppleMusic::addUserCollectionAt(path);
        }
    }
    else if (path.hasExtension(NXA_FILEPATH("nml"), FilePath::CaseSensitivity::None)) {
        result = Traktor::addUserCollectionAt(path);
    }

    if (!result) {
        AlertMessageBox alert;
        alert.setButtonLayout(QObject::tr("Ok"));
        alert.setText(QObject::tr("Can't add this collection"),
                      maybeError.isValid() ? QObject::tr("Operation returned an error.") : QObject::tr("Looks like it is already in the sidebar."),
                      maybeError.isValid() ? fromString(*maybeError) : QObject::tr("You may be dragging file that's located in a standard location instead of a user location."));
        alert.exec();
    }

    return result;
}

void MutableCollectionList::maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>& tracks)
{
    MutableSet<NotNull<Common::MutableCollection*>> collectionsAffected;
    for (auto&& track : tracks) {
        auto maybeAsMutableTrackItem = track.maybeAs<MutableTrackItem*>();
        if (!maybeAsMutableTrackItem.isValid()) {
            // -- If this track is not mutable then it couldn't have changed so we ignore it.
            continue;
        }

        collectionsAffected.add((*maybeAsMutableTrackItem)->getTrack()->collection());
    }

    for (auto&& collection : this->p_collectionsInSidebar) {
        auto* maybeAsMutableViewCollection = dynamic_cast<MutableViewCollection*>(collection.asRawPointer());
        if (maybeAsMutableViewCollection == nullptr) {
            // -- If this collection is not mutable then it can contains tracks that change so we skip it.
            continue;
        }

        if (!collectionsAffected.contains(maybeAsMutableViewCollection->getCollection())) {
            continue;
        }

        maybeAsMutableViewCollection->maybeUpdateBecauseTracksChanged(tracks);
    }
}
