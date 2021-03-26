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

#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/DebugFlags.hpp>

#include <SeratoCollection/Markers/Markers.hpp>

#include <CommonCollection/Markers/Markers.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace Serato {

// -- Forward Declarations
class MutableCollection;

namespace CollectionImplementation {

// -- Forward Declarations
class Tag;
class Database;

#if defined(NXA_BUILD_FOR_TESTING)
class CrateTests;
#endif

// -- Public Interface
class Track
{
    // -- Friends
    friend class Serato::MutableCollection;
    friend class Database;
    
#if defined(NXA_BUILD_FOR_TESTING)
    friend class CrateTests;
    friend class TrackTests;
#endif

    // -- Private Instance Variables
    Pointer<Serato::MutableCollection> p_collection;

    ObjectTag p_trackTag;

    mutable Optional<MutableArray<Shared<Serato::MutableMarkerOfSomeSort>>> p_maybeMarkers;

    boolean p_needsToUpdateDatabaseFile = false;
    boolean p_needsToUpdateTrackFile = false;

    // -- Private Instance Methods
    Shared<Serato::MutableMarkerOfSomeSort> p_mutableMarkerWith(SeratoCueMarker&&) const;
    Shared<Serato::MutableMarkerOfSomeSort> p_mutableMarkerWith(SeratoGridMarker&&) const;
    Shared<Serato::MutableMarkerOfSomeSort> p_mutableMarkerWith(SeratoLoopMarker&&) const;

    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::CueMarker*>& marker,
                                                               Optional<DecimalNumber> maybeOffset)
    {
        this->appendCueMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }
    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::GridMarker*>& marker,
                                                               Optional<DecimalNumber> maybeOffset)
    {
        this->appendGridMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }
    inline void p_appendCommonMarkerAndMaybeAddOffsetInSeconds(const NotNull<const Common::LoopMarker*>& marker,
                                                               Optional<DecimalNumber> maybeOffset)
    {
        this->appendLoopMarker()->setWithSamePropertiesAsAndMaybeAddOffsetInSeconds(*marker, maybeOffset);
    }

    MutableArray<Shared<Serato::MutableMarkerOfSomeSort>>& p_ensureMarkersAreLoaded() const;

    Optional<String> p_maybeStringForSubTagForIdentifier(uinteger32) const;
    Optional<FilePath> p_maybePathForSubTagForIdentifier(uinteger32) const;
    Optional<uinteger32> p_maybeUint32ForSubTagForIdentifier(uinteger32) const;
    Optional<boolean> p_maybeBooleanForSubTagForIdentifier(uinteger32) const;

    void p_setStringForSubTagForIdentifier(const String&, uinteger32);
    void p_setPathForSubTagForIdentifier(const FilePath&, uinteger32);
    void p_setUInt32ForSubTagForIdentifier(uinteger32, uinteger32);
    void p_setBooleanForSubTagForIdentifier(boolean, uinteger32);

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Class Methods
#if defined(NXA_PRINT_SERATO_TRACK_DEBUG_INFO)
    static void debugPrintString(const String&, const String&);
    static void debugPrintUint(uinteger32, const String&);
    static void debugPrintTimeFromMilliseconds(uinteger32, const String&);
    static void debugPrintDate(timestamp, const String&);
    static void debugPrint(const Track&);
