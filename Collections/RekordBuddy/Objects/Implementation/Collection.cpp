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

#include <RekordBuddyCollection/Implementation/Artist.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/TrackDisplayCache.hpp>
#include <RekordBuddyCollection/Implementation/Crates/RootFolder.hpp>
#include <RekordBuddyCollection/Implementation/Crates/AllTracksPlaylist.hpp>
#include <RekordBuddyCollection/Implementation/CollectionInternal.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>

#include <CommonCollection/Markers/MarkerOffset.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <memory>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

#define NXA_OBJECT_CLASS                            Collection

#include <Base/ObjectDefinition.ipp>

// -- Inspector Used To Get Track References From Predicates

class CollectionTrackPredicateInspectorData
{
public:
    // -- Constructors & Destructors
    CollectionTrackPredicateInspectorData(Set<ObjectID>&& from) : trackObjectIDs(std::move(from)) { }

    // -- Instance Variables
    Set<ObjectID> trackObjectIDs;
};

struct CollectionTrackPredicateInspector
{
    using Result = std::unique_ptr<CollectionTrackPredicateInspectorData>;

    // -- Class Methods
    Result fromComparisonPredicate(const Common::TrackPredicate& tag,
                                   Common::Property::TypeID tagTypeID,
                                   Common::TrackPredicate::ComparisonOperator comparisonOperator,
                                   String value)
    {
        switch (Common::Property::valueTypeIDFromPropertyType(tagTypeID)) {
            case Common::Property::StringValueType: {
                auto result = this->internal->tracksMatchingComparisonInStringProperties(tagTypeID,
                                                                                         comparisonOperator,
                                                                                         value);
                return std::make_unique<CollectionTrackPredicateInspectorData>(std::move(result));
            }
            case Common::Property::IntegerValueType: {
                auto result = this->internal->tracksMatchingComparisonInIntegerProperties(tagTypeID,
                                                                                          comparisonOperator,
                                                                                          value.integerValue());
                return std::make_unique<CollectionTrackPredicateInspectorData>(std::move(result));
            }
            case Common::Property::TimeValueType: {
                auto result = this->internal->tracksMatchingComparisonInTimeProperties(tagTypeID,
                                                                                       comparisonOperator,
                                                                                       value);
                return std::make_unique<CollectionTrackPredicateInspectorData>(std::move(result));
            }
            case Common::Property::DecimalValueType: {
                auto result = this->internal->tracksMatchingComparisonInDecimalProperties(tagTypeID,
                                                                                          comparisonOperator,
                                                                                          DecimalNumber(value));
                return std::make_unique<CollectionTrackPredicateInspectorData>(std::move(result));
            }
            case Common::Property::OtherValueType: {
                // -- This tag uniqueID is in the 'other' category and the data does not come from tags.
                if (tagTypeID == Common::Property::TypeID::RelativeFilePath) {
                    // TODO
                }
                else if (tagTypeID == Common::Property::TypeID::FileType) {
                    // TODO
                }
            }
            [[clang::fallthrough]];
            default: {
                NXA_ALOG_WITH_FORMAT("Illegal tag unique ID value %hhu.", tagTypeID);
            }
        }
    }
    Result fromCompoundPredicate(const Common::TrackPredicate& tag, Result left,
                                 Common::TrackPredicate::CompoundOperator compoundOperator,
                                 Result right)
    {
        auto& leftSideTracks = left->trackObjectIDs;
        auto& rightSideTracks = right->trackObjectIDs;

        if (compoundOperator == Common::TrackPredicate::CompoundOperator::And) {
            auto result = leftSideTracks.intersectionWith(rightSideTracks);
            return std::make_unique<CollectionTrackPredicateInspectorData>(CollectionTrackPredicateInspectorData(std::move(result)));
        }
        else if (compoundOperator == Common::TrackPredicate::CompoundOperator::Or) {
            auto result = leftSideTracks.unionWith(rightSideTracks);
            return std::make_unique<CollectionTrackPredicateInspectorData>(CollectionTrackPredicateInspectorData(std::move(result)));
        }
        else {
            NXA_ALOG_WITH_FORMAT("Illegal compound operator 0x%02hhx.", compoundOperator);
        }
    }

    CollectionTrackPredicateInspector(CollectionInternal* withInternal) : internal{withInternal} {}

    // -- Instance Variables
    CollectionInternal* internal;
};

// -- Constructor & Destructors

