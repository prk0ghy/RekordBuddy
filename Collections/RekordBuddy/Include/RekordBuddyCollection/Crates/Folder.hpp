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

#include <RekordBuddyCollection/Implementation/Crates/Folder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/Playlist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/SmartPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/CrateClone.hpp>

#include <RekordBuddyCollection/Tracks/Track.hpp>

#include <CommonCollection/Crates/Folder.hpp>
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
class MutablePlaylist;
class MutableSmartPlaylist;
class MutablePlaylistClone;
class MutableSmartPlaylistClone;

// -- Public Interface
class MutableFolder : public Common::MutableFolder, public Common::Folder
{
    // -- Friends
    friend class CollectionImplementation::Folder;
    friend class CollectionImplementation::Crate;
    friend class MutableCollection;
    friend class MutableFolder;
    friend class MutableFolderClone;
    friend class RekordBuddy::MutablePlaylist;
    friend class RekordBuddy::MutableSmartPlaylist;
    friend class RekordBuddy::MutablePlaylistClone;
    friend class RekordBuddy::MutableSmartPlaylistClone;

    // -- Private Instance Variables
    CollectionImplementation::Folder p_rekordBuddyFolder;

    Optional<NotNull<MutableFolder*>> p_maybeParentFolder;

    mutable Optional<MutableArray<CollectionImplementation::Crate::MutableSubCrate>> p_maybeSubCrates;

    // -- Private Instance Methods
    void p_moveSubCrateAtIndexToIndex(count, count) override;

    Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const;
    Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection();

    virtual MutableArray<CollectionImplementation::Crate::MutableSubCrate>& p_ensureSubCratesAreLoaded() const;

    inline Common::SubCrate p_asSubCrate() const
    {
        return { NotNull<const Common::Folder*>{ this } };
    }
    inline Common::MutableSubCrate p_asSubCrate()
    {
        return { NotNull<Common::MutableFolder*>{ this } };
    }

    template <class T>
        void p_moveSubCrateToIndex(NotNull<T*>, count);

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    void p_moveSubCrateToIndex(NotNull<Common::MutablePlaylist*>, count) override;
    void p_moveSubCrateToIndex(NotNull<Common::MutableFolder*>, count) override;

public:
    // -- Constructors & Destructors
    MutableFolder(CollectionImplementation::Folder from,
                  Optional<NotNull<MutableFolder*>> maybeWithParentFolder,
                  const Protected&) : p_rekordBuddyFolder{ std::move(from) },
                                      p_maybeParentFolder{ maybeWithParentFolder } { }
    ~MutableFolder() override = default;

    // -- Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override;
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override;

    Time lastModificationTime() const override
    {
        return this->p_rekordBuddyFolder.lastModificationTime();
    }
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }

    boolean canBeCloned() const override
    {
        // -- This is turned off for now.
        return false;
    }
    NotNull<Common::MutableFolder*> cloneWithName(const String&) override;

    String name() const override
    {
        return this->p_rekordBuddyFolder.name();
    }
    void setName(const String& name) override
    {
        auto maybeParentFolder = this->p_maybeParentFolder;
        NXA_ASSERT_TRUE(maybeParentFolder.isValid());

        this->p_rekordBuddyFolder.setName((*maybeParentFolder)->nextAvailableNameForFolderNamed(name));
    }

    Array<NotNull<const Common::Track*>> tracks() const override;
    Array<NotNull<Common::MutableTrack*>> tracks() override;

    count numberOfSubCrates() const override
    {
        return this->p_ensureSubCratesAreLoaded().length();
    }
    Common::SubCrate subCrateAtIndex(count) const override;
    Common::MutableSubCrate subCrateAtIndex(count) override;
    void removeSubCrateAtIndex(count) override;

    NotNull<Common::MutablePlaylist*> newPlaylistWithName(const String&) override;
    NotNull<Common::MutableFolder*>  newFolderWithName(const String&) override;
    NotNull<Common::MutableSmartPlaylist*> newSmartPlaylistWithName(const String&) override;

    CollectionImplementation::Folder& asRekordBuddyFolder()
    {
        return this->p_rekordBuddyFolder;
    }

    NotNull<Common::MutablePlaylist*> newCloneOfPlaylistWithName(NotNull<RekordBuddy::MutablePlaylist*>, const String&);
    NotNull<Common::MutableFolder*> newCloneOfFolderWithName(NotNull<RekordBuddy::MutableFolder*>, const String&);
    NotNull<Common::MutableSmartPlaylist*> newCloneOfSmartPlaylistWithName(NotNull<RekordBuddy::MutableSmartPlaylist*>, const String&);
    NotNull<Common::MutablePlaylist*> newCloneOfPlaylistCloneWithName(NotNull<RekordBuddy::MutablePlaylistClone*>, const String&);
    NotNull<Common::MutableFolder*> newCloneOfFolderCloneWithName(NotNull<RekordBuddy::MutableFolderClone*>, const String&);
    NotNull<Common::MutableSmartPlaylist*> newCloneOfSmartPlaylistCloneWithName(NotNull<RekordBuddy::MutableSmartPlaylistClone*>, const String&);
};

} }
