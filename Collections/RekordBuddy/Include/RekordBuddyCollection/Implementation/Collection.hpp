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

#include <RekordBuddyCollection/Implementation/Tags/Tag.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Common {

// -- Forward Declarations
class TrackPredicate;

}

namespace RekordBuddy {

// -- Forward Declarations
class MutableTrack;

namespace CollectionImplementation {

#define NXA_OBJECT_CLASS                            Collection
#include <Base/ObjectForwardDeclarations.ipp>

class Track;
class Artist;
class TrackDisplayCache;

// -- Public Interface
class Collection : protected NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

    friend class StringPropertyInternal;
    friend class DecimalPropertyInternal;
    friend class IntegerPropertyInternal;
    friend class TimePropertyInternal;

protected:

    // -- Protected Instance Methods
    MutableMap<Common::Property::TypeID, Shared<MutableMap<String, ObjectID>>>& p_stringPropertyTagsCache() const;
    MutableMap<Common::Property::TypeID, Shared<MutableMap<integer32, ObjectID>>>& p_integerPropertyTagsCache() const;
    MutableMap<Common::Property::TypeID, Shared<MutableMap<DecimalNumber, ObjectID>>>& p_decimalPropertyTagsCache() const;
    MutableMap<Common::Property::TypeID, Shared<MutableMap<Time, ObjectID>>>& p_timePropertyTagsCache() const;

public:
    // -- Class Methods
    inline static String className()
    {
        return "NxA::RekordBuddy::CollectionImplementation::Collection"_String;
    }

    // -- Constructors & Destructors
    explicit Collection(const Volume& onVolume);
    explicit Collection(const FilePath& usingFile);

    // -- Instance Methods
    Volume volume() const;
    FilePath filePath() const;

    Optional<Common::Collection::Error> open();
    boolean isOpened() const;
    boolean hasChangesToSave() const;
    void reset();
    void save();
    void saveWithProgress(std::function<void(double)>&&);

    Set<ObjectID> tracksMatchingPredicate(const Common::TrackPredicate&) const;
    Set<ObjectID> tracksMatchingKey(const String&) const;

    TrackDisplayCache trackDisplayCacheFor(const ObjectID&) const;

    NotNull<const RekordBuddy::MutableTrack*> trackWithObjectID(const ObjectID&) const;
    NotNull<RekordBuddy::MutableTrack*> trackWithObjectID(const ObjectID&);
    NotNull<RekordBuddy::MutableTrack*> trackWithRelativeFilePath(const FilePath&);
    Optional<NotNull<const RekordBuddy::MutableTrack*>> maybeExistingTrackWithRelativeFilePath(const FilePath&) const;
    Optional<NotNull<RekordBuddy::MutableTrack*>> maybeExistingTrackWithRelativeFilePath(const FilePath&);
    void removeTrackWithRelativeFilePath(const FilePath&);
    void removeAllTracks();
    void updateCachesForTrack(NotNull<RekordBuddy::MutableTrack*>);
    void deleteTrack(Track&);

    Artist artistNamed(const String&);
    Optional<Artist> maybeExistingArtistNamed(const String&);
    void renameArtistTo(Artist&, const String&);
    void deleteArtist(Artist&);

    void deleteAnyOrphanTags();

    // -- These are temporary and will be removed when the use of the persistent classes are fully encapsulated everywhere in the code.
    RootContext& persistentContext();
    Const<RootContext> persistentContext() const;
};

} } }
