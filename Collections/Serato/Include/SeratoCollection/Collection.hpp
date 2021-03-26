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

#include <SeratoCollection/Markers/Markers.hpp>
#include <SeratoCollection/Crates/AllTracksPlaylist.hpp>

#include <SeratoCollectionImplementation/Database.hpp>

#include <CommonCollection/Tracks/Track.hpp>
#include <CommonCollection/Tracks/TrackPredicate.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>
#include <CommonCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/Playlist.hpp>
#include <CommonCollection/Artist.hpp>
#include <CommonCollection/Tracks/Tag.hpp>

#include <Base/TestUtility.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Serato {

// -- Public Interface
class MutableCollection : public Common::MutableCollection, public Common::Collection
{
    // -- Friends
    friend class CollectionImplementation::Database;

    // -- Private Instance Variables
    mutable CollectionImplementation::Database p_seratoDatabase;
    Time p_lastModificationTime;
    Time p_lastSavingTime;

    Map<FilePath, MutableArray<SeratoMarker::OfSomeSort>> p_markersPerFilePath;

    mutable Optional<Unique<MutableAllTracksPlaylist>> p_maybeAllTracksPlaylist;

    String p_artistsSeparator;
    String p_genresSeparator;
    String p_musicalKeysSeparator;

    boolean p_convertMemoryCuesToHotCuesOnExport;
    boolean p_ignorePlaylistsThatAreAlsoFoldersOnImport;

    Optional<Collection::Error> p_lastOpenResult;

    // -- Private Instance Methods
    String p_description() const override
    {
        return this->volume().name();
    }

protected:
    // -- Protected Instance Methods
    void p_addCratesFoundInSeratoFolderToRootFolderAndSaveSmartCrateNamesIn(const Directory& seratoFolderPath,
                                                                            MutableArray<String>& smartCrateNames);

    NotNull<MutableFolder*> p_rootFolder() const;
    NotNull<MutableAllTracksPlaylist*> p_tracks() const;

public:
    // -- Constructors & Destructors
    MutableCollection(CollectionImplementation::Database&& from) : p_seratoDatabase(std::move(from)) { }
    ~MutableCollection() override = default;

    // -- Instance Methods
    inline NXA_VIRTUAL_FOR_TESTING boolean convertMemoryCuesToHotCuesOnExport() const
    {
        return this->p_convertMemoryCuesToHotCuesOnExport;
    }
    inline boolean shouldBeOpenedLazily() const override
    {
        return true;
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
    Optional<Common::Collection::Error> open() override
    {
        this->notifyUserPreferencesHaveChanged();

        auto& seratoDatabase = this->p_seratoDatabase;
        if (!seratoDatabase.openInCollection(this)) {
            return this->p_lastOpenResult = Common::Collection::Error::CannotOpen;
        }

        auto rootFolderModificationDate = seratoDatabase.rootFolderModificationTime();
        this->p_lastSavingTime = seratoDatabase.modificationTime().laterDateBetweenThisAnd(rootFolderModificationDate);
        this->p_lastModificationTime = this->p_lastSavingTime;

        return this->p_lastOpenResult = nothing;
    }
    Optional<Collection::Error> lastOpenResult() const override
    {
        return this->p_lastOpenResult;
    }

    String name() const override
    {
        return "Serato"_String;
    }
    const character* iconName() const override
    {
        return "Preferences/Serato Logo/Serato 256";
    }
    Common::Collection::Type type() const override
    {
        return Common::Collection::Type::Serato;
    }
    Volume volume() const override
    {
        return this->p_seratoDatabase.volumePath();
    }
    Time lastModificationTime() const override
    {
        return this->p_lastModificationTime;
    }
    void markAsModifiedNow() override
    {
        this->p_lastModificationTime = Time::currentTime();
    }

    boolean isOpened() const override
    {
        return this->p_seratoDatabase.isOpened();
    }
    Optional<String> maybeOpeningErrorDescription() const override
    {
        return nothing;
    }
    boolean hasChangesToSave() const override
    {
        return this->p_lastModificationTime > this->p_lastSavingTime;
    }

    void reset() override
    {
        // -- TODO: Implement this if needed for serato.
    }
    void save() override
    {
        this->p_seratoDatabase.save();
        this->p_lastModificationTime = this->p_lastSavingTime =  Time::currentTime();
    }
    void saveWithProgress(std::function<void(double)>&& progressCallback) override
    {
        this->p_seratoDatabase.saveWithProgress(std::move(progressCallback));
        this->p_lastModificationTime = this->p_lastSavingTime =  Time::currentTime();
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

    NotNull<const Common::Folder*> rootFolder() const override;
    NotNull<Common::MutableFolder*> rootFolder() override;
    NotNull<Common::MutablePlaylist*> tracks() override;
    NotNull<const Common::Playlist*> tracks() const override;
    Array<Unique<Common::Artist>> artists() const override
    {
        return { };
    }
    Array<Unique<Common::MutableArtist>> artists() override
    {
        return { };
    }
    Array<Unique<Common::MusicalKey>> musicalKeys() const override
    {
        return { };
    }
    Array<Unique<Common::MutableMusicalKey>> musicalKeys() override
    {
        return { };
    }
    Array<Unique<Common::Tag>> tags() const override
    {
        return { };
    }
    Array<Unique<Common::MutableTag>> tags() override
    {
        return { };
    }
    Array<Common::Property::TypeID> propertyTypes() const override
    {
        return { };
    }
    
    NotNull<Common::MutableTrack*> trackWithRelativeFilePath(const FilePath&) override;
    Optional<NotNull<const Common::Track*>> maybeExistingTrackWithRelativeFilePath(const FilePath&) const override;
    Optional<NotNull<Common::MutableTrack*>> maybeExistingTrackWithRelativeFilePath(const FilePath&) override;

    Optional<NotNull<const Common::Track*>> maybeExistingTrackWithAbsoluteFilePath(const FilePath& absoluteFilePath) const override
    {
        return Common::Collection::p_maybeExistingTrackWithAbsoluteFilePathIn(absoluteFilePath, *this);
    }
    Optional<NotNull<Common::MutableTrack*>> maybeExistingTrackWithAbsoluteFilePath(const FilePath& absoluteFilePath) override
    {
        return this->Common::MutableCollection::maybeExistingTrackWithAbsoluteFilePath(absoluteFilePath);
    }

    void removeTrackAtIndex(count);
    void removeAllTracks();

    inline const CollectionImplementation::Database& seratoDatabase() const
    {
        return this->p_seratoDatabase;
    }
    inline CollectionImplementation::Database& seratoDatabase()
    {
        return this->p_seratoDatabase;
    }
};

} }