Collection::Collection(const Volume& onVolume) : std::shared_ptr<Internal>{ std::make_shared<Internal>(onVolume) } { }
Collection::Collection(const FilePath& usingFile) : std::shared_ptr<Internal>{ std::make_shared<Internal>(usingFile) } { }

// -- Instance Methods

Volume Collection::volume() const
{
    return Volume{ nxa_const_internal->filePath() };
}

FilePath Collection::filePath() const
{
    return nxa_const_internal->filePath();
}

Optional<Common::Collection::Error> Collection::open()
{
    try {
        auto internal = nxa_internal;
        if (internal->isOpened()) {
            return nothing;
        }

        auto maybeError = internal->open();
        if (maybeError.isValid()) {
            return maybeError;
        }

        // -- Next we want to make sure root folders and all tracks playlists are created if they do not exist.
        RootFolder::inCollection(*this);
        AllTracksPlaylist::inCollection(*this);
    }
    catch (Exception& e) {
        CrashLog::addBreadCrumb(String::stringWithFormat("Error opening Rekord Buddy collection: '%s'.", e.what()));

        return Common::Collection::Error::Corrupted;
    }

    // -- We store our own shared wrapper to this collection in the user data, this makes it easy (and fast) to retrieve.
    this->persistentContext()->setUserData(this);

    return nothing;
}

boolean Collection::isOpened() const
{
    return nxa_const_internal->isOpened();
}

boolean Collection::hasChangesToSave() const
{
    return nxa_const_internal->context->contextHasChanges();
}

void Collection::reset()
{
    auto internal = nxa_internal;
    internal->reset();
    internal->context->reset();
}

static void p_doNothing(double) { }

void Collection::save()
{
    this->saveWithProgress(p_doNothing);
}

void Collection::saveWithProgress(std::function<void(double)>&& progressCallback)
{
    auto internal = nxa_internal;
    auto context = internal->context;

    context->saveContextWithProgress(std::move(progressCallback));
}

NotNull<const RekordBuddy::MutableTrack*> Collection::trackWithObjectID(const ObjectID& reference) const
{
    auto internal = nxa_const_internal;

    auto maybeTrackDisplayCache = internal->tracksDisplayCache().maybeValueForKey(reference);
    NXA_ASSERT_TRUE(maybeTrackDisplayCache.isValid());

    auto relativeFilePath = maybeTrackDisplayCache->stringValuesForTagOfType(Common::Property::TypeID::RelativeFilePath);
    NXA_ASSERT_TRUE(relativeFilePath.length() == 1);

    auto maybeTrack = this->maybeExistingTrackWithRelativeFilePath(FilePath{ relativeFilePath.firstObject() });
    NXA_ASSERT_TRUE(maybeTrack.isValid());

    return maybeTrack->as<const RekordBuddy::MutableTrack*>();
}

NotNull<RekordBuddy::MutableTrack*> Collection::trackWithObjectID(const ObjectID& reference)
{
    auto internal = nxa_internal;

    auto maybeTrackDisplayCache = internal->tracksDisplayCache().maybeValueForKey(reference);
    NXA_ASSERT_TRUE(maybeTrackDisplayCache.isValid());

    auto relativeFilePath = maybeTrackDisplayCache->stringValuesForTagOfType(Common::Property::TypeID::RelativeFilePath);
    NXA_ASSERT_TRUE(relativeFilePath.length() == 1);

    auto maybeTrack = this->maybeExistingTrackWithRelativeFilePath(FilePath{ relativeFilePath.firstObject() });
    NXA_ASSERT_TRUE(maybeTrack.isValid());

    return *maybeTrack;
}

NotNull<RekordBuddy::MutableTrack*> Collection::trackWithRelativeFilePath(const FilePath& relativeFilePath)
{
    // -- We look for an existing track.
    auto maybeExistingTrack = this->maybeExistingTrackWithRelativeFilePath(relativeFilePath);
    if (maybeExistingTrack.isValid()) {
        return *maybeExistingTrack;
    }

    // -- But if all else fails, we create it.
    auto internal = nxa_internal;
    auto mutableCollection = &*internal->p_rekordBuddyCollection();
    auto newTrack = Shared<MutableTrack>::with(Track::p_createTrackWithRelativeFilePathInCollection(relativeFilePath, *this),
                                               mutableCollection,
                                               RekordBuddy::MutableTrack::p_isProtected);
    internal->p_tracks.append(newTrack);

    auto allTracksPlaylist = AllTracksPlaylist::inCollection(*this);

    // -- The track is new so it can't already be in the all tracks playlist.
    NXA_ASSERT_FALSE_DEBUG(allTracksPlaylist.maybeIndexForTrackObjectID(newTrack->reference()).isValid());

    auto newTrackPointer = newTrack.asRawPointer();
    allTracksPlaylist.addTrackAtIndex(newTrackPointer, allTracksPlaylist.numberOfTracks());

    this->updateCachesForTrack(newTrackPointer);

    // -- Since this track is new we need to calculate any marker offsets that some programs may need.
    newTrack->updateMarkerOffsets();

    return newTrackPointer;
}

