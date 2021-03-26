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

#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Crates/Crates.hpp>

using namespace NxA;
using namespace RekordBuddy;

// -- Instance Methods

void MutableFolder::p_moveSubCrateAtIndexToIndex(count fromIndex, count toIndex)
{
    this->p_ensureSubCratesAreLoaded().moveObjectAtIndexTo(fromIndex, toIndex);
    this->p_rekordBuddyFolder.moveSubCrateAtIndexTo(fromIndex, toIndex);
    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();
}

Pointer<const RekordBuddy::MutableCollection> MutableFolder::p_rekordBuddyCollection() const
{
    return this->p_rekordBuddyFolder.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

Pointer<RekordBuddy::MutableCollection> MutableFolder::p_rekordBuddyCollection()
{
    return this->p_rekordBuddyFolder.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
}

MutableArray<CollectionImplementation::Crate::MutableSubCrate>& MutableFolder::p_ensureSubCratesAreLoaded() const
{
    if (!this->p_maybeSubCrates.isValid()) {
        auto thisMutated = const_cast<MutableFolder*>(this);
        this->p_maybeSubCrates = thisMutated->p_rekordBuddyFolder.folderSubCrates(NotNull<MutableFolder *>{ thisMutated });
    }

    return *this->p_maybeSubCrates;
}

template <class T>
    void MutableFolder::p_moveSubCrateToIndex(NotNull<T*> rekordBuddyCrate, count index)
    {
        auto maybeOtherParentFolder = rekordBuddyCrate->p_maybeParentFolder;
        NXA_ASSERT_TRUE(maybeOtherParentFolder.isValid());

        auto rekordBuddyCratePointer = static_cast<void*>(rekordBuddyCrate.get());
        auto& otherParentFolder = *maybeOtherParentFolder;
        auto& otherSubCrates = otherParentFolder->p_ensureSubCratesAreLoaded();
        count otherSubCrateIndex = 0;

        for (auto&& otherSubCrate : otherSubCrates) {
            if (otherSubCrate.apply([](auto&& crate) -> void* { return crate.asRawPointer(); }) == rekordBuddyCratePointer) {
                break;
            }

            ++otherSubCrateIndex;
        }

        NXA_ASSERT_TRUE(otherSubCrateIndex < otherSubCrates.length());

        this->p_rekordBuddyFolder.moveCrateFromFolderAtIndexTo(otherParentFolder->p_rekordBuddyFolder, otherSubCrateIndex, index);
        this->p_ensureSubCratesAreLoaded().emplaceAppend(otherSubCrates.removeAndReturnObjectAtIndex(otherSubCrateIndex));
        this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
        this->collection()->markAsModifiedNow();

        rekordBuddyCrate->p_maybeParentFolder = { this };
    }

void MutableFolder::p_moveSubCrateToIndex(NotNull<Common::MutablePlaylist*> playlist, count index)
{
    auto maybeAsRekordBuddyPlaylist = playlist.maybeAs<MutablePlaylist*>();
    NXA_ASSERT_TRUE(maybeAsRekordBuddyPlaylist.isValid());

    this->p_moveSubCrateToIndex<MutablePlaylist>(*maybeAsRekordBuddyPlaylist, index);
}

void MutableFolder::p_moveSubCrateToIndex(NotNull<Common::MutableFolder*> folder, count index)
{
    auto maybeAsRekordBuddyFolder = folder.maybeAs<MutableFolder*>();
    NXA_ASSERT_TRUE(maybeAsRekordBuddyFolder.isValid());

    this->p_moveSubCrateToIndex<MutableFolder>(*maybeAsRekordBuddyFolder, index);
}

NotNull<const Common::Collection*> MutableFolder::collection() const
{
    return this->p_rekordBuddyCollection().asNotNull();
}

NotNull<Common::MutableCollection*> MutableFolder::collection()
{
    return this->p_rekordBuddyCollection().asNotNull();
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

NotNull<Common::MutableFolder*> MutableFolder::cloneWithName(const String& name)
{
    auto maybeParentFolder = this->p_maybeParentFolder;
    NXA_ASSERT_TRUE(maybeParentFolder.isValid());

    return (*maybeParentFolder)->newCloneOfFolderWithName(this, name);
}

Array<NotNull<const Common::Track*>> MutableFolder::tracks() const
{
    auto& collection = this->p_rekordBuddyCollection().asReference();

    MutableArray<NotNull<const Common::Track*>> results;

    for (auto&& trackObjectID : this->p_rekordBuddyFolder.uniqueAndUnorderedObjectIDs()) {
        results.append(collection.trackWithObjectID(trackObjectID).as<const Common::Track*>());
    }

    return std::move(results);
}

Array<NotNull<Common::MutableTrack*>> MutableFolder::tracks()
{
    auto& collection = this->p_rekordBuddyCollection().asReference();

    MutableArray<NotNull<Common::MutableTrack*>> results;

    for (auto&& trackObjectID : this->p_rekordBuddyFolder.uniqueAndUnorderedObjectIDs()) {
        results.append(collection.trackWithObjectID(trackObjectID));
    }

    return std::move(results);
}

Common::SubCrate MutableFolder::subCrateAtIndex(count index) const
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](const auto& crate) {
        return crate->p_asSubCrate();
    });
}

Common::MutableSubCrate MutableFolder::subCrateAtIndex(count index)
{
    return this->p_ensureSubCratesAreLoaded()[index].apply([](auto& crate) {
        return crate->p_asSubCrate();
    });
}