#endif

    // -- Constructors & Destructors
    Track() = delete;
    Track(ObjectTag fromTag, Pointer<Serato::MutableCollection> inCollection, const Protected&) : p_collection{ inCollection },
                                                                                                  p_trackTag{ fromTag } { }
    Track(const FilePath& withRelativeFilePath, Pointer<Serato::MutableCollection> inCollection, const Protected&);

    // -- Operators
    bool operator==(const Track& other) const noexcept;

    // -- Instance Methods
    Pointer<const Serato::MutableCollection> collection() const;
    Pointer<Serato::MutableCollection> collection()
    {
        return this->p_collection;
    }

    FilePath filePathRelativeToDatabaseVolume() const;

    String title() const;
    void setTitle(const String&);

    String artist() const;
    void setArtist(const String&);

    String album() const;
    void setAlbum(const String&);

    String genre() const;
    void setGenre(const String&);

    String comments() const;
    void setComments(const String&);

    String grouping() const;
    void setGrouping(const String&);

    String remixer() const;
    void setRemixer(const String&);

    String recordLabel() const;
    void setRecordLabel(const String&);

    String composer() const;
    void setComposer(const String&);

    String key() const;
    void setKey(const String&);

    String bpm() const;
    void setBpm(const String&);

    String year() const;
    void setYear(const String&);

    count trackNumber() const;
    void setTrackNumber(count);

    count discNumber() const;
    void setDiscNumber(count);

    count playCount() const;
    void setPlayCount(count);

    Color color() const;
    void setColor(const Color&);

    timestamp timeLastModifiedInSecondsSinceJanuary1st1970() const;
    void setTimeLastModifiedInSecondsSinceJanuary1st1970(timestamp);

    timestamp dateAddedInSecondsSinceJanuary1st1970() const;
    void setDateAddedInSecondsSinceJanuary1st1970(timestamp);

    boolean beatGridIsLocked() const;
    void setBeatGridIsLocked(boolean);

    count sizeInBytes() const;
    void setSizeInBytes(count);

    uinteger32 lengthInMilliSeconds() const;
    void setLengthInMilliSeconds(uinteger32);

    uinteger32 bitRateInKiloBitsPerSecond() const;
    void setBitRateInKiloBitsPerSecond(uinteger32);

    uinteger32 sampleRateInSamplesPerSecond() const;
    void setSampleRateInSamplesPerSecond(uinteger32);

    count numberOfMarkers() const
    {
        return this->p_ensureMarkersAreLoaded().length();
    }
    NotNull<Common::MutableCueMarker *> appendCueMarker()
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<Serato::MutableMarkerOfSomeSort>::with(MutableCueMarker{ SeratoCueMarker{ },
                                                                                       *this,
                                                                                       MutableCueMarker::p_isProtected }));

        this->p_needsToUpdateTrackFile = true;

        return &markers.lastObject()->get<MutableCueMarker>();
    }
    NotNull<Common::MutableGridMarker *> appendGridMarker()
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<Serato::MutableMarkerOfSomeSort>::with(MutableGridMarker{ SeratoGridMarker{ },
                                                                                        *this,
                                                                                        MutableGridMarker::p_isProtected }));

        this->p_needsToUpdateTrackFile = true;

        return &markers.lastObject()->get<MutableGridMarker>();
    }
    NotNull<Common::MutableLoopMarker *> appendLoopMarker()
    {
        auto& markers = this->p_ensureMarkersAreLoaded();
        markers.append(Shared<Serato::MutableMarkerOfSomeSort>::with(MutableLoopMarker{ SeratoLoopMarker{ },
                                                                                        *this,
                                                                                        MutableLoopMarker::p_isProtected }));

        this->p_needsToUpdateTrackFile = true;

        return &markers.lastObject()->get<MutableLoopMarker>();
    }
    Common::MutableMarkerOfSomeSort markerAtIndex(count index)
    {
        return withVariant(*this->p_ensureMarkersAreLoaded()[index], [](auto& marker) {
            return Common::MutableMarkerOfSomeSort{ &marker };
        });
    }
    Common::MarkerOfSomeSort markerAtIndex(count index) const
    {
        return withVariant(*this->p_ensureMarkersAreLoaded()[index], [](auto& marker) {
            return Common::MarkerOfSomeSort{ &marker };
        });
    }
    Array<Common::MarkerOfSomeSort> markers() const
    {
        MutableArray<Common::MarkerOfSomeSort> results;

        for (auto&& marker : this->p_ensureMarkersAreLoaded()) {
            withVariant(*marker, [&results](auto& marker) {
                results.emplaceAppend(&marker.asImmutableReference());
            });
        }

        return std::move(results);
    }
    Array<Common::MutableMarkerOfSomeSort> markers()
    {
        MutableArray<Common::MutableMarkerOfSomeSort> results;

        for (auto&& marker : this->p_ensureMarkersAreLoaded()) {
            withVariant(*marker, [&results](auto& marker) {
                results.emplaceAppend(&marker);
            });
        }

        return std::move(results);
    }
    void setMarkersAndMaybeAddOffsetInSeconds(const Array<Common::MarkerOfSomeSort>&, Optional<DecimalNumber> = nothing);
    void removeMarkerAtIndex(count index)
    {
        auto& markers = this->p_ensureMarkersAreLoaded();

        markers.removeObjectAtIndex(index);

        this->p_needsToUpdateTrackFile = true;
    }

    boolean needsToUpdateDatabaseFile() const;
    boolean needsToUpdateTrackFile() const;
    void setNeedsToUpdateTrackFile(boolean);
    void updateTrackFile() const;

    void addTo(MutableBlob& destination) const;
};

} } }
