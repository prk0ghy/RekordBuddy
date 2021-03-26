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

#include <CommonCollection/Collection.hpp>
#include <CommonCollection/Tracks/Track.hpp>
#include <CommonCollection/Tracks/TrackPredicate.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>
#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/Playlist.hpp>

#include <RekordBuddyCollection/Crates/Folder.hpp>
#include <RekordBuddyCollection/Crates/Playlist.hpp>

#include <RekordBuddyCollection/Implementation/Collection.hpp>

#include <CommonCollection/Markers/MarkerOffset.hpp>

#include <Base/Array.hpp>
#include <Base/FilePath.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Time.hpp>

namespace NxA { namespace RekordBuddy {

#if defined(NXA_BUILD_FOR_TESTING)
class Test;
#endif

// -- Public Interface
class MutableCollection : public Common::MutableCollection, public Common::Collection
{
    // -- Friends
    friend class MutableFolder;
    friend class MutablePlaylist;

#if defined(NXA_BUILD_FOR_TESTING)
    friend class RekordBuddy::Test;
#endif

    // -- Private Instance Variables
    mutable CollectionImplementation::Collection p_rekordBuddyCollection;
    Time p_lastModificationTime;
    Time p_lastSavingTime;
    String p_collectionName = "Rekord Buddy"_String;
    boolean p_shouldBeOpenedLazily = false;

    mutable Optional<Unique<MutableFolder>> p_maybeRootFolder;
    mutable Optional<Unique<MutablePlaylist>> p_maybeAllTracksPlaylist;

    String p_artistsSeparator;
    String p_genresSeparator;
    String p_musicalKeysSeparator;

    Optional<Collection::Error> p_lastOpenResult;

    // -- Private Instance Methods
    String p_description() const override
    {
        return this->volume().name();
    }

    NotNull<RekordBuddy::MutableFolder*> p_rootFolder() const;
    NotNull<RekordBuddy::MutablePlaylist*> p_tracks() const;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

#if defined(NXA_BUILD_FOR_TESTING)
    boolean p_testIsAUnitTestCollection;
#endif

public:
    // -- Class Methods
    inline static String className()
    {
        return "NxA::RekordBuddy::MutableCollection"_String;
    }

    static Shared<MutableCollection> namedWithFileAt(const String&, const FilePath&);

    // -- Constructors & Destructors
    explicit MutableCollection(const Volume& onVolume) : p_rekordBuddyCollection{ onVolume } { }
    explicit MutableCollection(CollectionImplementation::Collection&& usingCollection, const Protected&) : p_rekordBuddyCollection{ std::move(usingCollection) } { }
    explicit MutableCollection(const FilePath& usingFile, const Protected&) : p_rekordBuddyCollection{ usingFile } { }
    ~MutableCollection() override { }

    // -- Instance Methods
    inline boolean shouldBeOpenedLazily() const override
    {
        return this->p_shouldBeOpenedLazily;
    }
    void setShouldBeOpenedLazily(boolean shouldBeOpenedLazily)
    {
        this->p_shouldBeOpenedLazily = shouldBeOpenedLazily;
    }
    inline boolean mustHaveTracksOnTheSameVolume() const override
    {
        return true;
    }
    inline boolean hasTracksNotOnVolume(const Volume& volume) const override
    {
        return this->volume() != volume;
    }
    inline boolean allowsMovieTracksInPlaylists() const override
    {
        return true;
    }
#if defined(NXA_BUILD_FOR_TESTING)
    inline boolean testIsAUnitTestCollection() const
    {
        return this->p_testIsAUnitTestCollection;
    }
#endif

    Optional<Collection::Error> open() override;
    Optional<Collection::Error> lastOpenResult() const override
    {
        return this->p_lastOpenResult;
    }
    String name() const override
    {
        return this->p_collectionName;
    }
    const character* iconName() const override
    {
        if (this->name() == "DJcity"_String) {
            return "Preferences/DJcity Logo/DJcity 256";
        }
        else {
            return "App Icon/Rekord Buddy Icon 256";
        }
    }
    void setName(const String& name)
    {
        this->p_collectionName = name;
    }
    Common::Collection::Type type() const override
    {
        return Common::Collection::Type::RekordBuddy;
    }

