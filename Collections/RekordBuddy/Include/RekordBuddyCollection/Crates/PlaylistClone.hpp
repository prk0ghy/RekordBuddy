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

#include <RekordBuddyCollection/Crates/Playlist.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class MutablePlaylistClone : public MutablePlaylist
{
    // -- Friends
    friend class CollectionImplementation::Folder;
    friend class CollectionImplementation::Crate;
    friend class MutablePlaylist;
    friend class MutableFolder;

    // -- Private Instance Variables
    CollectionImplementation::CrateClone p_rekordBuddyCrateClone;

    // -- Private Instance Methods
    MutableArray<CollectionImplementation::Crate::MutablePlaylistSubCrate>& p_ensureSubCratesAreLoaded() const override;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constructors & Destructors
    MutablePlaylistClone(CollectionImplementation::CrateClone&& from,
                         NotNull<MutableFolder*> withParentFolder,
                         const Protected&) : MutablePlaylist{ std::move(get<CollectionImplementation::Playlist>(from.cloneOf())),
                                                              withParentFolder,
                                                              MutablePlaylist::p_isProtected },
                                             p_rekordBuddyCrateClone{ std::move(from) } { }
    ~MutablePlaylistClone() override = default;

    // -- Instance Methods
    CollectionImplementation::Playlist& cloneOf()
    {
        return get<CollectionImplementation::Playlist>(this->p_rekordBuddyCrateClone.cloneOf());
    }

    // -- Overridden MutablePlaylist Instance Methods
    NotNull<Common::MutablePlaylist*> cloneWithName(const String&) override;

    void setName(const String& name) override
    {
        this->p_rekordBuddyCrateClone.setName(name);
    }

    void organizeBy(Common::MutableTag& tag) override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        this->p_rekordBuddyCrateClone.addSortingCriteriaForTrackTag(tag);
        this->p_maybeSubCrates = nothing;
    }
    void removeOrganizationBy(Common::MutableTag& tag) override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        auto rekordBuddyTag = static_cast<const MutableTag&>(tag);
        this->p_rekordBuddyCrateClone.removeAndDeleteSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
        this->p_maybeSubCrates = nothing;
    }

    void organizeBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddyCrateClone.addSortingCriteriaForTagTypeID(typeID);;
        this->p_maybeSubCrates = nothing;
    }
    void removeOrganizationBy(Common::Property::TypeID typeID) override
    {
        this->p_rekordBuddyCrateClone.removeAndDeleteSortingCriteriaForTagTypeID(typeID);
        this->p_maybeSubCrates = nothing;
    }

    // -- Overridden Playlist Instance Methods
    Time lastModificationTime() const override
    {
        return this->p_rekordBuddyCrateClone.lastModificationTime();
    }
    String name() const override
    {
        return this->p_rekordBuddyCrateClone.name();
    }

    boolean isOrganizedBy(const Common::Tag& tag) const override
    {
        NXA_ASSERT_TRUE(this->collection() == tag.collection());
        auto rekordBuddyTag = static_cast<const Tag&>(tag);
        return this->p_rekordBuddyCrateClone.hasSortingCriteriaForTrackTagWithDescription(rekordBuddyTag.propertyDescription());
    }
    boolean isOrganizedBy(Common::Property::TypeID typeID) const override
    {
        return this->p_rekordBuddyCrateClone.hasSortingCriteriaForTagTypeID(typeID);
    }
};

} }