Optional<NotNull<const RekordBuddy::MutableTrack*>> Collection::maybeExistingTrackWithRelativeFilePath(const FilePath& relativeFilePath) const
{
    auto& cache = nxa_const_internal->tracksCache();

    auto maybeTrack = cache.maybeValueForKey(relativeFilePath);
    NXA_ASSERT_TRUE(maybeTrack.isValid());

    auto objectID = (*maybeTrack)->reference();
    if (!objectID.isValid() || this->persistentContext().get()->isDeleted(objectID)) {
        // -- We found an invalid track so we ignore it.
        cache.removeValueForKey(relativeFilePath);

        return nothing;
    }

    // -- We found a valid track in the cache.
    return maybeTrack.maybe([](auto track) {
        return track.template as<const RekordBuddy::MutableTrack*>();
    });
}

Optional<NotNull<RekordBuddy::MutableTrack*>> Collection::maybeExistingTrackWithRelativeFilePath(const FilePath& relativeFilePath)
{
    auto& cache = nxa_internal->tracksCache();

    auto maybeTrack = cache.maybeValueForKey(relativeFilePath);
    if(!maybeTrack.isValid()) {
        return nothing;
    }

    auto objectID = (*maybeTrack)->reference();
    if (!objectID.isValid() || this->persistentContext()->isDeleted(objectID)) {
        // -- We found an invalid track so we ignore it.
        cache.removeValueForKey(relativeFilePath);

        return nothing;
    }

    // -- We found a valid track in the cache.
    return maybeTrack;
}

void Collection::removeTrackWithRelativeFilePath(const FilePath& path)
{
    auto maybeExistingTrack = nxa_internal->tracksCache().maybeValueForKey(path);
    NXA_ASSERT_TRUE(maybeExistingTrack.isValid());

    this->deleteTrack((*maybeExistingTrack)->asRekordBuddyTrack());
}

void Collection::removeAllTracks()
{
    auto internal = nxa_internal;
    for (auto&& pair : internal->tracksCache()) {
        pair.second->asRekordBuddyTrack().p_deleteFromCollection();
    }

    internal->tracksDisplayCache().removeAll();
    internal->tracksCache().removeAll();
}

void Collection::updateCachesForTrack(NotNull<RekordBuddy::MutableTrack*> track)
{
    auto internal = nxa_internal;
    auto reference = track->reference();

    // -- Update the track file cache
    auto& cache = internal->tracksDisplayCache();
    auto maybeTrackDisplayCache = cache.maybeValueForKey(reference);
    if (maybeTrackDisplayCache.isValid()) {
        maybeTrackDisplayCache->updateUsingTrack(track->asRekordBuddyTrack());
    }
    else {
        cache.setValueForKey(TrackDisplayCache::usingTrackInCollection(track->asRekordBuddyTrack(), *this), reference);
    }

    // -- Update the tracks cache
    internal->tracksCache().setValueForKey(track, track->relativeFilePath());
}

void Collection::deleteTrack(Track& track)
{
    auto internal = nxa_internal;

    // -- Update the track file cache
    internal->tracksDisplayCache().removeValueForKey(track.reference());

    // -- Update the tracks cache
    internal->tracksCache().removeValueForKey(track.relativeFilePath());

    track.p_deleteFromCollection();
}

Artist Collection::artistNamed(const String& name)
{
    // -- We look for an existing artist
    auto maybeExistingArtist = Collection::maybeExistingArtistNamed(name);
    if (maybeExistingArtist.isValid()) {
        return *maybeExistingArtist;
    }

    // -- But if all else fails, we create it
    auto newArtist = Artist::p_artistNamedInCollection(name, *this);

    // -- And add it to the cache
    nxa_internal->artistsCache().setValueForKey(newArtist->objectID(), name);

    return newArtist;
}

