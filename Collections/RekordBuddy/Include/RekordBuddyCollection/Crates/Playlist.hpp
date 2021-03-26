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

#include <RekordBuddyCollection/Implementation/Crates/Playlist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Folder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SmartPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/CrateClone.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SortingCriteria.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Tracks/Tag.hpp>

#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/SmartPlaylist.hpp>
#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Time.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class MutableCollection;

// -- Public Interface
class MutablePlaylist : public Common::MutablePlaylist, public Common::Playlist, public Common::Folder
{
    // -- Friends
    friend class MutableCollection;
    friend class MutableFolder;
    friend class RootFolder;
    friend class MutableRootFolder;
    friend class MutablePlaylistClone;
    friend class CollectionImplementation::Folder;
    friend class CollectionImplementation::Crate;

    // -- Private Instance Variables
    CollectionImplementation::Playlist p_rekordBuddyPlaylist;

    mutable Optional<MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>> p_maybeSubCrates;

    // -- Private Instance Methods
    Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const;
    Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection();

    virtual MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& p_ensureSubCratesAreLoaded() const;

    Optional<count> p_addExistingTrackAtIndex(NotNull<Common::MutableTrack*>, count) override;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Variables
    Optional<NotNull<MutableFolder*>> p_maybeParentFolder;

    // -- Protected Instance Methods
    inline Common::SubCrate p_asSubCrate() const
    {
        return { NotNull<const Common::Playlist*>{ this } };
    }
    inline Common::MutableSubCrate p_asSubCrate()
    {
        return { NotNull<Common::MutablePlaylist*>{ this } };
    }

public:
    // -- Constructors & Destructors
    MutablePlaylist(CollectionImplementation::Playlist from,
                    Optional<NotNull<MutableFolder*>> maybeWithParentFolder,
                    const Protected&) : p_rekordBuddyPlaylist{ std::move(from) },
                                        p_maybeParentFolder{ maybeWithParentFolder } { }
    ~MutablePlaylist() override = default;

    // -- Instance Methods
    CollectionImplementation::Playlist& asRekordBuddyPlaylist()
    {
        return this->p_rekordBuddyPlaylist;
    }

    // -- Overridden Common::MutablePlaylist Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override;
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override;

    boolean canBeCloned() const override
    {
        return true;
    }
    NotNull<Common::MutablePlaylist*> cloneWithName(const String&) override;

    Time lastModificationTime() const override
    {
        return this->p_rekordBuddyPlaylist.lastModificationTime();
    }
    String name() const override
    {
        return this->p_rekordBuddyPlaylist.name();
    }
    void setName(const String& name) override
    {
        this->p_rekordBuddyPlaylist.setName(name);
    }
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }
    inline const character* iconName() const override
    {
        return "Playlist Icon/MusicPlaylist invert";
    }

    count numberOfSubCrates() const override
    {
        return this->p_ensureSubCratesAreLoaded().length();
    }
    Common::SubCrate subCrateAtIndex(count) const override;
    Common::MutableSubCrate subCrateAtIndex(count) override;

    count numberOfTracks() const override
    {
        return this->p_rekordBuddyPlaylist.numberOfTracks();
    }
    NotNull<const Common::Track*> trackAtIndex(count) const override;
    NotNull<Common::MutableTrack*> trackAtIndex(count) override;
    Array<NotNull<const Common::Track*>> tracks() const override
    {
        return Common::Playlist::tracksIn<NotNull<const Common::Track*>>(*this);
    }
    Array<NotNull<Common::MutableTrack*>> tracks() override
    {
        return Common::Playlist::tracksIn<NotNull<Common::MutableTrack*>>(*this);
    }
    inline boolean canReceive(NotNull<const Common::Track*> track) const override
    {
        return this->collection()->volume() == track->volume();
    }
    inline boolean canReceive(NotNull<Common::MutableTrack*> track) const override
    {
        return this->collection()->volume() == track->volume();
    }
    void moveTracksAtIndicesToIndex(Array<count>, count) override;
    void moveTrackAtIndexTo(count, count) override;
    void removeTrackAtIndex(count) override;
    void removeAllTracks() override;

    boolean isOrganizedBy(const Common::Tag& tag) const override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        auto rekordBuddyTag = static_cast<const Tag&>(tag);
        return this->p_rekordBuddyPlaylist.hasSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
    }
    void organizeBy(Common::MutableTag& tag) override;
    void removeOrganizationBy(Common::MutableTag& tag) override;

    boolean isOrganizedBy(Common::Property::TypeID typeID) const override
    {
        return this->p_rekordBuddyPlaylist.hasSortingCriteriaForTagTypeID(typeID);
    }
    void organizeBy(Common::Property::TypeID) override;
    void removeOrganizationBy(Common::Property::TypeID) override;
};

} }
