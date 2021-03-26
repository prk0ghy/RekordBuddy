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

#include <SeratoCollection/Crates/Folder.hpp>
#include <SeratoCollection/Collection.hpp>

using namespace NxA;
using namespace Serato;

// -- Constructors & Destructors

MutableFolder::MutableFolder(CollectionImplementation::Crate&& from,
                             Pointer<MutableCollection> inCollection,
                             Optional<NotNull<MutableFolder*>> maybeWithParentFolder,
                             const Protected&) : p_collection{ inCollection },
                                                 p_maybeParentFolder{ maybeWithParentFolder },
                                                 p_seratoFolder{ std::move(from) },
                                                 p_lastModificationTime{ this->p_seratoFolder.maybeLastModificationTimeInSeratoFolder((*inCollection).seratoDatabase().seratoFolderPath()).valueOr(Time::currentTime()) }
{

}

// -- Instance Methods

void MutableFolder::p_markAsModifiedNow()
{
    this->p_lastModificationTime = Time::currentTime();
    this->p_seratoFolder.p_markCrateAsModified();
    this->p_collection.asReference().markAsModifiedNow();
}

NotNull<MutablePlaylist*> MutableFolder::p_newPlaylistWithName(const String& name)
{
    auto newCrate = CollectionImplementation::Crate{ name, CollectionImplementation::Crate::p_isProtected };
    newCrate.p_parentCrate = NotNull<CollectionImplementation::Crate*>{ &this->p_seratoFolder };

    auto& childrenCrates = *this->p_seratoFolder.p_maybeChildrenCrates;

    auto newPlaylist = Unique<MutablePlaylist>::with(std::move(newCrate),
                                                     this->p_collection,
                                                     this,
                                                     MutablePlaylist::p_isProtected);
    auto newPlaylistPointer = newPlaylist.asRawPointer();
    childrenCrates.emplaceAppend(std::move(newPlaylist));

    return { newPlaylistPointer };
}

NotNull<MutableFolder*> MutableFolder::p_newFolderWithName(const String& name)
{
    auto newCrate = CollectionImplementation::Crate{ name, CollectionImplementation::Crate::p_isProtected };
    newCrate.p_parentCrate = NotNull<CollectionImplementation::Crate*>{ &this->p_seratoFolder };

    auto& childrenCrates = *this->p_seratoFolder.p_maybeChildrenCrates;

    auto newFolder = Unique<MutableFolder>::with(std::move(newCrate),
                                                 this->p_collection,
                                                 NotNull<NxA::Serato::MutableFolder *>{ this },
                                                 MutableFolder::p_isProtected);
    auto newFolderPointer = newFolder.asRawPointer();
    childrenCrates.emplaceAppend(std::move(newFolder));

    return { newFolderPointer };
}

Optional<NotNull<MutableFolder*>> MutableFolder::p_maybeSubFolderWithName(const String& name)
{
    for (count index = 0; index < this->p_seratoFolder.numberOfCrates(); ++index) {
        auto subCrate = this->p_seratoFolder.crateAtIndex(index);
        auto subCrateName = subCrate.apply([](auto& crate) {
            return crate->name();
        });

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
        NXA_DLOG_WITH_FORMAT("       found '%s'", subCrateName.asUTF8());
#endif
        if (subCrateName == name) {
            auto maybeCrateFoundAsAFolder = subCrate.maybeGet<NotNull<MutableFolder*>>();

            if (maybeCrateFoundAsAFolder.isValid()) {
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
                NXA_DLOG("     found it");
#endif
                return maybeCrateFoundAsAFolder;
            }
        }
    }

    return nothing;
}

Optional<count> MutableFolder::p_maybeIndexOfSubPlaylistWithName(const String& name)
{
    for (count index = 0; index < this->p_seratoFolder.numberOfCrates(); ++index) {
        auto subCrate = this->p_seratoFolder.crateAtIndex(index);
        auto subCrateName = subCrate.apply([](auto& crate) {
            return crate->name();
        });

        if (subCrateName == name) {
            if (subCrate.maybeGet<NotNull<MutablePlaylist*>>().isValid()) {
                return index;
            }
        }
    }

    return nothing;
}

void MutableFolder::p_findOrAddCrateWithRelativeNameInSeratoFolder(const String& fullCrateFileName,
                                                                   const String& relativeCratePath,
                                                                   const Directory& seratoFolderPath,
                                                                   boolean andRemovePlaylistsThatAreAlsoFolders)
{
    auto crateName = CollectionImplementation::Crate::p_topParentCrateNameFromFullCrateName(relativeCratePath);

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("     looking for crate '%s' as part of '%s'.", crateName.asUTF8(), relativeCratePath.asUTF8());
#endif

    if (crateName == relativeCratePath) {
        if (CollectionImplementation::Crate::isAnExistingFullCrateName(fullCrateFileName, seratoFolderPath)) {
            count index = this->p_seratoFolder.numberOfCrates();
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
            NXA_DLOG("     found it as a valid crate file we create it as a playlist");
#endif

            auto newPlaylist = this->p_newPlaylistWithName(crateName);
            newPlaylist->p_readPlaylistFromFolder(seratoFolderPath);

            if (newPlaylist->numberOfTracks() == 0) {
                this->p_removeSubCrateAtIndexAndMarkAsModified(index, false);
            }
        }
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
        else {
            NXA_DLOG_WITH_FORMAT("     could not find it, and '%s' doesn't exists as a valid crate file??", fullCrateFileName.asUTF8());
        }
#endif
    }
    else {
        // -- If the relative path is not a single crate name then we are parsing a folder.
        if (andRemovePlaylistsThatAreAlsoFolders) {
            auto maybeIndexOfPlaylistFound = this->p_maybeIndexOfSubPlaylistWithName(crateName);
            if (maybeIndexOfPlaylistFound.isValid()) {
                // -- If we have a playlist with the same name we delete it.
                this->p_removeSubCrateAtIndexAndMarkAsModified(*maybeIndexOfPlaylistFound, false);
            }
        }

        auto maybeFolderFound = this->p_maybeSubFolderWithName(crateName);
        if (!maybeFolderFound.isValid()) {
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
            NXA_DLOG("     could not find it, we create it as a folder");
#endif
            // -- But we didn't find a folder so we have to create it.
            maybeFolderFound = this->p_newFolderWithName(crateName);
        }

        (*maybeFolderFound)->p_findOrAddCrateWithRelativeNameInSeratoFolder(fullCrateFileName,
                                                                            relativeCratePath.subString(crateName.length() + 2),
                                                                            seratoFolderPath,
                                                                            andRemovePlaylistsThatAreAlsoFolders);
    }
}

