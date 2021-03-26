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

#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Crates/ParentPlaylist.hpp>

#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace RekordBuddy {

class MutableFolder;
class MutablePlaylist;
class MutableSmartPlaylist;
class MutablePlaylistClone;
class MutableFolderClone;
class MutableSmartPlaylistClone;
class MutableOtherTracksPlaylist;
class MutableFilterPlaylist;
class MutableSortingPlaylist;

namespace CollectionImplementation {

#include <Base/ResetObjectForwardDeclarations.ipp>

// -- Forward Declarations
#define NXA_OBJECT_CLASS                                        Crate
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION
#include <Base/ObjectForwardDeclarations.ipp>

class Collection;
class Folder;
class Playlist;
class SmartPlaylist;
class AllTracksPlaylist;
class FilterPlaylist;
class OtherTracksPlaylist;
class SortingPlaylist;
class RootFolder;
class CrateClone;

// -- Public Interface
class Crate : protected NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

public:
    // -- Constants
    enum class AndUpdateClones : boolean {
        No,
        Yes,
    };

    // -- Types
    using MutableSubCrate = Variant<Unique<RekordBuddy::MutablePlaylist>, Unique<RekordBuddy::MutableFolder>,
                                    Unique<RekordBuddy::MutableSmartPlaylist>, Unique<RekordBuddy::MutablePlaylistClone>,
                                    Unique<RekordBuddy::MutableFolderClone>, Unique<RekordBuddy::MutableSmartPlaylistClone>>;
    using MutablePlaylistSubCrate = Variant<Unique<RekordBuddy::MutableOtherTracksPlaylist>, Unique<RekordBuddy::MutableFilterPlaylist>,
                                            Unique<RekordBuddy::MutableSortingPlaylist>>;

    // -- Operators
    friend bool operator<(const Crate&, const Crate&);

    // -- Instance Methods
    Time lastModificationTime() const;
    void markAsModifiedNow(AndUpdateClones = AndUpdateClones::Yes);

    count numberOfClones() const;

    String name() const;
    void setName(String name);

    Set<ObjectID> uniqueAndUnorderedObjectIDs() const;

    MutableArray<MutableSubCrate> folderSubCrates(Optional<NotNull<RekordBuddy::MutableFolder*>>);
    MutableArray<MutablePlaylistSubCrate> playlistSubCrates(RekordBuddy::MutableParentPlaylist);

    Pointer<const Collection> collection() const;
    Pointer<Collection> collection();
    boolean isDeletedOrNotInAnyCollection() const;
    void deleteFromCollection();
};

} } }