void MutableFolder::removeSubCrateAtIndex(count index)
{
    this->p_rekordBuddyFolder.removeSubCrateAtIndex(index);
    this->p_ensureSubCratesAreLoaded().removeObjectAtIndex(index);
    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();
}

NotNull<Common::MutablePlaylist*> MutableFolder::newPlaylistWithName(const String& name)
{
    auto newCrate = Unique<RekordBuddy::MutablePlaylist>::with(CollectionImplementation::Playlist::withNameAndParent(this->nextAvailableNameForPlaylistNamed(name),
                                                                                                                     this->p_rekordBuddyFolder),
                                                               NotNull<MutableFolder*>(this),
                                                               MutablePlaylist::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableFolder*> MutableFolder::newFolderWithName(const String& name)
{
    auto newCrate = Unique<RekordBuddy::MutableFolder>::with(CollectionImplementation::Folder::withNameAndParent(this->nextAvailableNameForFolderNamed(name),
                                                                                                                 this->p_rekordBuddyFolder),
                                                             NotNull<MutableFolder*>(this),
                                                             MutableFolder::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableSmartPlaylist*> MutableFolder::newSmartPlaylistWithName(const String& name)
{
    auto newCrate = Unique<RekordBuddy::MutableSmartPlaylist>::with(CollectionImplementation::SmartPlaylist::withNameAndParent(this->nextAvailableNameForPlaylistNamed(name),
                                                                                                                               this->p_rekordBuddyFolder),
                                                                    this,
                                                                    MutableSmartPlaylist::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutablePlaylist*> MutableFolder::newCloneOfPlaylistWithName(NotNull<RekordBuddy::MutablePlaylist*> playlist, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == playlist->collection());

    auto newCrate = Unique<RekordBuddy::MutablePlaylistClone>::with(
            CollectionImplementation::CrateClone::ofPlaylistWithNameAndParent(playlist->asRekordBuddyPlaylist(),
                                                                              this->nextAvailableNameForPlaylistNamed(name),
                                                                              this->asRekordBuddyFolder()),
            this,
            MutablePlaylistClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableFolder*> MutableFolder::newCloneOfFolderWithName(NotNull<RekordBuddy::MutableFolder*> folder, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == folder->collection());

    auto newCrate = Unique<RekordBuddy::MutableFolderClone>::with(
            CollectionImplementation::CrateClone::ofFolderWithNameAndParent(folder->asRekordBuddyFolder(),
                                                                            this->nextAvailableNameForFolderNamed(name),
                                                                            this->asRekordBuddyFolder()),
            this,
            MutableFolderClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableSmartPlaylist*> MutableFolder::newCloneOfSmartPlaylistWithName(NotNull<RekordBuddy::MutableSmartPlaylist*> playlist, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == playlist->collection());

    auto newCrate = Unique<RekordBuddy::MutableSmartPlaylistClone>::with(
            CollectionImplementation::CrateClone::ofSmartPlaylistWithNameAndParent(playlist->asRekordBuddySmartPlaylist(),
                                                                                   this->nextAvailableNameForPlaylistNamed(name),
                                                                                   this->asRekordBuddyFolder()),
            this,
            MutableSmartPlaylistClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutablePlaylist*> MutableFolder::newCloneOfPlaylistCloneWithName(NotNull<RekordBuddy::MutablePlaylistClone*> playlist, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == playlist->collection());

    auto newCrate = Unique<RekordBuddy::MutablePlaylistClone>::with(
            CollectionImplementation::CrateClone::ofPlaylistWithNameAndParent(playlist->cloneOf(),
                                                                              this->nextAvailableNameForPlaylistNamed(name),
                                                                              this->asRekordBuddyFolder()),
            this,
            MutablePlaylistClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableFolder*> MutableFolder::newCloneOfFolderCloneWithName(NotNull<RekordBuddy::MutableFolderClone*> folder, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == folder->collection());

    auto newCrate = Unique<RekordBuddy::MutableFolderClone>::with(
            CollectionImplementation::CrateClone::ofFolderWithNameAndParent(folder->cloneOf(),
                                                                            this->nextAvailableNameForFolderNamed(name),
                                                                            this->asRekordBuddyFolder()),
            this,
            MutableFolderClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}

NotNull<Common::MutableSmartPlaylist*> MutableFolder::newCloneOfSmartPlaylistCloneWithName(NotNull<RekordBuddy::MutableSmartPlaylistClone*> playlist, const String& name)
{
    NXA_ASSERT_TRUE(this->collection() == playlist->collection());

    auto newCrate = Unique<RekordBuddy::MutableSmartPlaylistClone>::with(
            CollectionImplementation::CrateClone::ofSmartPlaylistWithNameAndParent(playlist->cloneOf(),
                                                                                   this->nextAvailableNameForPlaylistNamed(name),
                                                                                   this->asRekordBuddyFolder()),
            this,
            MutableSmartPlaylistClone::p_isProtected);
    auto newCrateAsPointer = newCrate.asRawPointer();

    this->p_ensureSubCratesAreLoaded().emplaceAppend(std::move(newCrate));

    this->p_rekordBuddyFolder.markAsModifiedNow(CollectionImplementation::Crate::AndUpdateClones::Yes);
    this->collection()->markAsModifiedNow();

    return newCrateAsPointer;
}
