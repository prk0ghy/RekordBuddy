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

#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/TrackDisplayCache.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>

#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Collection.hpp>
#include <RekordBuddyCollection/RekordBuddy.hpp>

#include <CommonCollection/Tracks/TrackPredicate.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class MutableTrack;
class MutableCollection;

namespace CollectionImplementation {

#define NXA_OBJECT_CLASS    Collection

// -- Internal Interface
class CollectionInternal
{
    #include <Base/ObjectInternal.ipp>

    // -- Private Class Methods
    static boolean p_copyCollectionFiles(const FilePath& source, const FilePath& destination, boolean andDeleteSource = false)
    {
        if (!File::copyFileAtTo(source, destination)) {
            return false;
        }

        if (andDeleteSource) {
            if (!File::deleteFileAt(source)) {
                return false;
            }
        }

        FilePath sourceShm = source.append(NXA_FILEPATH("-shm"));
        FilePath destinationShm = destination.append(NXA_FILEPATH("-shm"));
        if (File::existsAt(sourceShm, File::IfFileEmptyThenDoesNotExist::No)) {
            if (!File::copyFileAtTo(sourceShm, destinationShm)) {
                return false;
            }

            if (andDeleteSource) {
                if (!File::deleteFileAt(sourceShm, File::IfDoesNotExist::Ignore)) {
                    return false;
                }
            }
        }
        else if (File::existsAt(destinationShm, File::IfFileEmptyThenDoesNotExist::No)) {
            if (!File::deleteFileAt(destinationShm, File::IfDoesNotExist::Ignore)) {
                return false;
            }
        }

        FilePath sourceWal = source.append(NXA_FILEPATH("-wal"));
        FilePath destinationWal = source.append(NXA_FILEPATH("-wal"));
        if (File::existsAt(sourceWal, File::IfFileEmptyThenDoesNotExist::No)) {
            if (!File::copyFileAtTo(sourceWal, destinationWal)) {
                return false;
            }

            if (andDeleteSource) {
                if (!File::deleteFileAt(sourceWal, File::IfDoesNotExist::Ignore)) {
                    return false;
                }
            }
        }
        else if (File::existsAt(destinationWal, File::IfFileEmptyThenDoesNotExist::No)) {
            if (!File::deleteFileAt(destinationWal, File::IfDoesNotExist::Ignore)) {
                return false;
            }
        }

        return true;
    }

    static boolean p_attemptMigration(const FilePath& collectionFile)
    {
        auto maybeDirectory = Directory{ collectionFile }.maybeParent();
        if (!maybeDirectory.isValid()) {
            return false;
        }

        Directory collectionFolder{ *maybeDirectory };
        auto backupFolder = Directory{ FilePath::filePathByJoiningPaths(collectionFolder, NXA_FILEPATH("backups")) };
        if (!backupFolder.createIfDoesNotExist()) {
            return false;
        }

        if (File::existsAt(collectionFile)) {
            auto maybeCollectionFilename = collectionFile.maybeFileName();
            if (!maybeCollectionFilename.isValid()) {
                return false;
            }

            // -- If we don't have one yet, we make a backup of the user's v9 collection in case something goes wrong.
            // -- We only make one of those.
            auto v9Backup = FilePath::filePathByJoiningPaths(backupFolder, *maybeCollectionFilename).append(NXA_FILEPATH(".v9backup"));
            if (!File::existsAt(v9Backup)) {
                if (!CollectionInternal::p_copyCollectionFiles(collectionFile, v9Backup)) {
                    return false;
                }
            }

            // -- Then we create the backup that we are going to use to try and migrate the collection if it needs to be.
            auto migrationBackup = FilePath::filePathByJoiningPaths(backupFolder, *maybeCollectionFilename).append(NXA_FILEPATH("migrationbackup"));
            if (!CollectionInternal::p_copyCollectionFiles(collectionFile, migrationBackup)) {
                return false;
            }

            auto migrationStore = std::make_shared<RBSchema::Store>(migrationBackup);
            auto migrationContext = std::make_shared<PersistentContext<RBSchema>>(migrationStore);
            if (!migrationContext->setupAndMigrate()) {
                return false;
            }

            migrationStore->commit();
            migrationContext = nullptr;
            migrationStore = nullptr;

            // -- If everything went fine we can put the migrated collection back in place.
            if (!CollectionInternal::p_copyCollectionFiles(migrationBackup, collectionFile, true)) {
                return false;
            }
        }

        return true;
    }

