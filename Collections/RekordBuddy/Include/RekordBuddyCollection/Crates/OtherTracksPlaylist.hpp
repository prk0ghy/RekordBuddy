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

#include <RekordBuddyCollection/Implementation/Crates/OtherTracksPlaylist.hpp>
#include <RekordBuddyCollection/Crates/ParentPlaylist.hpp>

#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/SmartPlaylist.hpp>
#include <CommonCollection/Crates/CratePath.hpp>
#include <CommonCollection/Tracks/TrackPredicate.hpp>

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
class Tag;
class MutableTag;

// -- Public Interface
class MutableOtherTracksPlaylist : public Common::MutableSmartPlaylist, public Common::SmartPlaylist, public Common::Folder, public Common::Playlist
{
    // -- Friends
    friend class CollectionImplementation::Playlist;
    friend class CollectionImplementation::Crate;

    // -- Private Instance Variables
    CollectionImplementation::OtherTracksPlaylist p_rekordBuddyOtherTracksPlaylist;

    MutableParentPlaylist p_parentPlaylist;

    mutable Optional<MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>> p_maybeSubCrates;

    // -- Private Instance Methods
    inline Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const
    {
        return this->p_rekordBuddyOtherTracksPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }
    inline Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection()
    {
        return this->p_rekordBuddyOtherTracksPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }

    virtual MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& p_ensureSubCratesAreLoaded() const;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constructors & Destructors
    MutableOtherTracksPlaylist(CollectionImplementation::OtherTracksPlaylist&& from,
                               MutableParentPlaylist p_parentPlaylist,
                               const Protected&) : p_rekordBuddyOtherTracksPlaylist{ std::move(from) },
                                                   p_parentPlaylist{ p_parentPlaylist } { }
    ~MutableOtherTracksPlaylist() override = default;

    // -- Overridden Common::SortingPlaylist Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override;
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override;

    Time lastModificationTime() const override
    {
        return this->p_rekordBuddyOtherTracksPlaylist.lastModificationTime();
    }
    String name() const override
    {
        return this->p_rekordBuddyOtherTracksPlaylist.name();
    }
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }
    const character* iconName() const override
    {
        return "Filter Icon/39864";
    }

    count numberOfSubCrates() const override
    {
        return this->p_ensureSubCratesAreLoaded().length();
    }
    Common::SubCrate subCrateAtIndex(count) const override;
    Common::MutableSubCrate subCrateAtIndex(count) override;

    count numberOfTracks() const override
    {
        return this->p_rekordBuddyOtherTracksPlaylist.numberOfTracks();
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

    inline boolean canHavePredicates() const override
    {
        return false;
    }
    Optional<Common::TrackPredicate> maybePredicate() const override
    {
        NXA_ALOG("This cannot have predicates.");
    }
    void setPredicate(const Optional<Common::TrackPredicate>&) override
    {
        NXA_ALOG("This cannot have predicates.");
    }

    boolean isOrganizedBy(const Common::Tag& tag) const override;
    void organizeBy(Common::MutableTag& tag) override;
    void removeOrganizationBy(Common::MutableTag& tag) override;

    boolean isOrganizedBy(Common::Property::TypeID typeID) const override
    {
        return this->p_rekordBuddyOtherTracksPlaylist.hasSortingCriteriaForTagTypeID(typeID);
    }
    void organizeBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddyOtherTracksPlaylist.addSortingCriteriaForTagTypeID(typeID);;
        this->p_maybeSubCrates = nothing;
    }
    void removeOrganizationBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddyOtherTracksPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
        this->p_maybeSubCrates = nothing;
    }
};

} }
