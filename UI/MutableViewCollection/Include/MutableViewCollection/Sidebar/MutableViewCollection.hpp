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

#include <AbstractViewCollection/Sidebar/AbstractCollection.hpp>

#include <CommonCollection/Collection.hpp>

#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class WorkQueue;
class MutableAllTracksItem;
class MutableArtistsHeader;
class MutableKeysHeader;
class MutableTagsHeader;
class MutablePlaylistsHeader;

// -- Public Interface
class MutableViewCollection : public AbstractCollection
{
    // -- Private Instance Variables
    Shared<Common::MutableCollection> p_collection;

    Optional<Shared<MutableAllTracksItem>> p_maybeAllTracks;
    Optional<Shared<MutableArtistsHeader>> p_maybeArtists;
    Optional<Shared<MutableKeysHeader>> p_maybeKeys;
    Optional<Shared<MutableTagsHeader>> p_maybeTags;
    Optional<Shared<MutablePlaylistsHeader>> p_maybePlaylists;

    mutable Optional<QIcon> maybeCachedIcon;

protected:
    // -- Protected Instance Methods
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    // -- Constructors & Destructors
    MutableViewCollection(Shared<Common::MutableCollection>, NotNull<SidebarModel*>, SidebarItem*, WorkQueue&);

    // -- Instance Methods
    NotNull<const Common::MutableCollection*> getCollection() const;
    NotNull<Common::MutableCollection*> getCollection();

    count numberOfCratesAndTrackEntriesContainedWithin() const override;
    count numberOfTracks() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
    boolean parentCollectionMustHaveTracksOnTheSameVolume() const override;
    boolean hasTracksNotOnVolume(const Volume&) const override;
    boolean parentCollectionAllowsMovieTracksInPlaylists() const override;
    boolean hasMovieTracks() const override;

    void forceRefresh() override;

    QString getName() const override;
    Optional<QIcon> maybeIcon() const override;
    Optional<QString> getDescription() const override;
    count childCount() const override;
    boolean canBeDeleted() const override;
    boolean canReceiveCollection() const override;
    boolean canReceive(NotNull<AbstractCollection*>) const override;
    void receiveDraggedCollectionWithPerItemProgressCallBack(NotNull<AbstractCollection*>, std::function<void(void)>&&) override;
    boolean canReceiveFiles() const override;
    boolean receiveFileAt(const FilePath&) override;

    void notifyUserPreferencesHaveChanged() override;
    boolean isOpened() const override;
    boolean hasChangesToSave() const override;
    boolean needsLoading() const override;
    boolean saveChangesWithProgress(std::function<void(double)>&&) override;
    std::future<integer64> loadChildrenIfUnloaded(std::function<integer64()>&&) override;

    boolean offerToRemove() override;

    void populateContextMenu(QMenu&, NotNull<MainWindow*>, boolean) override;

    void maybeUpdateBecauseTracksChanged(const Array<NotNull<AbstractTrackItem*>>&) override;
};

} }