void MutableFolder::p_moveSubCrateToIndex(NotNull<Common::MutablePlaylist*> playlist, count index)
{
    auto maybeAsSeratoPlaylist = playlist.maybeAs<MutablePlaylist*>();
    NXA_ASSERT_TRUE(maybeAsSeratoPlaylist.isValid());

    auto& seratoCrate = *maybeAsSeratoPlaylist;
    auto seratoCratePointer = static_cast<void*>(seratoCrate.get());
    auto otherParentFolder = seratoCrate->p_parentFolder;
    auto& otherSubCrates = *otherParentFolder->p_seratoFolder.p_maybeChildrenCrates;
    count otherSubCrateIndex = 0;

    for (auto&& otherSubCrate : otherSubCrates) {
        if (otherSubCrate.apply([](auto&& crate) -> void* { return crate.asRawPointer(); }) == seratoCratePointer) {
            break;
        }

        ++otherSubCrateIndex;
    }

    NXA_ASSERT_TRUE(otherSubCrateIndex < otherSubCrates.length());

    this->p_seratoFolder.p_maybeChildrenCrates->emplaceAppend(otherSubCrates.removeAndReturnObjectAtIndex(otherSubCrateIndex));

    seratoCrate->p_parentFolder = this;
    seratoCrate->p_seratoPlaylist.p_parentCrate = { &this->p_seratoFolder };

    otherParentFolder->p_markAsModifiedNow();
    this->p_markAsModifiedNow();
}

void MutableFolder::p_moveSubCrateToIndex(NotNull<Common::MutableFolder*> folder, count index)
{
    auto maybeAsSeratoFolder = folder.maybeAs<MutableFolder*>();
    NXA_ASSERT_TRUE(maybeAsSeratoFolder.isValid());

    auto& seratoCrate = *maybeAsSeratoFolder;
    auto seratoCratePointer = static_cast<void*>(seratoCrate.get());
    auto& maybeOtherParentFolder = seratoCrate->p_maybeParentFolder;
    NXA_ASSERT_TRUE(maybeOtherParentFolder.isValid());
    auto& otherParentFolder = *maybeOtherParentFolder;
    auto& otherSubCrates = *otherParentFolder->p_seratoFolder.p_maybeChildrenCrates;
    count otherSubCrateIndex = 0;

    for (auto&& otherSubCrate : otherSubCrates) {
        if (otherSubCrate.apply([](auto&& crate) -> void* { return crate.asRawPointer(); }) == seratoCratePointer) {
            break;
        }

        ++otherSubCrateIndex;
    }

    NXA_ASSERT_TRUE(otherSubCrateIndex < otherSubCrates.length());

    this->p_seratoFolder.p_maybeChildrenCrates->emplaceAppend(otherSubCrates.removeAndReturnObjectAtIndex(otherSubCrateIndex));

    seratoCrate->p_maybeParentFolder = { this };
    seratoCrate->p_seratoFolder.p_parentCrate = { &this->p_seratoFolder };

    otherParentFolder->p_markAsModifiedNow();
    this->p_markAsModifiedNow();    
}

// -- Instance Methods

NotNull<const Common::Collection*> MutableFolder::collection() const
{
    return this->p_collection.asNotNull();
}

NotNull<Common::MutableCollection*> MutableFolder::collection()
{
    return this->p_collection.asNotNull();
}

Optional<NotNull<const Common::Folder*>> MutableFolder::maybeParentFolder() const
{
    return this->p_maybeParentFolder.maybe([](auto folder) {
        return folder.template as<const Common::Folder*>();
    });
}

Optional<NotNull<Common::MutableFolder*>> MutableFolder::maybeParentFolder()
{
    return this->p_maybeParentFolder.maybe([](auto folder) {
        return folder.template as<Common::MutableFolder*>();
    });
}

NotNull<Common::MutablePlaylist*> MutableFolder::newPlaylistWithName(const String& name)
{
    this->p_markAsModifiedNow();

    return this->p_newPlaylistWithName(this->nextAvailableNameForPlaylistNamed(name)).as<Common::MutablePlaylist*>();
}

NotNull<Common::MutableFolder*> MutableFolder::newFolderWithName(const String& name)
{
    this->p_markAsModifiedNow();

    return this->p_newFolderWithName(this->nextAvailableNameForFolderNamed(name)).as<Common::MutableFolder*>();
}
