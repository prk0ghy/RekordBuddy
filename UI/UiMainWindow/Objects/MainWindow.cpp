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

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <AbstractViewCollection/Sidebar/AbstractCrateItem.hpp>

#include <Collections/Collections.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <MutableViewCollection/Predicate/MutableGroupItem.hpp>
#include <MutableViewCollection/Sidebar/MutableCollectionList.hpp>
#include <MutableViewCollection/Sidebar/MutableFolderItem.hpp>
#include <MutableViewCollection/Sidebar/MutablePlaylistItem.hpp>
#include <MutableViewCollection/Sidebar/MutableViewCollection.hpp>
#include <MutableViewCollection/TrackList/MutableTrackItem.hpp>
#include <MutableViewCollection/TrackList/MutableTrackListNode.hpp>

#include <UiMainWindow/AboutDialog.hpp>
#include <UiMainWindow/MainWindow.hpp>
#include <UiMainWindow/PredicateEditorDialog.hpp>
#include <UiMainWindow/TrackEditDialog.hpp>

#include <UiModel/Preferences/PreferencesModuleManager.hpp>
#include <UiModel/SidebarModel.hpp>
#include <UiModel/TrackListAlbumModel.hpp>
#include <UiModel/TrackListDetailsModel.hpp>
#include <UiModel/TrackListSortingModel.hpp>

#include <UiPreferences/PreferencesWindow.hpp>

#include <UiUpdater/Updater.hpp>

#include <UiBase/AlertMessageBox.hpp>
#include <UiBase/variantInterop.hpp>

#include <Widgets/ArtworkTrackViewDelegate.hpp>
#include <Widgets/ConfirmationMessageBox.hpp>
#include <Widgets/DetailsTrackViewDelegate.hpp>
#include <Widgets/PlayerWidget.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <qmessagebox.h>
#include <qevent.h>
#include <qtimer.h>
#include <qsettings.h>
#include <qdesktopservices.h>

#include <future>

#include "nxa_build_defines.h"
#include "rkb_build_defines.h"

#include "ui_MainWindow.h"
#include "UiMainWindow/moc_MainWindow.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Private variables

static NxA::boolean p_userChoiceForReset{ false };
static Semaphore p_userAcknowledgedDialog;

// -- Private functions

static std::unique_ptr<Updater> getAutoUpdater()
{
    // -- Bug fix builds need to still update when a new release build is posted.
    // -- Beta builds only update when a new beta build is posted.
#if defined(NXA_BETA_BUILD) && !defined(NXA_BUGFIX_BUILD)
    String beta{ "beta" };
#else
    String beta;
#endif

    auto url = String::stringWithFormat("https://rekordbuddy.org/updates/%s.%s%s/Appcast.xml",
                                        RKB_MAJOR_VERSION,
                                        RKB_MINOR_VERSION,
                                        beta.asUTF8());
    return std::make_unique<Updater>(url);
}

// -- Constructors & Destructors

