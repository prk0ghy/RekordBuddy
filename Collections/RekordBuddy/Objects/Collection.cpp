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

#include <RekordBuddyCollection/RekordBuddy.hpp>
#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/Artist.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Tracks/MusicalKey.hpp>
#include <RekordBuddyCollection/Tracks/Tag.hpp>
#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <RekordBuddyCollection/Implementation/Artist.hpp>
#include <RekordBuddyCollection/Implementation/Crates/RootFolder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/AllTracksPlaylist.hpp>

#include <CommonCollection/Markers/MarkerOffset.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy;

// -- Class Methods

Shared<MutableCollection> MutableCollection::namedWithFileAt(const String& name, const FilePath& path)
{
    auto collection = Shared<MutableCollection>::with(path, MutableCollection::p_isProtected);
    collection->setName(name);
    return collection;
}

// -- Instance Methods

NotNull<RekordBuddy::MutableFolder*> MutableCollection::p_rootFolder() const
{
    if (!this->p_maybeRootFolder.isValid()) {
        NXA_ASSERT_TRUE(this->isOpened());

        auto maybeImplementationRootFolder = CollectionImplementation::RootFolder::maybeExistingInCollection(this->p_rekordBuddyCollection);
        NXA_ASSERT_TRUE(maybeImplementationRootFolder.isValid());
        this->p_maybeRootFolder = Unique<MutableFolder>::with(*maybeImplementationRootFolder, nothing, MutableFolder::p_isProtected);
    }

    return this->p_maybeRootFolder->asRawPointer();
}

NotNull<RekordBuddy::MutablePlaylist*> MutableCollection::p_tracks() const
{
    if (!this->p_maybeAllTracksPlaylist.isValid()) {
        NXA_ASSERT_TRUE(this->isOpened());

        auto maybeImplementationAllTracksPlaylist = CollectionImplementation::AllTracksPlaylist::maybeExistingInCollection(this->p_rekordBuddyCollection);
        NXA_ASSERT_TRUE(maybeImplementationAllTracksPlaylist.isValid());
        this->p_maybeAllTracksPlaylist = Unique<MutablePlaylist>::with(*maybeImplementationAllTracksPlaylist, nothing, MutablePlaylist::p_isProtected);
    }

    return this->p_maybeAllTracksPlaylist->asRawPointer();
}

Optional<Common::Collection::Error> MutableCollection::open()
{
    if (this->p_rekordBuddyCollection.isOpened()) {
        return nothing;
    }

    auto maybeError = this->p_rekordBuddyCollection.open();
    if (!maybeError.isValid()) {
        this->p_lastModificationTime = this->p_lastSavingTime = Time::currentTime();

        // -- We store our own shared wrapper to this collection in the user data, this makes it easy (and fast) to retrieve.
        this->p_rekordBuddyCollection.persistentContext()->setUserData(this);

        this->notifyUserPreferencesHaveChanged();
    }

    return this->p_lastOpenResult = maybeError;
}

void MutableCollection::notifyUserPreferencesHaveChanged()
{
    auto userPreferences = RekordBuddy::UserPreferences::forVolume(this->volume());

    this->p_artistsSeparator = *userPreferences->maybeStringForKey(String{ NXA_ARTISTS_SEPARATOR_PREFERENCES_KEY });
    this->p_genresSeparator = *userPreferences->maybeStringForKey(String{ NXA_GENRES_SEPARATOR_PREFERENCES_KEY });
    this->p_musicalKeysSeparator = *userPreferences->maybeStringForKey(String{ NXA_MUSICAL_KEYS_SEPARATOR_PREFERENCES_KEY });
}

Array<Unique<Common::MutableArtist>> MutableCollection::artists()
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::MutableArtist>> results;

    for (auto&& rekordBuddyArtist : CollectionImplementation::Artist::artistsInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<MutableArtist>::with(std::move(rekordBuddyArtist), Artist::p_isProtected));
    }

    return std::move(results);
}

