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

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Tracks/Tag.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Crates/AllTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SmartPlaylist.hpp>

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

// -- Public Interface
class MutableSmartPlaylist : public Common::MutableSmartPlaylist, public Common::SmartPlaylist, public Common::Folder, public Common::Playlist
{
    // -- Friends
    friend class MutableSmartPlaylistClone;
    friend class CollectionImplementation::Folder;
    friend class CollectionImplementation::Crate;
    friend class MutableFolder;

    // -- Private Instance Variables
    CollectionImplementation::SmartPlaylist p_rekordBuddySmartPlaylist;

    NotNull<MutableFolder*> p_parentFolder;

    mutable Optional<MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>> p_maybeSubCrates;

    // -- Private Instance Methods
    inline Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const
    {
        return this->p_rekordBuddySmartPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }
    inline Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection()
    {
        return this->p_rekordBuddySmartPlaylist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }

    virtual MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& p_ensureSubCratesAreLoaded() const;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    inline Common::SubCrate p_asSubCrate() const
    {
        return { NotNull<const Common::SmartPlaylist*>{ this } };
    }
    inline Common::MutableSubCrate p_asSubCrate()
    {
        return { NotNull<Common::MutableSmartPlaylist*>{ this } };
    }

public:
    // -- Constructors & Destructors
    MutableSmartPlaylist(CollectionImplementation::SmartPlaylist&& from,
                         NotNull<MutableFolder*> withParentFolder,
                         const Protected&) : p_rekordBuddySmartPlaylist{ std::move(from) },
                                             p_parentFolder{ withParentFolder } { }
    ~MutableSmartPlaylist() override = default;

    // -- Instance Methods
    NotNull<const Common::Collection*> collection() const override
    {
        return this->p_rekordBuddyCollection().asNotNull();
    }
    NotNull<Common::MutableCollection*> collection() override
    {
        return this->p_rekordBuddyCollection().asNotNull();
    }

    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override
    {
        return this->p_parentFolder.as<const Common::Folder*>();
    }
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override
    {
        return this->p_parentFolder.as<Common::MutableFolder*>();
    }

    NotNull<Common::MutableSmartPlaylist*> cloneWithName(const String&) override;

    boolean canBeCloned() const override
    {
        return true;
    }

    Time lastModificationTime() const override
    {
        return this->p_rekordBuddySmartPlaylist.lastModificationTime();
    }
    boolean canBeRenamed() const override
    {
        return true;
    }
    String name() const override
    {
        return this->p_rekordBuddySmartPlaylist.name();
    }
    void setName(const String& name) override
    {
        this->p_rekordBuddySmartPlaylist.setName(name);
    }
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }
    const character* iconName() const override
    {
        return "Smart Playlist Icon/SmartPlaylist";
    }

    count numberOfSubCrates() const override
    {
        return this->p_ensureSubCratesAreLoaded().length();
    }
    Common::SubCrate subCrateAtIndex(count) const override;
    Common::MutableSubCrate subCrateAtIndex(count) override;

    count numberOfTracks() const override
    {
        return this->p_rekordBuddySmartPlaylist.numberOfTracks();
    }
    NotNull<const Common::Track*> trackAtIndex(count index) const override
    {
        return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddySmartPlaylist.trackObjectIDAtIndex(index));
    }
    NotNull<Common::MutableTrack*> trackAtIndex(count index) override
    {
        return (*this->p_rekordBuddyCollection()).trackWithObjectID(this->p_rekordBuddySmartPlaylist.trackObjectIDAtIndex(index));
    }
    Array<NotNull<const Common::Track*>> tracks() const override
    {
        return Common::Playlist::tracksIn<NotNull<const Common::Track*>>(*this);
    }
    Array<NotNull<Common::MutableTrack*>> tracks() override
    {
        return Common::Playlist::tracksIn<NotNull<Common::MutableTrack*>>(*this);
    }

    boolean canHavePredicates() const override
    {
        return true;
    }
    Optional<Common::TrackPredicate> maybePredicate() const override
    {
        return this->p_rekordBuddySmartPlaylist.maybePredicate();
    }
    void setPredicate(const Optional<Common::TrackPredicate>& predicate) override
    {
        this->p_rekordBuddySmartPlaylist.setPredicate(predicate);
    }

    boolean isOrganizedBy(const Common::Tag& tag) const override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        auto rekordBuddyTag = static_cast<const Tag&>(tag);
        return this->p_rekordBuddySmartPlaylist.hasSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
    }
    void organizeBy(Common::MutableTag& tag) override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        this->p_rekordBuddySmartPlaylist.addSortingCriteriaForTrackTag(tag);
        this->p_maybeSubCrates = nothing;
    }
    void removeOrganizationBy(Common::MutableTag& tag) override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        auto rekordBuddyTag = static_cast<const MutableTag&>(tag);
        this->p_rekordBuddySmartPlaylist.removeAndDeleteSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
        this->p_maybeSubCrates = nothing;
    }

    boolean isOrganizedBy(Common::Property::TypeID typeID) const override
    {
        return this->p_rekordBuddySmartPlaylist.hasSortingCriteriaForTagTypeID(typeID);
    }
    void organizeBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddySmartPlaylist.addSortingCriteriaForTagTypeID(typeID);;
        this->p_maybeSubCrates = nothing;
    }
    void removeOrganizationBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddySmartPlaylist.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
        this->p_maybeSubCrates = nothing;
    }

    CollectionImplementation::SmartPlaylist& asRekordBuddySmartPlaylist()
    {
        return this->p_rekordBuddySmartPlaylist;
    }
};

} }
