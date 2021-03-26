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

#include <SeratoCollection/Tracks/Track.hpp>

#include <SeratoCollectionImplementation/Crate.hpp>

#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Time.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations
class MutableFolder;

// -- Public Interface
class MutablePlaylist : public Common::MutablePlaylist, public Common::Playlist
{
    // -- Friends
    friend class MutableFolder;
    friend class MutableRootFolder;
    friend class CollectionImplementation::Crate;
    friend class CollectionImplementation::Database;

    // -- Private Instance Variables
    Pointer<MutableCollection> p_collection;

    NotNull<MutableFolder*> p_parentFolder;

    CollectionImplementation::Crate p_seratoPlaylist;

    Time p_lastModificationTime;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    Optional<count> p_addExistingTrackAtIndex(NotNull<Common::MutableTrack*> track, count index) override
    {
        auto maybeSeratoTrack = track.maybeAs<Serato::MutableTrack*>();
        NXA_ASSERT_TRUE(maybeSeratoTrack.isValid());
        NXA_ASSERT_TRUE(this->collection() == track->collection());

        return this->p_seratoPlaylist.addTrackAtIndex(*maybeSeratoTrack, index);
    }

    inline NotNull<const CollectionImplementation::Crate*> p_asSeratoCrate() const
    {
        return &this->p_seratoPlaylist;
    }
    inline NotNull<CollectionImplementation::Crate*> p_asSeratoCrate()
    {
        return &this->p_seratoPlaylist;
    }

    inline Common::SubCrate p_asSubCrate() const
    {
        return { NotNull<const Common::Playlist*>{ this } };
    }
    inline Common::MutableSubCrate p_asSubCrate()
    {
        return { NotNull<Common::MutablePlaylist*>{ this } };
    }

    inline CollectionImplementation::Crate::MutableSubCrate p_asSeratoSubCrate()
    {
        return { NotNull<Serato::MutablePlaylist*>{ this } };
    }

    void p_readPlaylistFromFolder(const Directory&);

    void p_markAsModifiedNow();

public:
    // -- Constructors & Destructors
    MutablePlaylist(CollectionImplementation::Crate&& from,
                    Pointer<MutableCollection> inCollection,
                    NotNull<MutableFolder*> withParentFolder,
                    const Protected&);
    ~MutablePlaylist() override = default;

    // -- Overridden Common::MutablePlaylist Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;
    
    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override;
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override;
    
    Time lastModificationTime() const override;
    String name() const override
    {
        return this->p_seratoPlaylist.name();
    }
    void setName(const String& name) override;
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }
    inline const character* iconName() const override
    {
        return "Playlist Icon/MusicPlaylist invert";
    }

    inline count numberOfSubCrates() const override
    {
        return 0;
    }
    Common::SubCrate subCrateAtIndex(count) const override
    {
        NXA_ALOG("This playlist doesn't have subcrates.");
    }
    Common::MutableSubCrate subCrateAtIndex(count) override
    {
        NXA_ALOG("This playlist doesn't have subcrates.");
    }

    count numberOfTracks() const override
    {
        return this->p_seratoPlaylist.numberOfTracks();
    }
    NotNull<const Common::Track*> trackAtIndex(count index) const override
    {
        return this->p_seratoPlaylist.trackAtIndex(index);
    }
    NotNull<Common::MutableTrack*> trackAtIndex(count index) override
    {
        return this->p_seratoPlaylist.trackAtIndex(index);
    }
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
    void moveTracksAtIndicesToIndex(Array<count> indices, count toIndex) override
    {
        this->p_seratoPlaylist.moveTracksAtIndicesToIndex(indices, toIndex);
    }
    void moveTrackAtIndexTo(count index, count toIndex) override
    {
        this->p_seratoPlaylist.moveTrackAtIndexTo(index, toIndex);
    }
    void removeTrackAtIndex(count) override;
    void removeAllTracks() override
    {
        for (count trackNumber = 0; trackNumber < this->numberOfTracks(); ++trackNumber) {
            this->p_seratoPlaylist.removeTrackAtIndex(0);
        }

        this->p_markAsModifiedNow();
    }

    inline boolean isOrganizedBy(const Common::Tag&) const override
    {
        return false;
    }
    void organizeBy(Common::MutableTag&) override
    {
        NXA_ALOG("This playlist can't be organized.");
    }
    void removeOrganizationBy(Common::MutableTag&) override
    {
        NXA_ALOG("This playlist can't be organized.");
    }

    inline boolean isOrganizedBy(Common::Property::TypeID) const override
    {
        return false;
    }
    void organizeBy(Common::Property::TypeID typeID) override
    {
        NXA_ALOG("This playlist can't be organized.");
    }
    void removeOrganizationBy(Common::Property::TypeID typeID) override
    {
        NXA_ALOG("This playlist can't be organized.");
    }
};

} }
