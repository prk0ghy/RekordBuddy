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

#pragma once

#include <UiMainWindow/MultiValueEditor.hpp>

#include <Base/NotNull.hpp>
#include <Base/Semaphore.hpp>

#include <QFutureWatcher>
#include <QMainWindow>

#include <future>

namespace Ui {

class MainWindow;

}

namespace NxA { namespace RekordBuddy {

class AboutDialog;
class PredicateEditorDialog;
class SidebarModel;
class SidebarItem;
class TrackListModel;
class WorkQueue;
class TrackEditDialog;
class TrackListAlbumModel;
class PreferencesWindow;
class AbstractCrateItem;
class AbstractCollectionListItem;
class AbstractCollection;
class AbstractTrackListNode;
class MutableCollectionList;
class TrackListDetailsModel;
class TrackListSortingModel;
class CollectionOpenError;
class Updater;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    // -- Private Instance Methods.
    void clickTrack(const QModelIndex&);
    void selectTrackItem(const QModelIndex&);
    void expandSidebarItem(const QModelIndex&);
    void resetSidebarModel(const QModelIndex&);
    void selectSidebarItem(const QModelIndex&);
    void showSidebarContextMenu(const QPoint&);
    void showTrackAlbumContextMenu(const QPoint&);
    void showTrackDetailContextMenu(const QPoint&);
    void showLicenseErrorDialog(CollectionOpenError);
    void showResetCollectionDialog(CollectionOpenError);
    void showCorruptedCollectionDialog(CollectionOpenError);
    void finishSettingUpCollections();
    void editDetailTrackSelection();
    void editAlbumTrackSelection();
    void showAboutDialog();
    void showPreferencesWindow();
    void showOfficialWebsite();
    void setViewModeArtwork();
    void setViewModeDetails();
    void checkForUpdates();
    void timeoutForUpdate();
    void saveCollections();
    void saveCollectionsThenCall(const std::function<void()>&);
    void selectedCrateShouldRefreshAndBeNamed(const QString&);
    void didBeginWorking();
    void didPauseWorking();
    boolean isEditing()
    {
        return this->currentTracksEdited.length() != 0;
    }

private:
    // -- Private Instance Variables
    WorkQueue& workQueue;
    std::unique_ptr<Updater> updater;

    std::unique_ptr<PredicateEditorDialog> predicateDialog;
    std::shared_ptr<AboutDialog> aboutDialog;
    std::unique_ptr<PreferencesWindow> preferencesWindow;
    std::unique_ptr<SidebarItem> previousItem;
    QTimer* periodicUpdater;

    MutableTrackEditorList currentTracksEdited;

    std::unique_ptr<SidebarModel> sidebarModel;
    NotNull<TrackListSortingModel*> detailsSortProxy;
    NotNull<TrackListDetailsModel*> trackListDetailsModel;
    NotNull<TrackListAlbumModel*> trackListAlbumModel;
    TrackListModel* currentModel{ nullptr };

    std::unique_ptr<Ui::MainWindow> ui;
    std::shared_ptr<MutableCollectionList> collectionList;

    // -- Private Instance Methods
    void setActiveTrackList(std::future<Optional<Shared<AbstractTrackListNode>>>&&);

protected:
    // -- Protected Instance Methods.
    void closeEvent(QCloseEvent*) override;
    void showEvent(QShowEvent*) override;

public:
    // -- Constructors & Destructors
    MainWindow(WorkQueue&, QWidget* parent = nullptr);
    ~MainWindow() override;

    // -- Instance Methods
    void editSelectedCratePredicates();
    void renameSelectedCrate();
    void cloneSelectedCrate();
    void deleteSelectedCrates();
    void offerToRemoveCollectionAtIndex(const QModelIndex&);
    void tracksChanged();
    void addSubfolderToSelectedCrate();
    void addPlaylistToSelectedCrate();
    void addSmartPlaylistToSelectedCrate();
    void handleOrganizeCrateCommand();
    void setupCollections();
    void loadWindowSettings();
    void saveWindowSettings();

public slots:
    void setupWindowThenCall(const std::function<void()>& callback);
    void clearSidebarSelection();
};

} }