    Volume volume() const override
    {
        return this->p_rekordBuddyCollection.volume();
    }
    Time lastModificationTime() const override
    {
        return this->p_lastModificationTime;
    }

    boolean isOpened() const override
    {
        return this->p_rekordBuddyCollection.isOpened();
    }
    Optional<String> maybeOpeningErrorDescription() const override
    {
        return nothing;
    }
    boolean hasChangesToSave() const override
    {
        return this->p_lastModificationTime > this->p_lastSavingTime;
    }

    void markAsModifiedNow() override
    {
        this->p_lastModificationTime = Time::currentTime();
    }
    void reset() override
    {
        // -- TODO: Implement this if needed for Rekord Buddy.
    }
    void save() override
    {
        this->p_rekordBuddyCollection.save();
        this->p_lastSavingTime = this->p_lastModificationTime = Time::currentTime();
    }
    void saveWithProgress(std::function<void(double)>&& progress) override
    {
        this->p_rekordBuddyCollection.saveWithProgress(std::move(progress));
        this->p_lastSavingTime = this->p_lastModificationTime = Time::currentTime();
    }

    void notifyUserPreferencesHaveChanged() override;
    inline const String& artistsSeparator() const override
    {
        return this->p_artistsSeparator;
    }
    inline const String& genresSeparator() const override
    {
        return this->p_genresSeparator;
    }
    inline const String& musicalKeysSeparator() const override
    {
        return this->p_musicalKeysSeparator;
    }

    NotNull<const Common::Folder*> rootFolder() const override
    {
        return this->p_rootFolder();
    }
    NotNull<Common::MutableFolder*> rootFolder() override
    {
        return this->p_rootFolder();
    }
    NotNull<const Common::Playlist*> tracks() const override
    {
        return this->p_tracks();
    }
    NotNull<Common::MutablePlaylist*> tracks() override
    {
        return this->p_tracks();
    }
    Array<Unique<Common::Artist>> artists() const override;
    Array<Unique<Common::MutableArtist>> artists() override;
    Array<Unique<Common::MusicalKey>> musicalKeys() const override;
    Array<Unique<Common::MutableMusicalKey>> musicalKeys() override;
    Array<Unique<Common::Tag>> tags() const override;
    Array<Unique<Common::MutableTag>> tags() override;
    Array<Common::Property::TypeID> propertyTypes() const override;

    Optional<NotNull<const Common::Track*>> maybeExistingTrackWithRelativeFilePath(const FilePath&) const override;
    NotNull<Common::MutableTrack*> trackWithRelativeFilePath(const FilePath&) override;
    Optional<NotNull<Common::MutableTrack*>> maybeExistingTrackWithRelativeFilePath(const FilePath&) override;

    Optional<NotNull<const Common::Track*>> maybeExistingTrackWithAbsoluteFilePath(const FilePath& absoluteFilePath) const override
    {
        return Common::Collection::p_maybeExistingTrackWithAbsoluteFilePathIn(absoluteFilePath, *this);
    }
    Optional<NotNull<Common::MutableTrack*>> maybeExistingTrackWithAbsoluteFilePath(const FilePath& absoluteFilePath) override
    {
        return this->Common::MutableCollection::maybeExistingTrackWithAbsoluteFilePath(absoluteFilePath);
    }

    NotNull<const Common::Track*> trackWithObjectID(const CollectionImplementation::ObjectID&) const;
    NotNull<Common::MutableTrack*> trackWithObjectID(const CollectionImplementation::ObjectID&);

    inline Pointer<const CollectionImplementation::Collection> asRekordBuddyCollection() const
    {
        return { &this->p_rekordBuddyCollection };
    }
    inline Pointer<CollectionImplementation::Collection> asRekordBuddyCollection()
    {
        return { &this->p_rekordBuddyCollection };
    }
};

} }