    // -- Private Instance Variables
    mutable MutableMap<Common::Property::TypeID, Shared<MutableMap<String, ObjectID>>> p_stringPropertyTagsCacheStorage;
    mutable MutableMap<Common::Property::TypeID, Shared<MutableMap<integer32, ObjectID>>> p_integerPropertyTagsCacheStorage;
    mutable MutableMap<Common::Property::TypeID, Shared<MutableMap<DecimalNumber, ObjectID>>> p_decimalPropertyTagsCacheStorage;
    mutable MutableMap<Common::Property::TypeID, Shared<MutableMap<Time, ObjectID>>> p_timePropertyTagsCacheStorage;
    mutable MutableMap<String, ObjectID> p_artistsCacheStorage;
    mutable MutableMap<FilePath, NotNull<RekordBuddy::MutableTrack*>> p_tracksCacheStorage;
    mutable MutableMap<ObjectID, TrackDisplayCache> p_tracksDisplayCacheStorage;
    mutable MutableArray<Shared<RekordBuddy::MutableTrack>> p_tracks;

    // -- Protected Constructors & Destructors
    inline Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const
    {
        return this->context->userData<RekordBuddy::MutableCollection>();
    }
    inline Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection()
    {
        return this->context->userData<RekordBuddy::MutableCollection>();
    }

public:
    // -- Instance Variables
    File collectionDatabaseFile;

    RootContext context;

    // -- Constructors & Destructors
    explicit CollectionInternal(const Volume& volumePath) : collectionDatabaseFile{ rekordBuddyDatabasePathOnVolume(volumePath) } { }
    explicit CollectionInternal(const FilePath& filePath) : collectionDatabaseFile{ filePath } { }
    virtual ~CollectionInternal()
    {
#if defined(DEBUG)
        // -- We can't throw here in release builds so hopefully this gets caught in debug builds
        if (this->context != nullptr && this->context.use_count() != 1) {
            NXA_DLOG_WITH_FORMAT("Somehow we still have a retain count of %ld for the persistent context", this->context.use_count());
        }
#endif
    }

    // -- Operators
    inline bool operator==(const CollectionInternal& other) const noexcept
    {
        return this->context == other.context;
    }
    inline bool operator<(const CollectionInternal& other) const noexcept
    {
        return this->context < other.context;
    }

    // -- Instance Methods
    FilePath filePath() const
    {
        return this->collectionDatabaseFile.path();
    }

    Optional<Common::Collection::Error> open()
    {
        if (this->context != nullptr) {
            return nothing;
        }

        auto maybeDirectory = Directory{ this->filePath() }.maybeParent();
        if (!maybeDirectory.isValid()) {
            return Common::Collection::Error::CannotOpen;
        }

        Directory collectionFolder{ *maybeDirectory };
        Volume collectionVolume{ collectionFolder };
        collectionFolder.createIfDoesNotExist();

        auto store = std::make_shared<RBSchema::Store>(this->filePath());
        this->context = std::make_shared<PersistentContext<RBSchema>>(store);

        if (!this->context->setupAndMigrate()) {
            this->context = nullptr;
            return Common::Collection::Error::CannotOpen;
        }

        return nothing;
    }
    boolean isOpened() const
    {
        return this->context != nullptr;
    }
    void reset()
    {
        this->p_stringPropertyTagsCacheStorage.removeAll();
        this->p_integerPropertyTagsCacheStorage.removeAll();
        this->p_decimalPropertyTagsCacheStorage.removeAll();
        this->p_timePropertyTagsCacheStorage.removeAll();
        this->p_artistsCacheStorage.removeAll();
        this->p_tracksCacheStorage.removeAll();
        this->p_tracksDisplayCacheStorage.removeAll();
    }

