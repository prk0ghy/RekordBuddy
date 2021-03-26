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
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackFileType.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackKey.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackModificationTime.hpp>

#include <CommonCollection/Tracks/Property.hpp>
#include <CommonCollection/Tracks/TrackRating.hpp>

#include <TrackFiles/TrackFile.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class MutableTrack;

namespace CollectionImplementation {

#include <Base/ResetObjectForwardDeclarations.ipp>

// -- Forward Declarations
#define NXA_OBJECT_CLASS                                        Track
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION
#include <Base/ObjectForwardDeclarations.ipp>

class Playlist;
class Collection;

// -- Public Interface
class Track : private NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

    friend class TrackDisplayCache;
    friend class Collection;

protected:
    // -- These methods are used by the track collection and should not be called directly by anything else.

    // -- Protected Factory Methods
    static Track p_createTrackWithRelativeFilePathInCollection(const FilePath&, Collection&);

    // -- Protected Instance Methods
    Blob p_displayCacheData() const;
    void p_deleteFromCollection();

public:
    // -- Constants
    enum class Error : byte {
        // -- These are stored in user data and cannot be re-ordered or modified.
        // -- They all need to be unique.
        TrackFileCouldNotBeUpdated,
    };

    enum class AndDeleteOrphans : boolean {
        No,
        Yes,
    };

    // -- Class Methods
    static void updateSmartContentForParentPlaylistsOfTracks(Array<NotNull<CollectionImplementation::Track*>>&);

    // -- Instance Methods
    ObjectID reference() const;

    FilePath relativeFilePath() const;
    void setRelativeFilePath(const FilePath&);

    Time lastModificationTime() const;
    void markAsModifiedNow();

    Optional<String> maybeTitle() const;
    void setTitle(const Optional<String>&);

    Optional<String> maybeAlbum() const;
    void setAlbum(const Optional<String>&);

    Optional<count> maybeAlbumTrackCount() const;
    void setAlbumTrackCount(const Optional<count>&);

    Optional<String> maybeComments() const;
    void setComments(const Optional<String>&);

    Array<String> genres() const;
    void setGenres(const Array<String>&);

    Optional<String> maybeGrouping() const;
    void setGrouping(const Optional<String>&);

    Optional<String> maybeMixName() const;
    void setMixName(const Optional<String>&);

    Optional<String> maybeRecordLabel() const;
    void setRecordLabel(const Optional<String>&);

    Array<String> tags() const;
    void setTags(const Array<String>&);

    Array<String> artists() const;
    void setArtists(const Array<String>&);

    Array<String> producers() const;
    void setProducers(const Array<String>&);

    Array<String> remixers() const;
    void setRemixers(const Array<String>&);

    Optional<Date> maybeDateAdded() const;
    void setDateAdded(const Optional<Date>&);

    Optional<Date> maybeDateReleased() const;
    void setDateReleased(const Optional<Date>&);

    Optional<boolean> maybeBeatGridLocked() const;
    void setBeatGridLocked(boolean);

    Optional<count> maybeBitDepthInBits() const;
    void setBitDepthInBits(const Optional<count>&);

    Optional<count> maybeBitRateInKiloBitsPerSecond() const;
    void setBitRateInKiloBitsPerSecond(const Optional<count>&);

    Optional<DecimalNumber> maybeBeatsPerMinute() const;
    void setBeatsPerMinute(const Optional<DecimalNumber>&);

    Optional<Color> maybeColor() const;
    void setColor(const Optional<Color>&);

    Optional<count> maybeFileSizeInBytes() const;
    void setFileSizeInBytes(const Optional<count>&);

    AudioFileType fileType() const;
    void setFileType(AudioFileType);

    Array<String> musicalKeys() const;
    void setMusicalKeys(const Array<String>&);

    Optional<DecimalNumber> maybeLengthInSeconds() const;
    void setLengthInSeconds(const Optional<DecimalNumber>&);

    Optional<count> maybeTrackNumber() const;
    void setTrackNumber(const Optional<count>&);

    Optional<count> maybeDiscNumber() const;
    void setDiscNumber(const Optional<count>&);

    Optional<count> maybePlayCount() const;
    void setPlayCount(const Optional<count>&);

    Optional<Common::TrackRating> maybeRating() const;
    void setRating(const Optional<Common::TrackRating>&);

    Optional<count> maybeSampleRateInHertz() const;
    void setSampleRateInHertz(const Optional<count>&);

    boolean hasOnlyOnePlaylistPointingToIt() const;
    boolean hasGridMarkers() const;

    Array<Playlist> parentPlaylists();

    Pointer<const Collection> collection() const;
    Pointer<Collection> collection();
    boolean isDeletedOrNotInAnyCollection() const;
    void deleteFromCollection();

    // -- These should be removed in the future as we're trying to hide tags in the track's implementation.
    template<typename T>
        boolean containsAtLeastOneTag() const;
    template<typename T>
        Array<Const<T>> tagsOfBaseTypeWithTypeID(Common::Property::TypeID) const;
    template<typename T>
        Array<T> tagsOfBaseTypeWithTypeID(Common::Property::TypeID);
    template<typename T>
        void addTagAndSetAsModifiedOn(T tag, const Time&, AndDeleteOrphans);

    std::shared_ptr<PersistentTrack> asPersistent()
    {
        return *this;
    }
};

} } }