Array<Unique<Common::Artist>> MutableCollection::artists() const
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::Artist>> results;

    for (auto&& rekordBuddyArtist : CollectionImplementation::Artist::artistsInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<Artist>::with(std::move(rekordBuddyArtist), Artist::p_isProtected));
    }

    return std::move(results);
}

Array<Unique<Common::MutableMusicalKey>> MutableCollection::musicalKeys()
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::MutableMusicalKey>> results;

    for (auto&& rekordBuddyMusicalKey : CollectionImplementation::TrackKey::musicalKeysInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<MutableMusicalKey>::with(std::move(rekordBuddyMusicalKey), MusicalKey::p_isProtected));
    }

    return std::move(results);
}

Array<Unique<Common::MusicalKey>> MutableCollection::musicalKeys() const
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::MusicalKey>> results;

    for (auto&& rekordBuddyMusicalKey : CollectionImplementation::TrackKey::musicalKeysInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<MusicalKey>::with(std::move(rekordBuddyMusicalKey), MusicalKey::p_isProtected));
    }

    return std::move(results);
}

Array<Unique<Common::MutableTag>> MutableCollection::tags()
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::MutableTag>> results;

    for (auto&& rekordBuddyTrackTag : CollectionImplementation::TrackTag::parentTrackTagsFoundInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<RekordBuddy::MutableTag>::with(std::move(rekordBuddyTrackTag), RekordBuddy::MutableTag::p_isProtected));
    }

    return std::move(results);
}

Array<Unique<Common::Tag>> MutableCollection::tags() const
{
    NXA_ASSERT_TRUE(this->isOpened());

    MutableArray<Unique<Common::Tag>> results;

    for (auto&& rekordBuddyTrackTag :
            CollectionImplementation::TrackTag::parentTrackTagsFoundInCollection(this->p_rekordBuddyCollection)) {
        results.append(Unique<RekordBuddy::Tag>::with(std::move(rekordBuddyTrackTag), RekordBuddy::Tag::p_isProtected));
    }

    return std::move(results);
}

Array<Common::Property::TypeID> MutableCollection::propertyTypes() const
{
    NXA_ASSERT_TRUE(this->isOpened());

    return CollectionImplementation::Tag::tagsTypesFoundInCollection(this->p_rekordBuddyCollection);
}

Optional<NotNull<const Common::Track*>> MutableCollection::maybeExistingTrackWithRelativeFilePath(const FilePath& path) const
{
    NXA_ASSERT_TRUE(this->isOpened());

    auto maybeTrack = this->p_rekordBuddyCollection.maybeExistingTrackWithRelativeFilePath(path);
    if (!maybeTrack.isValid()) {
        return nothing;
    }

    return maybeTrack->as<const Common::Track*>();
}

NotNull<Common::MutableTrack*> MutableCollection::trackWithRelativeFilePath(const FilePath& path)
{
    NXA_ASSERT_TRUE(this->p_rekordBuddyCollection.isOpened());

    this->markAsModifiedNow();

    return this->p_rekordBuddyCollection.trackWithRelativeFilePath(path);
}

Optional<NotNull<Common::MutableTrack*>> MutableCollection::maybeExistingTrackWithRelativeFilePath(const FilePath& path)
{
    NXA_ASSERT_TRUE(this->p_rekordBuddyCollection.isOpened());

    auto track = this->p_rekordBuddyCollection.maybeExistingTrackWithRelativeFilePath(path);
    if (!track.isValid()) {
        return nothing;
    }

    return track->as<Common::MutableTrack*>();
}

NotNull<const Common::Track*> MutableCollection::trackWithObjectID(const CollectionImplementation::ObjectID& objectID) const
{
    NXA_ASSERT_TRUE(this->p_rekordBuddyCollection.isOpened());

    return this->p_rekordBuddyCollection.trackWithObjectID(objectID);
}

NotNull<Common::MutableTrack*> MutableCollection::trackWithObjectID(const CollectionImplementation::ObjectID& objectID)
{
    NXA_ASSERT_TRUE(this->p_rekordBuddyCollection.isOpened());

    return this->p_rekordBuddyCollection.trackWithObjectID(objectID);
}
