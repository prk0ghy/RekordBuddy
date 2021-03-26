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

#include <SeratoCollectionImplementation/Crate.hpp>

#include <SeratoCollection/Tracks/Track.hpp>
#include <SeratoCollection/Crates/Playlist.hpp>

#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/SmartPlaylist.hpp>
#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Time.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace Serato {

// -- Forward Declaration
class MutableFolder;

// -- Public Interface
class MutableFolder : public Common::MutableFolder, public Common::Folder
{
    // -- Friends
    friend class RootFolder;
    friend class MutableRootFolder;
    friend class MutablePlaylist;
    friend class MutableCollection;
    friend class CollectionImplementation::Crate;
    friend class CollectionImplementation::Database;

    // -- Private Instance Variables
    Pointer<MutableCollection> p_collection;

    Optional<NotNull<MutableFolder*>> p_maybeParentFolder;

    CollectionImplementation::Crate p_seratoFolder;

    Time p_lastModificationTime;

    // -- Private Instance Methods
    NotNull<MutablePlaylist*> p_newPlaylistWithName(const String&);
    NotNull<MutableFolder*> p_newFolderWithName(const String&);

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    void p_moveSubCrateAtIndexToIndex(count fromIndex, count toIndex) override
    {
        if (fromIndex == toIndex) {
            return;
        }

        auto& maybeChildrenCrates = this->p_seratoFolder.p_maybeChildrenCrates;
        NXA_ASSERT_TRUE(maybeChildrenCrates.isValid());

        maybeChildrenCrates->moveObjectAtIndexTo(fromIndex, toIndex);

        this->p_markAsModifiedNow();
    }

    void p_removeSubCrateAtIndexAndMarkAsModified(count index, boolean markAsModified)
    {
        auto& maybeChildrenCrates = this->p_seratoFolder.p_maybeChildrenCrates;
        NXA_ASSERT_TRUE(maybeChildrenCrates.isValid());

        auto crate = (*maybeChildrenCrates)[index].apply([](auto& crate) {
            return crate->p_asSeratoCrate();
        });

        NXA_ASSERT_TRUE(crate->hasParentCrate() && (crate->p_parentCrate == this->p_asSeratoCrate()));
        crate->p_parentCrate = nothing;

        maybeChildrenCrates->removeObjectAtIndex(index);

        if (markAsModified) {
            this->p_markAsModifiedNow();
        }
    }

    void p_markAsModifiedNow();

    inline NotNull<const CollectionImplementation::Crate*> p_asSeratoCrate() const
    {
        return &this->p_seratoFolder;
    }
    inline NotNull<CollectionImplementation::Crate*> p_asSeratoCrate()
    {
        return &this->p_seratoFolder;
    }
    inline Common::SubCrate p_asSubCrate() const
    {
        return { NotNull<const Common::Folder*>{ this } };
    }
    inline Common::MutableSubCrate p_asSubCrate()
    {
        return { NotNull<Common::MutableFolder*>{ this } };
    }
    inline CollectionImplementation::Crate::MutableSubCrate p_asSeratoSubCrate()
    {
        return { NotNull<Serato::MutableFolder*>{ this } };
    }

    Optional<NotNull<MutableFolder*>> p_maybeSubFolderWithName(const String&);
    Optional<count> p_maybeIndexOfSubPlaylistWithName(const String&);
    void p_findOrAddCrateWithRelativeNameInSeratoFolder(const String&, const String&, const Directory&, boolean);

    String p_nextAvailableNameForSubCrateNamed(const String& name, boolean isAFolder) const override
    {
        count newEntryIndex = 1;

        while (1) {
            auto newEntryName = (newEntryIndex > 1) ? String::stringWithFormat("%s %llu", name.asUTF8(), newEntryIndex) : name;
            boolean alreadyThere = false;

            for (count crateIndex = 0; crateIndex < this->numberOfSubCrates(); ++crateIndex) {
                auto someTypeOfSubCrate = this->subCrateAtIndex(crateIndex);
                boolean foundAFolder = maybeGet<NotNull<const Common::Folder*>>(someTypeOfSubCrate).isValid();

                // -- We are ok with having playlists and folders with the same name since they end up merged together in Serato.
                if (isAFolder == foundAFolder) {
                    alreadyThere = someTypeOfSubCrate.apply([&newEntryName](auto&& subCrate) {
                        // -- We are ok with having playlists and folders with the same name since they get merged together in Serato.
                        return (subCrate->name() == newEntryName);
                    });
                }

                if (alreadyThere) {
                    break;
                }
            }

            if (!alreadyThere) {
                return newEntryName;
            }

            ++newEntryIndex;
        }
    }

    void p_moveSubCrateToIndex(NotNull<Common::MutablePlaylist*>, count) override;
    void p_moveSubCrateToIndex(NotNull<Common::MutableFolder*>, count) override;

public:
    // -- Constructors & Destructors
    MutableFolder(CollectionImplementation::Crate&& from,
                  Pointer<MutableCollection>inCollection,
                  Optional<NotNull<MutableFolder*>> maybeWithParentFolder,
                  const Protected&);
    ~MutableFolder() override = default;

    // -- Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    Optional<NotNull<const Common::Folder*>> maybeParentFolder() const override;
    Optional<NotNull<Common::MutableFolder*>> maybeParentFolder() override;

    Time lastModificationTime() const override
    {
        return this->p_lastModificationTime;
    }
    String name() const override
    {
        return this->p_maybeParentFolder.isValid() ? this->p_seratoFolder.name() : "<Root Folder>"_String;
    }
    void setName(const String& name) override
    {
        auto maybeParentFolder = this->p_maybeParentFolder;
        NXA_ASSERT_TRUE(maybeParentFolder.isValid());

        this->p_seratoFolder.setName((*maybeParentFolder)->nextAvailableNameForFolderNamed(name));
    }
    inline Common::CratePath path() const override
    {
        return Common::Folder::cratePathFor(*this);
    }

    Array<NotNull<const Common::Track*>> tracks() const override
    {
        return Folder::tracksIn<NotNull<const Common::Track*>>(*this);
    }
    Array<NotNull<Common::MutableTrack*>> tracks() override
    {
        return Folder::tracksIn<NotNull<Common::MutableTrack*>>(*this);
    }

    count numberOfSubCrates() const override
    {
        return this->p_seratoFolder.numberOfCrates();
    }
    Common::SubCrate subCrateAtIndex(count index) const override
    {
        return this->p_seratoFolder.crateAtIndex(index).apply([](auto& subCrate) -> Common::SubCrate {
            return { subCrate.get() };
        });
    }
    Common::MutableSubCrate subCrateAtIndex(count index) override
    {
        return this->p_seratoFolder.crateAtIndex(index).apply([](auto& subCrate) -> Common::MutableSubCrate {
            return { subCrate.get() };
        });
    }
    Optional<count> maybeIndexOfExistingSubCrateWithSameNameAs(SourceCrate subCrate) override
    {
        // -- Serato collections can have both a playlist and a folder with the same name, so we only look for crate of the same type.
        return this->p_maybeIndexOfExistingSubCrateWithSameNameAs(subCrate, FindExisting::OnlyIfTheSameType);
    }
    void removeSubCrateAtIndex(count index) override
    {
        this->p_removeSubCrateAtIndexAndMarkAsModified(index, true);
    }
    NotNull<Common::MutablePlaylist*> newPlaylistWithName(const String&) override;
    NotNull<Common::MutableFolder*> newFolderWithName(const String&) override;
};

} }