    template <typename T>
        std::shared_ptr<T> createObject()
        {
            NXA_ASSERT_TRUE(this->context != nullptr);

            return context->createObject<T>();
        }

    template <typename T>
        Set<std::shared_ptr<T>> fetchObjects()
        {
            NXA_ASSERT_TRUE(this->context != nullptr);

            return context->fetchObjects<T>();
        }

    MutableMap<Common::Property::TypeID, Shared<MutableMap<String, ObjectID>>>& stringPropertyTagsCache() const
    {
        NXA_ASSERT_TRUE(this->context != nullptr);

        auto& cache = this->p_stringPropertyTagsCacheStorage;
        if (cache.length() == 0) {
            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentTag : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentStringProperty>()) {
                auto objectID = persistentTag->objectID();
                cache.valueForKeyOrSetWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(objectID.objectType()),
                                           []() {
                                               return Shared<MutableMap<String, ObjectID>>::with();
                                           })->setValueForKey(objectID,
                                                              std::static_pointer_cast<RekordBuddy::CollectionImplementation::PersistentStringProperty>(persistentTag)->stringValue());
            }
        }

        return cache;
    }
    MutableMap<Common::Property::TypeID, Shared<MutableMap<integer32, ObjectID>>>& integerPropertyTagsCache() const
    {
        NXA_ASSERT_TRUE(this->context != nullptr);

        auto& cache = this->p_integerPropertyTagsCacheStorage;
        if (cache.length() == 0) {
            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentTag : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentIntegerProperty>()) {
                auto objectID = persistentTag->objectID();
                cache.valueForKeyOrSetWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(objectID.objectType()),
                                           []() {
                                               return Shared<MutableMap<integer32, ObjectID>>::with();
                                           })->setValueForKey(objectID,
                                                              std::static_pointer_cast<RekordBuddy::CollectionImplementation::PersistentIntegerProperty>(persistentTag)->integerValue());
            }
        }

        return cache;
    }
    MutableMap<Common::Property::TypeID, Shared<MutableMap<DecimalNumber, ObjectID>>>& decimalPropertyTagsCache() const
    {
        auto& cache = this->p_decimalPropertyTagsCacheStorage;
        if (cache.length() == 0) {
            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentTag : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentDecimalProperty>()) {
                auto objectID = persistentTag->objectID();
                cache.valueForKeyOrSetWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(objectID.objectType()),
                                           []() {
                                               return Shared<MutableMap<DecimalNumber, ObjectID>>::with();
                                           })->setValueForKey(objectID,
                                                              DecimalNumber(std::static_pointer_cast<RekordBuddy::CollectionImplementation::PersistentDecimalProperty>(persistentTag)->decimalValue()));
            }
        }

        return cache;
    }
    MutableMap<Common::Property::TypeID, Shared<MutableMap<Time, ObjectID>>>& timePropertyTagsCache() const
    {
        auto& cache = this->p_timePropertyTagsCacheStorage;
        if (cache.length() == 0) {
            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentTag : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentTimeProperty>()) {
                auto objectID = persistentTag->objectID();
                cache.valueForKeyOrSetWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(objectID.objectType()),
                                           []() {
                                               return Shared<MutableMap<Time, ObjectID>>::with();
                                           })->setValueForKey(objectID,
                                                              std::static_pointer_cast<RekordBuddy::CollectionImplementation::PersistentTimeProperty>(persistentTag)->timeValue());
            }
        }

        return cache;
    }
    MutableMap<String, ObjectID>& artistsCache() const
    {
        auto& cache = this->p_artistsCacheStorage;
        if (cache.length() == 0) {
            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentArtist : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentArtist>()) {
                cache.setValueForKey(persistentArtist->objectID(), persistentArtist->name());
            }
        }

        return cache;
    }
    MutableMap<FilePath, NotNull<RekordBuddy::MutableTrack*>>& tracksCache() const
    {
        auto& cache = this->p_tracksCacheStorage;
        if (cache.length() == 0) {
            auto mutatedCollection = this->p_rekordBuddyCollection().asMutated();

            // -- If the cache doesn't exist, we prime it.
            for (auto&& persistentTrack : context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentTrack>()) {
                this->p_tracks.append(Shared<MutableTrack>::with(ObjectID{ persistentTrack->objectID() }, mutatedCollection, MutableTrack::p_isProtected));

                cache.setValueForKey(this->p_tracks.lastObject().asRawPointer(),
                                     FilePath::filePathFromCrossPlatformSerialization(std::static_pointer_cast<RekordBuddy::CollectionImplementation::PersistentTrack>(persistentTrack)
                                                                                              ->filePathRelativeToCollectionVolume()));
            }
        }

        return cache;
    }
    MutableMap<ObjectID, TrackDisplayCache>& tracksDisplayCache() const
    {
        auto& cache = this->p_tracksDisplayCacheStorage;
        if (cache.length() == 0) {
            Array<std::shared_ptr<RekordBuddy::CollectionImplementation::PersistentTrackDisplayCache>> tracks{ context->fetchObjects<RekordBuddy::CollectionImplementation::PersistentTrackDisplayCache>() };
            for (auto&& persistentTrackDisplayCache : tracks) {
                auto objectID = persistentTrackDisplayCache->parentTrackObjectID();
                cache.setValueForKey(TrackDisplayCache(std::move(persistentTrackDisplayCache)), objectID);
            }
        }

        return cache;
    }

    template <typename T>
        inline void addTrackObjectIDsFromTagWithObjectIDTo(const ObjectID& id, MutableSet<ObjectID>* result) const
        {
            auto property = this->context->fetchObject<T>(id);
            if (property.isValid()) {
                for (auto&& objectID : (*property)->parentTracksObjectIDs()) {
                    result->add(objectID);
                }
            }
        }

    Set<ObjectID> tracksMatchingComparisonInStringProperties(Common::Property::TypeID tagTypeID,
                                                             Common::TrackPredicate::ComparisonOperator comparisonOperator, const String& value) const
    {
        MutableSet<ObjectID> result;

        auto maybeTags = this->stringPropertyTagsCache().maybeValueForKey(tagTypeID);
        if (maybeTags.isValid()) {
            switch (comparisonOperator) {
                case Common::TrackPredicate::ComparisonOperator::Equal: {
                    auto maybeTag = (*maybeTags)->maybeValueForKey(value);
                    if (maybeTag.isValid()) {
                        this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(*maybeTag, &result);
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::NotEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first != value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::StartsWith: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first.hasPrefix(value)) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::EndsWith: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first.hasPostfix(value)) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::Contains: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first.contains(value)) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::DoesNotContains: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (!valueIDPair.first.contains(value)) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentStringProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThan:
                case Common::TrackPredicate::ComparisonOperator::LessThanOrEqual:
                case Common::TrackPredicate::ComparisonOperator::GreaterThan:
                case Common::TrackPredicate::ComparisonOperator::GreaterThanOrEqual:
                    break;
            }
        }

        return std::move(result);
    }

    Set<ObjectID> tracksMatchingComparisonInIntegerProperties(Common::Property::TypeID tagTypeID,
                                                              Common::TrackPredicate::ComparisonOperator comparisonOperator, integer32 value) const
    {
        MutableSet<ObjectID> result;

        auto maybeTags = this->integerPropertyTagsCache().maybeValueForKey(tagTypeID);
        if (maybeTags.isValid()) {
            switch (comparisonOperator) {
                case Common::TrackPredicate::ComparisonOperator::Equal: {
                    auto maybeTag = (*maybeTags)->maybeValueForKey(value);
                    if (maybeTag.isValid()) {
                        this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(*maybeTag, &result);
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::NotEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first != value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first < value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first <= value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first > value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first >= value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentIntegerProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::StartsWith:
                case Common::TrackPredicate::ComparisonOperator::EndsWith:
                case Common::TrackPredicate::ComparisonOperator::Contains:
                case Common::TrackPredicate::ComparisonOperator::DoesNotContains:
                    break;
            }
        }

        return std::move(result);
    }

    Set<ObjectID> tracksMatchingComparisonInDateProperties(Common::Property::TypeID tagTypeID,
                                                           Common::TrackPredicate::ComparisonOperator comparisonOperator, const String& value) const
    {
        MutableSet<ObjectID> result;

        return std::move(result);
    }

    Set<ObjectID> tracksMatchingComparisonInTimeProperties(Common::Property::TypeID tagTypeID,
                                                           Common::TrackPredicate::ComparisonOperator comparisonOperator, const String& value) const
    {
        auto maybeTimeValue = Time::maybeTimeFromStringInLocalTimeZoneUsingFormat(value, Time::defaultStringFormatWithJustDay);
        if (!maybeTimeValue.isValid()) {
            return {};
        }

        auto dateValue = *maybeTimeValue;

        MutableSet<ObjectID> result;

        auto maybeTags = this->timePropertyTagsCache().maybeValueForKey(tagTypeID);
        if (maybeTags.isValid()) {
            switch (comparisonOperator) {
                case Common::TrackPredicate::ComparisonOperator::Equal: {
                    auto maybeTag = (*maybeTags)->maybeValueForKey(dateValue);
                    if (maybeTag.isValid()) {
                        this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(*maybeTag, &result);
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::NotEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first != dateValue) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first < dateValue) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (dateValue.isLaterThan(valueIDPair.first) || dateValue == valueIDPair.first) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first.isLaterThan(dateValue)) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(valueIDPair.second, &result);
                        }
                    }
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first.isLaterThan(dateValue) || valueIDPair.first == dateValue) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentTimeProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::StartsWith:
                case Common::TrackPredicate::ComparisonOperator::EndsWith:
                case Common::TrackPredicate::ComparisonOperator::Contains:
                case Common::TrackPredicate::ComparisonOperator::DoesNotContains:
                    break;
            }
        }

        return std::move(result);
    }

    Set<ObjectID> tracksMatchingComparisonInDecimalProperties(Common::Property::TypeID tagTypeID,
                                                              Common::TrackPredicate::ComparisonOperator comparisonOperator,
                                                              DecimalNumber value) const
    {
        MutableSet<ObjectID> result;

        auto maybeTags = this->decimalPropertyTagsCache().maybeValueForKey(tagTypeID);
        if (maybeTags.isValid()) {
            switch (comparisonOperator) {
                case Common::TrackPredicate::ComparisonOperator::Equal: {
                    auto maybeTag = (*maybeTags)->maybeValueForKey(value);
                    if (maybeTag.isValid()) {
                        this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(*maybeTag, &result);
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::NotEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first != value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first < value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::LessThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first <= value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThan: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first > value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::GreaterThanOrEqual: {
                    for (auto&& valueIDPair : **maybeTags) {
                        if (valueIDPair.first >= value) {
                            this->addTrackObjectIDsFromTagWithObjectIDTo<PersistentDecimalProperty>(valueIDPair.second, &result);
                        }
                    }
                    break;
                }
                case Common::TrackPredicate::ComparisonOperator::StartsWith:
                case Common::TrackPredicate::ComparisonOperator::EndsWith:
                case Common::TrackPredicate::ComparisonOperator::Contains:
                case Common::TrackPredicate::ComparisonOperator::DoesNotContains:
                    break;
            }
        }

        return std::move(result);
    }
};

}
}}