Optional<Artist> Collection::maybeExistingArtistNamed(const String& name)
{
    NXA_ASSERT_FALSE(name.isEmpty());

    auto internal = nxa_internal;
    auto context = internal->context;
    auto& cache = internal->artistsCache();
    auto maybeObjectID = cache.maybeValueForKey(name);
    if (maybeObjectID.isValid()) {
        auto& objectID = *maybeObjectID;
        if (objectID.isValid() && !context->isDeleted(objectID)) {
            // -- We found a valid artist in the cache
            return Artist{ *context->fetchObject<PersistentArtist>(objectID) };
        }

        // -- We found an invalid artist so we ignore it
        cache.removeValueForKey(name);
    }

    return nothing;
}

void Collection::renameArtistTo(Artist& artist, const String& name)
{
    auto internal = nxa_internal;
    auto& cache = internal->artistsCache();

    cache.removeValueForKey(artist.name());
    artist.p_setName(name);
    cache.setValueForKey(artist.persistentObject()->objectID(), name);

    artist.updateSmartContentForParentCratesOfParentTracks();
}

void Collection::deleteArtist(Artist& artist)
{
    auto internal = nxa_internal;

    // -- Update the artists cache
    internal->artistsCache().removeValueForKey(artist.name());

    artist.p_deleteFromCollection();
}

void Collection::deleteAnyOrphanTags()
{
    auto internal = nxa_internal;

    auto context = internal->context;

    Array<std::shared_ptr<PersistentStringProperty>> stringTags{ context->fetchObjects<PersistentStringProperty>() };
    for (auto&& persistentTag : stringTags) {
        auto isDeleted = persistentTag->isDeleted();
        auto tag = StringProperty(std::move(persistentTag));
        if (!isDeleted && tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    Array<std::shared_ptr<PersistentDecimalProperty>> decimalTags{ context->fetchObjects<PersistentDecimalProperty>() };
    for (auto&& persistentTag : decimalTags) {
        auto isDeleted = persistentTag->isDeleted();
        auto tag = DecimalProperty(std::move(persistentTag));
        if (!isDeleted && tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    Array<std::shared_ptr<PersistentIntegerProperty>> integerTags{ context->fetchObjects<PersistentIntegerProperty>() };
    for (auto&& persistentTag : integerTags) {
        auto isDeleted = persistentTag->isDeleted();
        auto tag = IntegerProperty(std::move(persistentTag));
        if (!isDeleted && tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }

    Array<std::shared_ptr<PersistentTimeProperty>> timeTags{ context->fetchObjects<PersistentTimeProperty>() };
    for (auto&& persistentTag : timeTags) {
        auto isDeleted = persistentTag->isDeleted();
        auto tag = TimeProperty(std::move(persistentTag));
        if (!isDeleted && tag.shouldBeDeletedIfOrphan() && tag.isAnOrphan()) {
            tag.deleteFromCollection();
        }
    }
}

Set<ObjectID> Collection::tracksMatchingPredicate(const Common::TrackPredicate& predicate) const
{
    std::unique_ptr<CollectionTrackPredicateInspectorData> result = inspectTrackPredicateUsing(predicate, CollectionTrackPredicateInspector{nxa_internal});
    return result->trackObjectIDs;
}

TrackDisplayCache Collection::trackDisplayCacheFor(const ObjectID& reference) const
{
    auto internal = nxa_const_internal;

    auto& cache = internal->tracksDisplayCache();
    if (!cache.length()) {
        for (auto persistentTrackDisplayCache : internal->context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentTrackDisplayCache>()) {
            cache.setValueForKey(TrackDisplayCache(std::move(persistentTrackDisplayCache)),
                                 ObjectID(persistentTrackDisplayCache->parentTrackObjectID()));
        }
    }

    return cache[reference];
}

RootContext& Collection::persistentContext()
{
    return nxa_internal->context;
}

Const<RootContext> Collection::persistentContext() const
{
    return nxa_const_internal->context;
}

MutableMap<Common::Property::TypeID, Shared<MutableMap<String, ObjectID>>>& Collection::p_stringPropertyTagsCache() const
{
    return nxa_const_internal->stringPropertyTagsCache();
}

MutableMap<Common::Property::TypeID, Shared<MutableMap<integer32, ObjectID>>>& Collection::p_integerPropertyTagsCache() const
{
    return nxa_const_internal->integerPropertyTagsCache();
}

MutableMap<Common::Property::TypeID, Shared<MutableMap<DecimalNumber, ObjectID>>>& Collection::p_decimalPropertyTagsCache() const
{
    return nxa_const_internal->decimalPropertyTagsCache();
}

MutableMap<Common::Property::TypeID, Shared<MutableMap<Time, ObjectID>>>& Collection::p_timePropertyTagsCache() const
{
    return nxa_const_internal->timePropertyTagsCache();
}
