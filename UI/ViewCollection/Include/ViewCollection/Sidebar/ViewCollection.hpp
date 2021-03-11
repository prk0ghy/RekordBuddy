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
class AllTracksItem;
class ArtistsHeader;
class KeysHeader;
class TagsHeader;
class PlaylistsHeader;

// -- Public Interface
class ViewCollection : public AbstractCollection
{
    // -- Private Instance Variables
    Shared<Common::Collection> p_collection;

    Optional<Shared<AllTracksItem>> p_maybeAllTracks;
    Optional<Shared<ArtistsHeader>> p_maybeArtists;
    Optional<Shared<KeysHeader>> p_maybeKeys;
    Optional<Shared<TagsHeader>> p_maybeTags;
    Optional<Shared<PlaylistsHeader>> p_maybePlaylists;

    mutable Optional<QIcon> maybeCachedIcon;

protected:
    // -- Protected Instance Methods
    InternalItemReference p_internalReferenceForChildAtIndex(count) const override;
    Shared<SidebarItem> p_newChildForIndex(count) const override;

public:
    // -- Constructors & Destructors
    ViewCollection(Shared<Common::Collection>, NotNull<SidebarModel*>, SidebarItem*, WorkQueue&);

    // -- Instance Methods
    NotNull<const Common::Collection*> getCollection() const;
    NotNull<Common::Collection*> getCollection();

    count numberOfCratesAndTrackEntriesContainedWithin() const override;
    count numberOfTracks() const override;
    Optional<Volume> maybeParentCollectionVolume() const override;
    Optional<QString> maybeParentCollectionTypeName() const override;
    Optional<QString> maybeParentCollectionFullName() const override;
    boolean hasTracksNotOnVolume(const Volume&) const override;
    boolean hasMovieTracks() const override;

    QString getName() const override;
    Optional<QIcon> maybeIcon() const override;
    boolean needsLoading() const override;
    std::future<integer64> loadChildrenIfUnloaded(std::function<integer64()>&&) override;
    Optional<QString> getDescription() const override;
    count childCount() const override;
    boolean canBeDeleted() const override;
    void notifyUserPreferencesHaveChanged() override;
    boolean isOpened() const override;
    boolean hasChangesToSave() const override;
    boolean saveChangesWithProgress(std::function<void(double)>&&) override;

    boolean offerToRemove() override;

    void populateContextMenu(QMenu&, NotNull<MainWindow*>, boolean) override;
};

} }