MainWindow::MainWindow(WorkQueue& withWorkQueue, QWidget* parent) : QMainWindow{ parent },
                                                                    workQueue{ withWorkQueue },
                                                                    updater{ nullptr },
                                                                    periodicUpdater{ nullptr },
                                                                    detailsSortProxy{ new TrackListSortingModel{ this } },
                                                                    trackListDetailsModel{ new TrackListDetailsModel{ withWorkQueue, this } },
                                                                    trackListAlbumModel{ new TrackListAlbumModel{ withWorkQueue, this } },
                                                                    ui{ std::make_unique<Ui::MainWindow>() }
{
    this->ui->setupUi(this);

    this->ui->progressView->associateWithWorkQueue(&withWorkQueue);
    this->ui->progressView->setHidden(true);

    this->aboutDialog = std::make_unique<AboutDialog>(withWorkQueue, this, AboutDialog::Mode::About);
    this->preferencesWindow = std::make_unique<PreferencesWindow>(this);

    // -- Bind signals/slots
    connect(this->ui->sidebarView, &QAbstractItemView::activated, this, &MainWindow::selectSidebarItem);
    connect(this->ui->sidebarView, &QAbstractItemView::clicked, this, &MainWindow::selectSidebarItem);
    connect(this->ui->sidebarView, &QTreeView::expanded, this, &MainWindow::expandSidebarItem);
    connect(this->ui->tracklistDetailsView, &QAbstractItemView::clicked, this, &MainWindow::clickTrack);
    connect(this->ui->tracklistAlbumView, &QAbstractItemView::clicked, this, &MainWindow::clickTrack);
    connect(this->ui->tracklistDetailsView, &QAbstractItemView::activated, this, &MainWindow::selectTrackItem);
    connect(this->ui->tracklistAlbumView, &QAbstractItemView::activated, this, &MainWindow::selectTrackItem);
    connect(this->ui->sidebarView, &QWidget::customContextMenuRequested, this, &MainWindow::showSidebarContextMenu);
    connect(this->ui->tracklistDetailsView, &QWidget::customContextMenuRequested, this, &MainWindow::showTrackDetailContextMenu);
    connect(this->ui->tracklistAlbumView, &QWidget::customContextMenuRequested, this, &MainWindow::showTrackAlbumContextMenu);
    connect(this->ui->actionAbout_Rekord_Buddy, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(this->ui->actionSaveCollection, &QAction::triggered, this, &MainWindow::saveCollections);
    connect(this->ui->actionArtwork_View, &QAction::triggered, this, &MainWindow::setViewModeArtwork);
    connect(this->ui->actionTrack_View, &QAction::triggered, this, &MainWindow::setViewModeDetails);
    connect(this->ui->actionCheck_for_Updates, &QAction::triggered, this, &MainWindow::checkForUpdates);
    connect(this->ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferencesWindow);
    connect(this->ui->actionGetting_Started, &QAction::triggered, this, &MainWindow::showGettingStarted);
    connect(this->ui->actionShow_Online_Manual, &QAction::triggered, this, &MainWindow::showOnlineManual);
    connect(this->ui->actionShow_Online_Support, &QAction::triggered, this, &MainWindow::showOnlineSupport);

    connect(&this->workQueue, &WorkQueue::didBeginWorking, this, &MainWindow::didBeginWorking);
    connect(&this->workQueue, &WorkQueue::didPauseWorking, this, &MainWindow::didPauseWorking);

    qRegisterMetaType<CollectionOpenError>("CollectionOpenError");

    // -- uniform row heights mean we'll demand-load data from the model, instead of loading it all at once.
    this->ui->tracklistAlbumView->setUniformRowHeights(true);
    this->ui->tracklistAlbumView->setItemDelegate(new ArtworkTrackViewDelegate{ this->ui->tracklistAlbumView });
    this->ui->tracklistAlbumView->setDragDropMode(QTreeView::DragDrop);
    this->ui->tracklistAlbumView->setDropIndicatorShown(true);

    // -- uniform row heights mean we'll demand-load data from the model, instead of loading it all at once.
    this->ui->tracklistDetailsView->setUniformRowHeights(true);
    this->ui->tracklistDetailsView->setItemDelegate(new DetailsTrackViewDelegate{ this->ui->tracklistDetailsView });
    this->ui->tracklistDetailsView->setDragDropMode(QTreeView::DragDrop);
    this->ui->tracklistDetailsView->setDropIndicatorShown(true);

    // -- restore window geometry and configuration
    this->loadWindowSettings();

    // -- Bind initial models
    this->detailsSortProxy->setSourceModel(this->trackListDetailsModel.get());
    this->detailsSortProxy->setSortLocaleAware(true);

    this->ui->tracklistDetailsView->setModel(this->detailsSortProxy.get());
    this->ui->tracklistAlbumView->setModel(this->trackListAlbumModel.get());

    this->updater = getAutoUpdater();

    this->periodicUpdater = new QTimer{ this };
    connect(this->periodicUpdater, &QTimer::timeout, this, &MainWindow::timeoutForUpdate);
    this->periodicUpdater->start(700);
}

MainWindow::~MainWindow() = default;

// -- Instance Methods

void MainWindow::loadWindowSettings()
{
    // -- Restore window geometry and configuration
    QSettings settings{ "RekordBuddy", "RekordBuddy" };

    restoreState(settings.value("mainWindowState").toByteArray());
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

    this->ui->splitter->restoreState(settings.value("splitter").toByteArray());

    if (settings.contains("pos")) {
        this->move(settings.value("pos").toPoint());
    }

    if (settings.contains("size")) {
        this->resize(settings.value("size").toSize());
    }

    if (settings.contains("isMax")) {
        if (settings.value("isMax").toBool()) {
            setWindowState(windowState() | Qt::WindowMaximized);
        }
    }
}

void MainWindow::saveWindowSettings()
{
    QSettings settings{ "RekordBuddy", "RekordBuddy" };
    settings.setValue("mainWindowState", saveState());
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("splitter", this->ui->splitter->saveState());

    if (this->isMaximized()) {
        settings.setValue("isMax", true);
    }
    else {
        settings.setValue("size", size());
        settings.setValue("isMax", false);
    }

    settings.setValue("pos", pos());

    this->trackListDetailsModel->saveColumnsOrder();
}

void MainWindow::showEvent(QShowEvent* event)
{
    this->loadWindowSettings();

    QMainWindow::showEvent(event);

    // -- We have to wait until the main window is on-screen to start this process
    this->updater->checkForUpdatesInBackground();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    static boolean reallyClose = false;
    if (reallyClose) {
        event->accept();
        QMainWindow::closeEvent(event);
        return;
    }

    CrashLog::addBreadCrumb("Exiting.");

    this->saveWindowSettings();

    PreferencesModuleManager::saveUserPreferencesChanges();

    auto hasChanges = this->sidebarModel->hasChangesToSave();
    if (!hasChanges) {
        event->accept();
        QMainWindow::closeEvent(event);
        return;
    }

    auto dialog = QMessageBox{ QObject::tr("Unsaved Changes"),
                               QObject::tr("There are unsaved changes. Do you want to save or discard?"),
                               QMessageBox::Warning,
                               QMessageBox::Save, QMessageBox::Discard, QMessageBox::Cancel,
                               this };
    dialog.setIconPixmap(QPixmap{ QStringLiteral(":/Icons/Warnings and Errors/Alert Icon/icon_512x512.png") }.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    dialog.setDefaultButton(QMessageBox::Cancel);

    auto result = dialog.exec();
    switch (result) {
        case QMessageBox::Save: {
            event->ignore();
            this->saveCollectionsThenCall([this]() {
                reallyClose = true;
                QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
            });
            return;
        }
        case QMessageBox::Discard: {
            reallyClose = true;
            event->accept();
            break;
        }
        default: {
            event->ignore();
            return;
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::saveCollectionsThenCall(const std::function<void()>& savedCallback)
{
    constexpr integer64 totalEffort = 200;
    workQueue.enqueueJob(totalEffort, [this, savedCallback](auto job) {
#if defined(DEBUG)
        auto start = Duration::startMeasuringDuration();
#endif
        job->updateStatus("Saving...");
        job->setProgress(0);

        integer64 oldProgress = 0;

        this->sidebarModel->saveChangesWithProgress([job, &oldProgress](double p) {
            auto newProgress = int(p * totalEffort);
            if (newProgress > oldProgress && newProgress <= totalEffort) {
                auto dots = newProgress % 4;
                job->updateStatus(QString{ QObject::tr("Saving%1") }.arg(dots == 0 ? "." : dots == 1 ? ".." : dots == 2 ? "..." : ""));
                job->setProgress(newProgress);
                oldProgress = newProgress;
            }
        });

        job->setProgress(totalEffort);

#if defined(DEBUG)
        NXA_DLOG_WITH_FORMAT("Save took %lldms", start.duration().toMilliseconds());
#endif
        savedCallback();
    });
}

void MainWindow::saveCollections()
{
    this->saveCollectionsThenCall([]() { });
}

static std::atomic_flag isTimeoutRunning = ATOMIC_FLAG_INIT;

void MainWindow::timeoutForUpdate()
{
    if (this->sidebarModel == nullptr) {
        return;
    }

    auto isRunning = isTimeoutRunning.test_and_set();
    if (!isRunning) {
        auto result = std::async([this]() {
            auto hasChanges = this->sidebarModel->hasChangesToSave();
            QMetaObject::invokeMethod(this, "setWindowModified", Qt::QueuedConnection, Q_ARG(bool, hasChanges));
            isTimeoutRunning.clear();
        });
    }
}

void MainWindow::editDetailTrackSelection()
{
    NXA_ASSERT_TRUE(this->currentTracksEdited.length() == 0);

    QModelIndexList indicies = this->ui->tracklistDetailsView->selectionModel()->selectedRows();

    boolean anyEditable = false;

    for (auto&& viewIndex : indicies) {
        auto index = this->detailsSortProxy->mapToSource(viewIndex);
        auto maybeTrack = this->trackListDetailsModel->maybeTrackAtIndex(index);
        if (!maybeTrack.isValid()) {
            continue;
        }

        auto& track = *maybeTrack;

        anyEditable = anyEditable || track->canEdit();

        this->currentTracksEdited.append(std::make_tuple(index, NotNull<NxA::RekordBuddy::AbstractTrackItem*>{ track.get() }));
    }

    TrackEditDialog trackEditDialog{ this->currentTracksEdited, anyEditable, &this->workQueue, this };
    connect(&trackEditDialog, &TrackEditDialog::tracksChanged, this, &MainWindow::tracksChanged);

    trackEditDialog.exec();

    this->currentTracksEdited.removeAll();
}

void MainWindow::editAlbumTrackSelection()
{
    NXA_ASSERT_TRUE(this->currentTracksEdited.length() == 0);

    QModelIndexList indicies = this->ui->tracklistAlbumView->selectionModel()->selectedRows();

    boolean anyEditable = false;

    for (auto&& index : indicies) {
        auto maybeTrack = this->trackListAlbumModel->maybeTrackAtIndex(index);
        if (!maybeTrack.isValid()) {
            continue;
        }

        auto& track = *maybeTrack;
        anyEditable = anyEditable || track->canEdit();
        this->currentTracksEdited.append(std::make_tuple(index,
                                                         NotNull<NxA::RekordBuddy::AbstractTrackItem*>{ track.get() }));
    }

    TrackEditDialog trackEditDialog{ this->currentTracksEdited, anyEditable, &this->workQueue, this };
    connect(&trackEditDialog, &TrackEditDialog::tracksChanged, this, &MainWindow::tracksChanged);

    trackEditDialog.exec();

    this->currentTracksEdited.removeAll();
}

void MainWindow::showTrackAlbumContextMenu(const QPoint& position)
{
    if (this->currentModel == nullptr) {
        return;
    }

    QModelIndexList indicies = this->ui->tracklistAlbumView->selectionModel()->selectedRows();

    if (!indicies.isEmpty()) {
        QMenu contextMenu{ QObject::tr("Context menu"), this };
        auto edit = new QAction{ QString{ "Edit" }, &contextMenu };
        QObject::connect(edit, &QAction::triggered, this, &MainWindow::editAlbumTrackSelection);
        contextMenu.addAction(edit);
        contextMenu.exec(this->ui->tracklistAlbumView->mapToGlobal(position));
    }
}

void MainWindow::showTrackDetailContextMenu(const QPoint& position)
{
    if (this->currentModel == nullptr) {
        return;
    }

    QMenu contextMenu{ QObject::tr("Context menu"), this };
    QModelIndexList indicies = this->ui->tracklistDetailsView->selectionModel()->selectedRows();

    if (indicies.isEmpty()) {
        return;
    }
    else if (indicies.length() == 1) {
        this->editDetailTrackSelection();
    }
    else {
        auto edit = new QAction{ QString{ "Edit" }, &contextMenu };
        QObject::connect(edit, &QAction::triggered, this, &MainWindow::editDetailTrackSelection);
        contextMenu.addAction(edit);
        contextMenu.exec(this->ui->tracklistDetailsView->mapToGlobal(position));
    }
}

void MainWindow::showSidebarContextMenu(const QPoint& position)
{
    QMenu contextMenu{ QObject::tr("Context menu"), this };

    MutableArray<NotNull<SidebarItem*>> deleteableItems;
    Optional<NotNull<SidebarItem*>> maybeFirstItem;

    auto indices = this->ui->sidebarView->indicesInSelection();
    for (auto&& index : indices) {
        if (!index.internalPointer()) {
            // -- Something is wrong here, maybe we have no sidebar items?
            continue;
        }

        NotNull<void*> internalPointer{ index.internalPointer() };
        auto selectedItem = internalPointer.forceAs<SidebarItem*>();
        if (selectedItem->canBeDeleted()) {
            deleteableItems.append(selectedItem);
        }

        maybeFirstItem = selectedItem;
    }

    if (deleteableItems.length() > 1) {
        QAction* action = new QAction{ QObject::tr("Delete playlists/folders"), &contextMenu };
        QObject::connect(action, &QAction::triggered, this, &MainWindow::deleteSelectedCrates);
        contextMenu.addAction(action);
    }
    else if (maybeFirstItem.isValid()) {
        (*maybeFirstItem)->populateContextMenu(contextMenu, this, false);
    }

    if (!contextMenu.isEmpty()) {
        contextMenu.exec(mapToGlobal(position));
    }
}

void MainWindow::renameSelectedCrate()
{
    QModelIndex index = this->ui->sidebarView->currentIndex();
    auto item = this->sidebarModel->itemFromIndex(index);
    auto selectedItem = dynamic_cast<AbstractCrateItem*>(item.get());
    NXA_ASSERT_TRUE(selectedItem->canBeRenamed());
    this->ui->sidebarView->edit(index);
}

void MainWindow::editSelectedCratePredicates()
{
    QModelIndex index = this->ui->sidebarView->currentIndex();
    auto selectedItem = static_cast<SidebarItem*>(index.internalPointer());
    auto selectedSmartPlaylist = dynamic_cast<MutablePlaylistItem*>(selectedItem);
    if (selectedSmartPlaylist == nullptr) {
        NXA_ALOG("Missing smart playlist");
        return;
    }
    auto predicates = selectedSmartPlaylist->predicates();

    if (!predicates.isValid()) {
        predicates = { std::make_unique<MutableGroupItem>(ChoiceData{ {}, {}, PredicateRule::ChildRules }, nothing) };
    }
    this->predicateDialog = std::make_unique<PredicateEditorDialog>(selectedSmartPlaylist, std::move(*predicates), this);
    connect(this->predicateDialog.get(), &PredicateEditorDialog::smartPlaylistUpdated, this, &MainWindow::selectedCrateShouldRefreshAndBeNamed);
    this->predicateDialog->show();
}

void MainWindow::selectedCrateShouldRefreshAndBeNamed(const QString& name)
{
    QModelIndex index = this->ui->sidebarView->currentIndex();
    this->sidebarModel->setData(index, name, Qt::EditRole);
    this->selectSidebarItem(index);
}

void MainWindow::cloneSelectedCrate()
{
    auto itemIndex = this->ui->sidebarView->currentIndex();
    if (!itemIndex.isValid()) {
        return;
    }

    auto& model = this->sidebarModel;
    auto parentIndex = model->parent(itemIndex);
    if (!parentIndex.isValid()) {
        return;
    }

    auto maybeCrateToClone = model->itemFromIndex(itemIndex).maybeAs<AbstractCrateItem*>();
    if (!maybeCrateToClone.isValid() || !(*maybeCrateToClone)->canBeCloned()) {
        return;
    }

    auto parentItem = model->itemFromIndex(parentIndex);
    count lastRow = parentItem->childCount();

    model->beginInsertRows(parentIndex, lastRow, lastRow);

    // -- This will currently always add the clone at the end of the crates.
    (*maybeCrateToClone)->cloneCrate();

    model->endInsertRows();

    model->notifyLayoutChangedAt(parentIndex);
}

void MainWindow::deleteSelectedCrates()
{
    auto& model = this->sidebarModel;
    auto asListOfIndices = this->ui->sidebarView->indicesInSelection();

    if (model->requestDeletionOfItemsAtIndices(asListOfIndices) &&
        model->displayedItemIsnItemsOrChildOfItemsAtIndices(asListOfIndices)) {
        this->clearSidebarSelection();
    }
}

void MainWindow::offerToRemoveCollectionAtIndex(const QModelIndex& index)
{
    auto asCollectionItem = this->sidebarModel->itemFromIndex(index).as<MutableViewCollection*>();
    if (asCollectionItem->offerToRemove() && this->sidebarModel->displayedItemIsnItemsOrChildOfItemsAtIndices(QModelIndexList{ index })) {
        this->clearSidebarSelection();
    }
}

void MainWindow::addSubfolderToSelectedCrate()
{
    auto parentItemIndex = this->ui->sidebarView->currentIndex();
    if (!parentItemIndex.isValid()) {
        return;
    }

    auto& model = this->sidebarModel;
    auto selectedItem = model->itemFromIndex(parentItemIndex);

    auto maybeParentCrate = selectedItem.maybeAs<AbstractCrateItem*>();
    if (!maybeParentCrate.isValid() ||  !(*maybeParentCrate)->canAddFolderChildren()) {
        return;
    }

    auto insertPos = selectedItem->childCount();
    model->beginInsertRows(parentItemIndex, insertPos, insertPos);

    // -- This will create the folder at the end of the parent folder.
    (*maybeParentCrate)->createChildFolder("New Folder"_String);

    model->endInsertRows();

    model->notifyLayoutChangedAt(parentItemIndex);

    this->ui->sidebarView->expand(parentItemIndex);
    this->ui->sidebarView->setCurrentIndex(this->sidebarModel->index(insertPos, 0, parentItemIndex));
}

void MainWindow::addSmartPlaylistToSelectedCrate()
{
    auto parentItemIndex = this->ui->sidebarView->currentIndex();
    if (!parentItemIndex.isValid()) {
        return;
    }

    auto& model = this->sidebarModel;
    auto selectedItem = model->itemFromIndex(parentItemIndex);

    auto maybeParentCrate = selectedItem.maybeAs<AbstractCrateItem*>();
    if (!maybeParentCrate.isValid() ||  !(*maybeParentCrate)->canAddFolderChildren()) {
        return;
    }

    auto insertPos = selectedItem->childCount();
    model->beginInsertRows(parentItemIndex, insertPos, insertPos);

    // -- This will create the smart playlist at the end of the parent folder.
    (*maybeParentCrate)->createChildSmartPlaylist("New Smart Playlist"_String);

    model->endInsertRows();

    model->notifyLayoutChangedAt(parentItemIndex);

    this->ui->sidebarView->expand(parentItemIndex);
    this->ui->sidebarView->setCurrentIndex(this->sidebarModel->index(insertPos, 0, parentItemIndex));
}

void MainWindow::addPlaylistToSelectedCrate()
{
    auto parentItemIndex = this->ui->sidebarView->currentIndex();
    if (!parentItemIndex.isValid()) {
        return;
    }

    auto& model = this->sidebarModel;
    auto selectedItem = model->itemFromIndex(parentItemIndex);

    auto maybeParentCrate = selectedItem.maybeAs<AbstractCrateItem*>();
    if (!maybeParentCrate.isValid() ||  !(*maybeParentCrate)->canAddFolderChildren()) {
        return;
    }

    auto insertPos = selectedItem->childCount();
    model->beginInsertRows(parentItemIndex, insertPos, insertPos);

    // -- This will create the playlist at the end of the parent folder.
    (*maybeParentCrate)->createChildPlaylist("New Playlist"_String);

    model->endInsertRows();

    model->notifyLayoutChangedAt(parentItemIndex);

    this->ui->sidebarView->expand(parentItemIndex);
    this->ui->sidebarView->setCurrentIndex(this->sidebarModel->index(insertPos, 0, parentItemIndex));
}

void MainWindow::handleOrganizeCrateCommand()
{
    auto sidebarView = this->ui->sidebarView;
    QModelIndex parentItemIndex = sidebarView->currentIndex();

    NotNull<SidebarItem*> selectedItem{ static_cast<SidebarItem*>(parentItemIndex.internalPointer()) };
    auto maybeCrateItem = selectedItem.maybeAs<MutablePlaylistItem*>();
    if (!maybeCrateItem.isValid()) {
        return;
    }

    auto& crateItem = *maybeCrateItem;
    QVariant userData = qobject_cast<QAction*>(sender())->data();
    if (userData.canConvert<Optional<Common::MutableTag*>>()) {
        crateItem->organizeBy(**userData.value<Optional<Common::MutableTag*>>());
    }
    else if (userData.canConvert<Optional<Common::Property::TypeID>>()) {
        crateItem->organizeBy(*userData.value<Optional<Common::Property::TypeID>>());
    }
    else {
        NXA_ALOG("Invalid item type attached to action.");
    }

    crateItem->getModel()->notifyLayoutChangedAt(parentItemIndex);

    sidebarView->expand(parentItemIndex);
    this->selectSidebarItem(parentItemIndex);
}

void MainWindow::showAboutDialog()
{
    this->aboutDialog->show();
}

void MainWindow::showPreferencesWindow()
{
    this->preferencesWindow->showAndCallOnClose([this]() {
        this->collectionList->notifyUserPreferencesHaveChanged();
    });
}

void MainWindow::showGettingStarted()
{
    QDesktopServices::openUrl(QUrl{ QString{ "https://forums.rekordbuddy.org/t/how-do-i-get-started-with-rekord-buddy/141" } });
}

void MainWindow::showOnlineManual()
{
    QDesktopServices::openUrl(QUrl{ QString{ "https://docs.rekordbuddy.org/v/2.2/" } });
}

void MainWindow::showOnlineSupport()
{
    QDesktopServices::openUrl(QUrl{ QString{ "https://forums.rekordbuddy.org/c/rekord-buddy/support" } });
}

void MainWindow::setViewModeArtwork()
{
    if (this->ui->tracklistViewsPanel->currentWidget() == this->ui->pageDetails) {
        // Pull current tracklist
        auto tracklist = this->currentModel->getTrackTree();
        // Set stacked widget to correct page
        this->ui->tracklistViewsPanel->setCurrentWidget(this->ui->pageAlbum);
        // Transfer tracklist
        std::promise<Optional<Shared<AbstractTrackListNode>>> tracklistPromise;
        tracklistPromise.set_value(tracklist);
        this->setActiveTrackList(tracklistPromise.get_future());
    }
}

void MainWindow::checkForUpdates()
{
    this->updater->userCheckForUpdates();
}

void MainWindow::didBeginWorking()
{
    if (this->isEditing()) {
        return;
    }

    QMetaObject::invokeMethod(this->menuBar(), "setEnabled", Qt::QueuedConnection, Q_ARG(bool, false));
    QMetaObject::invokeMethod(this, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, false));

    // -- We pause updates to the views in case tracks content changes during the work.
    this->ui->tracklistDetailsView->setUpdatesEnabled(false);
    this->ui->tracklistAlbumView->setUpdatesEnabled(false);

    this->ui->progressView->setHidden(false);
}

void MainWindow::didPauseWorking()
{
    if (this->isEditing()) {
        return;
    }

    QMetaObject::invokeMethod(this->menuBar(), "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
    QMetaObject::invokeMethod(this, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));

    // -- We restore updates to the views.
    this->ui->tracklistDetailsView->setUpdatesEnabled(true);
    this->ui->tracklistAlbumView->setUpdatesEnabled(true);

    this->ui->tracklistDetailsView->clearSelection();
    this->ui->tracklistAlbumView->clearSelection();

    this->ui->progressView->setHidden(true);
}

void MainWindow::setViewModeDetails()
{
    if (this->ui->tracklistViewsPanel->currentWidget() == this->ui->pageAlbum) {
        // Pull current tracklist
        auto tracklist = this->currentModel->getTrackTree();
        // Set stacked widget to correct page
        this->ui->tracklistViewsPanel->setCurrentWidget(this->ui->pageDetails);
        // Transfer tracklist
        std::promise<Optional<Shared<AbstractTrackListNode>>> tracklistPromise;
        tracklistPromise.set_value(tracklist);
        this->setActiveTrackList(tracklistPromise.get_future());
    }
}

void MainWindow::tracksChanged()
{
    MutableArray<NotNull<AbstractTrackItem*>> tracksThatHaveChanged;

    for (auto&& [trackIndex, track] : this->currentTracksEdited) {
        if (trackIndex.isValid()) {
            this->currentModel->dataChanged(trackIndex, trackIndex);
        }

        tracksThatHaveChanged.append(track);
    }

    this->sidebarModel->maybeUpdateBecauseTracksChanged(tracksThatHaveChanged);
}

void MainWindow::clickTrack(const QModelIndex& viewIndex)
{
    Optional<QUrl> maybeTrackUrl;

    QModelIndex index;
    if (dynamic_cast<TrackListDetailsModel*>(this->currentModel) != nullptr) {
        index = this->detailsSortProxy->mapToSource(viewIndex);
    }
    else {
        index = viewIndex;
    }

    if (index.isValid()) {
        auto maybeTrack = this->currentModel->maybeTrackAtIndex(index);
        if (maybeTrack.isValid()) {
            maybeTrackUrl = QUrl::fromLocalFile((*maybeTrack)->getTrackAbsoluteFilePath());
        }
    }

    this->ui->playerView->selectPlayableTrack(maybeTrackUrl);
}

void MainWindow::selectTrackItem(const QModelIndex& viewIndex)
{
    NXA_ASSERT_TRUE(this->currentTracksEdited.length() == 0);

    QModelIndex index;
    if (dynamic_cast<TrackListDetailsModel*>(this->currentModel) != nullptr) {
        index = this->detailsSortProxy->mapToSource(viewIndex);
    }
    else {
        index = viewIndex;
    }

    if (auto maybeTrack = this->currentModel->maybeTrackAtIndex(index); maybeTrack.isValid()) {
        auto& track = *maybeTrack;
        this->currentTracksEdited = MutableTrackEditorList{ std::make_tuple(index,
                                                                            NotNull<NxA::RekordBuddy::AbstractTrackItem*>{ track.get() } ) };

        TrackEditDialog trackEditDialog{ this->currentTracksEdited, track->canEdit(), &this->workQueue, this };
        connect(&trackEditDialog, &TrackEditDialog::tracksChanged, this, &MainWindow::tracksChanged);
        trackEditDialog.exec();

        this->currentTracksEdited.removeAll();
    }
}

void MainWindow::resetSidebarModel(const QModelIndex& index)
{
    this->sidebarModel->notifyLayoutChangedAt(index);
}

void MainWindow::expandSidebarItem(const QModelIndex& index)
{
    if (auto selectedItem = static_cast<SidebarItem*>(index.internalPointer()); selectedItem != nullptr) {
        if (selectedItem->needsLoading()) {
            selectedItem->loadChildrenIfUnloaded([this, index]() {
                QMetaObject::invokeMethod(this, "resetSidebarModel", Qt::QueuedConnection, Q_ARG(QModelIndex, index));
                return 0;
            });
        }
    }
}

void MainWindow::selectSidebarItem(const QModelIndex& index)
{
    if (auto selectedItem = static_cast<SidebarItem*>(index.internalPointer()); selectedItem != nullptr) {
        this->setActiveTrackList(selectedItem->maybeTrackTree());
        this->sidebarModel->setDisplayedIndex(index);
    }
    else {
        this->setActiveTrackList(std::async([]() {
            return Optional<Shared<AbstractTrackListNode>>{ };
        }));
        this->sidebarModel->setDisplayedIndex({ });
    }
}

void MainWindow::setupCollections()
{
    // -- This relies on the main thread being available in order to avoid deadlocks.
    NXA_ASSERT_FALSE_DEBUG(Threading::isOnMainThread());

    this->sidebarModel = std::make_unique<SidebarModel>(this->workQueue);
    connect(this->sidebarModel.get(), &SidebarModel::sidebarSelectionNeedsClearing, this, &MainWindow::clearSidebarSelection);

    this->collectionList = std::make_shared<MutableCollectionList>(this->sidebarModel.get(), this->workQueue);

    // -- This whole method is on the work queue; but we also open each collection on the queue.
    // -- hence the main thread can continue to pump messages.
    boolean goAgain = false;

    do {
        goAgain = false;

        for (auto&& error : collectionList->refreshCollectionsWithPerCollectionProgressCallBack([] { })) {
            String methodName;
            boolean collectionCannotBeReset = true;

            switch (error.errorType()) {
                case CollectionOpenError::Type::IgnoreCollection: {
                    // -- We do as we're told, we ignore this.
                    continue;
                }
                case CollectionOpenError::Type::InvalidLicence: {
                    // -- Invoke a main thread dialog to indicate licence error.
                    methodName = "showLicenseErrorDialog"_String;
                    break;
                }
                case CollectionOpenError::Type::ResetCollection: {
                    if (error.collectionType() != Common::Collection::Type::RekordBuddy) {
                        // -- We only know how to reset Rekord Buddy collections at the moment.
                        continue;
                    }

                    methodName = "showResetCollectionDialog"_String;

                    p_userChoiceForReset = false;
                    collectionCannotBeReset = false;

                    break;
                }
            }

            if (!methodName.length()) {
                continue;
            }

            QMetaObject::invokeMethod(this,
                                      methodName.asUTF8(),
                                      Qt::QueuedConnection,
                                      Q_ARG(CollectionOpenError, error));

            p_userAcknowledgedDialog.wait();

            if (error.errorType() == CollectionOpenError::Type::ResetCollection) {
                if (p_userChoiceForReset && !collectionCannotBeReset) {
                    deleteRekordBuddyCollectionOnVolume(error.volume());
                    goAgain = true;
                }
            }
        }

        if (goAgain) {
            // -- Abandon all previous work and try again
            collectionList = std::make_unique<MutableCollectionList>(this->sidebarModel.get(), workQueue);
        }
        else {
            QMetaObject::invokeMethod(this, "finishSettingUpCollections", Qt::QueuedConnection);
        }
    }
    while(goAgain);
}

void MainWindow::showLicenseErrorDialog(CollectionOpenError error)
{
    AlertMessageBox alert;

    boolean rekordBuddyCollectionOnHomeVolume = (error.collectionType() == Common::Collection::Type::RekordBuddy) &&
                                                (error.volume() == Volume::musicFolderVolume());
    auto dialogText = String::stringWithFormat("Valid license not found for the %s collection on volume '%s'.",
                                               error.collectionTypeName().asUTF8(),
                                               error.volume().name().asUTF8());
    alert.setText(QObject::tr(""), fromString(dialogText),
                  QObject::tr("License is either invalid or expired. If this is an external drive, you will need to plug it back "
                              "into the computer with your registered version of Rekord Buddy and launch the app to update the license."));
    alert.setButtonLayout(rekordBuddyCollectionOnHomeVolume ? QObject::tr("Exit") : QObject::tr("Ok"));
    alert.exec();

    if  (rekordBuddyCollectionOnHomeVolume) {
        exit(0);
    }
    else {
        p_userAcknowledgedDialog.notify();
    }
}

void MainWindow::showResetCollectionDialog(CollectionOpenError error)
{
    AlertMessageBox alert;

    boolean rekordBuddyCollectionOnHomeVolume = (error.collectionType() == Common::Collection::Type::RekordBuddy) &&
                                                (error.volume() == Volume::musicFolderVolume());
    auto dialogText = String::stringWithFormat("Error loading the %s collection on volume '%s'.",
                                               error.collectionTypeName().asUTF8(),
                                               error.volume().name().asUTF8());
    auto moreInfoText = "Database format for Rekord Buddy has changed. "
                        "By resetting the database, you will loose all your Rekord Buddy information.\n\n"
                        "If want to retain your old Rekord Buddy collection, do not use this build and wait for a future build allowing you to migrate your data.\n\n"
                        "More info at https://forums.rekordbuddy.org."_String;
    alert.setText(QObject::tr(""), fromString(dialogText), fromString(moreInfoText));
    alert.setButtonLayout(QObject::tr("Reset Database"),
                          rekordBuddyCollectionOnHomeVolume ? QObject::tr("Exit") : QObject::tr("Ignore It"));

    p_userChoiceForReset = (alert.exec() == QDialog::Accepted);

    if (!p_userChoiceForReset) {
        exit(0);
    }
    else {
        p_userAcknowledgedDialog.notify();
    }
}

void MainWindow::showCorruptedCollectionDialog(CollectionOpenError error)
{
    AlertMessageBox alert;

    auto dialogText = String::stringWithFormat(fromString(QObject::tr("Error loading the %s collection on volume '%s'.")),
                                               error.collectionTypeName().asUTF8(),
                                               error.volume().name().asUTF8());
    alert.setText(QObject::tr(""), fromString(dialogText),
                  QObject::tr("The collection could be corrupted but it doesn't appear to be yours so we can't let you reset it."));
    alert.setButtonLayout(QObject::tr("Ok"));

    p_userChoiceForReset = (alert.exec() == QDialog::Accepted);
    p_userAcknowledgedDialog.notify();
}

void MainWindow::finishSettingUpCollections()
{
    this->ui->sidebarView->setModel(this->sidebarModel.get());

    // -- Defer some time to the UI so it can set things up some more
    QTimer::singleShot(0, this, [this] {
        // -- Start with first collection expanded, with playlists also expanded within
        auto firstCollectionIndex = this->sidebarModel->index(0, 0, this->sidebarModel->rootModelIndex());
        this->ui->sidebarView->expand(firstCollectionIndex);

        auto firstCollectionAllTracksIndex = this->sidebarModel->index(0, 0, firstCollectionIndex);
        this->selectSidebarItem(firstCollectionAllTracksIndex);
    });
}

void MainWindow::setActiveTrackList(std::future<Optional<Shared<AbstractTrackListNode>>>&& newTrackTree)
{
    if (this->ui->tracklistViewsPanel->currentWidget() == this->ui->pageDetails) {
        this->currentModel = this->trackListDetailsModel.get();
    }
    else {
        this->currentModel = this->trackListAlbumModel.get();
    }

    // -- We will abandon whatever current task is in progress and swap in the new one, then start waiting for new value
    auto maybeTrackTree = newTrackTree.get();
    if (maybeTrackTree.isValid()) {
        auto& tree = *maybeTrackTree;

        // -- Set into the current visible widget
        if (this->ui->tracklistViewsPanel->currentWidget() == this->ui->pageDetails) {
            this->currentModel->setTrackTree(tree);

            auto maybeCrateBeingDisplayed = tree->maybeCrateItem();
            QTimer::singleShot(0, this, [this, maybeCrateBeingDisplayed] {
                auto trackListDetailsView = this->ui->tracklistDetailsView;

                trackListDetailsView->setCurrentlyDisplayedCrate(maybeCrateBeingDisplayed);
                trackListDetailsView->expandAll();

                // -- Set up the columns of details view now that the model is established on the view
                auto prefs = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());
                trackListDetailsView->header()->setSectionResizeMode(0, QHeaderView::Fixed);
                trackListDetailsView->header()->setStretchLastSection(false);

                for (integer column = 0; column < this->currentModel->columnCount(); ++column) {
                    auto columnID = trackListDetailsView->columnWidthIDAtIndex(column);
                    auto maybeWidth = prefs->maybeIntegerForKey(columnID);
                    if (maybeWidth.isValid() && (*maybeWidth != 0)) {
                        trackListDetailsView->setColumnWidth(*maybeNarrowCast<int>(column), *maybeWidth);
                    }
                    else {
                        trackListDetailsView->resizeColumnToContents(*maybeNarrowCast<int>(column));
                    }
                }
            });
        }
        else {
            this->currentModel->setTrackTree(tree);
            this->ui->tracklistAlbumView->expandAll();
        }
    }
    else {
        static Optional<Shared<AbstractTrackListNode>> emptyTrackTree;
        if(!emptyTrackTree.isValid()) {
            emptyTrackTree = { Shared<AbstractTrackListNode>(Shared<MutableTrackListNode>::with(nothing,
                                                                                                nothing,
                                                                                                [ ]() -> MutableTrackListNode::RefreshType
                                                                                                {
                                                                                                    return { };
                                                                                                })) };
        }

        this->currentModel->setTrackTree(*emptyTrackTree);
    }
}

void MainWindow::setupWindowThenCall(const std::function<void()>& callback)
{
    // -- This preforms tasks that require it to be on the main thread.
    NXA_ASSERT_TRUE_DEBUG(Threading::isOnMainThread());

    this->workQueue.enqueueJob(1, [this, callback](RunnableJob* runnableJob) {
        Semaphore progressDone;

        this->workQueue.enqueueJob(100, [&progressDone](RunnableJob* progressDisplayJob) {
            auto todaysDate = Date::inLocalTimeZoneFromTime(Time::currentTime());
            auto month = todaysDate.maybeMonth().valueOr(0);
            auto day = todaysDate.maybeDay().valueOr(0);
            if ((month == 12) && (day == 25)) {
                progressDisplayJob->updateStatus("Merry Christmas! Joyeux Noël! Feliz Navidad! メリークリスマス! Buon Natale!");
            }
            else if ((month == 1) && (day == 1)) {
                progressDisplayJob->updateStatus("Happy New Year! Bonne Année! Feliz Año Nuevo! 明けましておめでとう! Felice Anno Nuovo!");
            }
            else if ((month == 7) && (day == 23)) {
                progressDisplayJob->updateStatus("Happy Birthday Damien!");
            }
            else {
                static Array<String> startupMessages = {
                        "Readying the FLAC jacket..."_String,
                        "Go-go-gadgeto-streaming..."_String,
                        "Warming up the vinyl press..."_String,
                        "Trying to look busy..."_String,
                        "<Insert Funny Startup Message Here>"_String,
                        "Made \"By DJs for DJs\"..."_String,
                };
                progressDisplayJob->updateStatus(startupMessages[rand() % startupMessages.length()].asUTF8());
            }

            count progress = 0;

            do {
                progressDisplayJob->setProgress(progress++);
                Threading::sleepInMilliseconds(42);
            }
            while (progress <= 100);

            progressDone.notify();
        });

        auto musicFolderVolume = Volume::musicFolderVolume();
        // -- If this fails we don't care because we just care if the database exists for now.
        RekordBuddy::makeSureRekorBuddyCollectionExistsOnVolume(musicFolderVolume);

        progressDone.wait();

        auto expectedDatabasePath = RekordBuddy::rekordBuddyDatabasePathOnVolume(musicFolderVolume);
        if (!File::existsAt(expectedDatabasePath)) {
            Threading::runOnMainThread([] {
                ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Can't create a local Rekord Buddy database."),
                                                                 QObject::tr(""),
                                                                 QObject::tr(""));
                exit(1);
            }, Threading::Blocking::Yes);
        }

        this->setupCollections();

        runnableJob->addProgress(1);

        callback();
    });
}

void MainWindow::clearSidebarSelection()
{
    this->setActiveTrackList(std::async([]() {
        return Optional<Shared<AbstractTrackListNode>>{ };
    }));
    this->sidebarModel->setDisplayedIndex({ });
    this->ui->sidebarView->clearSelection();
}
