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

#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Tags.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <RekordBuddyCollection/Collection.hpp>

namespace NxA {

// -- Operator Specialization
inline constexpr bool operator==(const RekordBuddy::CollectionImplementation::PersistentTrack& first,
                                 const RekordBuddy::CollectionImplementation::PersistentTrack& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const RekordBuddy::CollectionImplementation::PersistentTrack& first,
                                 const RekordBuddy::CollectionImplementation::PersistentTrack& second) noexcept
{
    return first.objectID() < second.objectID();
}

namespace RekordBuddy { namespace CollectionImplementation {

// -- Macros used to generate templates
#define STRING_TAG_TYPES(function, parameter) \
        function(TrackTitle, Title, parameter) \
        function(TrackAlbum, Album, parameter) \
        function(TrackComments, Comments, parameter) \
        function(TrackGenre, Genre, parameter) \
        function(TrackGrouping, Grouping, parameter) \
        function(TrackMixName, MixName, parameter) \
        function(TrackRecordLabel, RecordLabel, parameter) \
        function(TrackTag, Tag, parameter) \
        function(TrackDateAdded, DateAdded, parameter) \
        function(TrackDateReleased, DateReleased, parameter) \

#define INTEGER_TAG_TYPES(function, parameter) \
        function(TrackBeatGridLockedFlag, BeatGridLockedFlag, parameter) \
        function(TrackBitDepth, BitDepthInBits, parameter) \
        function(TrackBitRate, BitRateInKiloBitsPerSecond, parameter) \
        function(TrackColor, Color, parameter) \
        function(TrackFileSize, FileSize, parameter); \
        function(TrackKey, MusicalKey, parameter) \
        function(TrackNumber, TrackNumber, parameter) \
        function(TrackDiscNumber, DiscNumber, parameter) \
        function(TrackPlayCount, PlayCount, parameter) \
        function(TrackRating, Rating, parameter) \
        function(TrackSampleRate, SampleRateInHertz, parameter) \
        function(TrackFileType, FileType, parameter)

#define DECIMAL_TAG_TYPES(function, parameter) \
        function(TrackBpm, BeatsPerMinute, parameter) \
        function(TrackLength, LengthInSeconds, parameter)

#define TIME_TAG_TYPES(function, parameter) \
        function(TrackModificationTime, LastModifiedOn, parameter)

#define ALL_TAG_TYPES(function, parameter) \
        STRING_TAG_TYPES(function, parameter) \
        INTEGER_TAG_TYPES(function, parameter) \
        DECIMAL_TAG_TYPES(function, parameter) \
        TIME_TAG_TYPES(function, parameter)

#define ALL_BASE_TYPES(function, parameter) \
        function(StringProperty, parameter) \
        function(IntegerProperty, parameter) \
        function(DecimalProperty, parameter) \
        function(TimeProperty, parameter)

// -- Convert a tag typeID to a Persistent type
template <typename T>
    struct PersistentTypeFromBaseType
    {
        using Type = void;
    };

#define PERSISTENT_TYPE_FROM_BASE_TYPE(name, argument) \
        template<> \
        struct PersistentTypeFromBaseType<name> \
        { \
            using Type = Persistent##name; \
        };
ALL_BASE_TYPES(PERSISTENT_TYPE_FROM_BASE_TYPE, 0);
#undef PERSISTENT_TYPE_FROM_BASE_TYPE

template <Common::Property::TypeID t>
    struct PersistentTypeFromTypeID
    {
        using Type = void;
    };

#define PERSISTENT_TYPE_FROM_TYPEID(name, typeID, argument) \
        template<> \
        struct PersistentTypeFromTypeID<Common::Property::TypeID::typeID> \
        { \
            using Type = Persistent##name; \
        };
ALL_TAG_TYPES(PERSISTENT_TYPE_FROM_TYPEID, 0);
#undef PERSISTENT_TYPE_FROM_TYPEID

// -- Convert a tag typeID to a Base Persistent type
template <Common::Property::TypeID t>
    struct BasePersistentTypeFromTypeID
    {
        using Type = void;
    };

#define BASE_PERSISTENT_TYPE_FROM_TYPEID(name, typeID, argument) \
        template<> \
        struct BasePersistentTypeFromTypeID<Common::Property::TypeID::typeID> \
        { \
            using Type = argument; \
        };
STRING_TAG_TYPES(BASE_PERSISTENT_TYPE_FROM_TYPEID, PersistentStringProperty);
INTEGER_TAG_TYPES(BASE_PERSISTENT_TYPE_FROM_TYPEID, PersistentIntegerProperty);
DECIMAL_TAG_TYPES(BASE_PERSISTENT_TYPE_FROM_TYPEID, PersistentDecimalProperty);
TIME_TAG_TYPES(BASE_PERSISTENT_TYPE_FROM_TYPEID, PersistentTimeProperty);
#undef BASE_PERSISTENT_TYPE_FROM_TYPEID

// -- Utility templates to get the right member function based on a type
template <class T>
    struct MemberTraits
    {
    };

#define MEMBER_TRAIT_FOR_STRING_TAG_TYPE(type, typeID, argument) \
        template<> \
        struct MemberTraits<type> \
        { \
            static constexpr auto objectIDMember = &PersistentTrack::stringPropertiesObjectIDs; \
            static constexpr auto propertyMember = &PersistentTrack::stringProperties; \
        };
STRING_TAG_TYPES(MEMBER_TRAIT_FOR_STRING_TAG_TYPE, None)
MEMBER_TRAIT_FOR_STRING_TAG_TYPE(StringProperty, None, None);
#undef MEMBER_TRAIT_FOR_STRING_TAG_TYPE

#define MEMBER_TRAIT_FOR_INTEGER_TAG_TYPE(type, typeID, argument) \
        template<> \
        struct MemberTraits<type> \
        { \
            static constexpr auto objectIDMember = &PersistentTrack::integerPropertiesObjectIDs; \
            static constexpr auto propertyMember = &PersistentTrack::integerProperties; \
        };
INTEGER_TAG_TYPES(MEMBER_TRAIT_FOR_INTEGER_TAG_TYPE, None)
MEMBER_TRAIT_FOR_INTEGER_TAG_TYPE(IntegerProperty, None, None);
#undef MEMBER_TRAIT_FOR_INTEGER_TAG_TYPE

#define MEMBER_TRAIT_FOR_DECIMAL_TAG_TYPE(type, typeID, argument) \
        template<> \
        struct MemberTraits<type> \
        { \
            static constexpr auto objectIDMember = &PersistentTrack::decimalPropertiesObjectIDs; \
            static constexpr auto propertyMember = &PersistentTrack::decimalProperties; \
        };
DECIMAL_TAG_TYPES(MEMBER_TRAIT_FOR_DECIMAL_TAG_TYPE, None)
MEMBER_TRAIT_FOR_DECIMAL_TAG_TYPE(DecimalProperty, None, None);
#undef MEMBER_TRAIT_FOR_DECIMAL_TAG_TYPE

#define MEMBER_TRAIT_FOR_TIME_TAG_TYPE(type, typeID, argument) \
        template<> \
        struct MemberTraits<type> \
        { \
            static constexpr auto objectIDMember = &PersistentTrack::timePropertiesObjectIDs; \
            static constexpr auto propertyMember = &PersistentTrack::timeProperties; \
        };
TIME_TAG_TYPES(MEMBER_TRAIT_FOR_TIME_TAG_TYPE, None)
MEMBER_TRAIT_FOR_TIME_TAG_TYPE(TimeProperty, None, None);
#undef MEMBER_TRAIT_FOR_TIME_TAG_TYPE

// -- Private Functions
inline Optional<RekordBuddy::CollectionImplementation::ObjectID> p_maybeFirstObjectIDOfPropertiesOfTypeInObjectIDs(Common::Property::TypeID type, const Array<ObjectID>& objectIDs)
{
    auto schemaTypeWeAreLookingFor = RekordBuddy::CollectionImplementation::Property::persistentObjectTypeForPropertyTypeID(type);

    for (auto&& objectIDFound : objectIDs) {
        if (RekordBuddy::CollectionImplementation::RBSchema::typeIs(objectIDFound, schemaTypeWeAreLookingFor)) {
            return { objectIDFound };
        }
    }

    return nothing;
}

inline MutableArray<RekordBuddy::CollectionImplementation::ObjectID> p_objectIDsOfPropertiesOfTypeInObjectIDs(Common::Property::TypeID type, const Array<ObjectID>& objectIDs)
{
    MutableArray<RekordBuddy::CollectionImplementation::ObjectID> results;
    auto schemaTypeWeAreLookingFor = RekordBuddy::CollectionImplementation::Property::persistentObjectTypeForPropertyTypeID(type);

    for (auto&& objectIDFound : objectIDs) {
        if (!RekordBuddy::CollectionImplementation::RBSchema::typeIs(objectIDFound, schemaTypeWeAreLookingFor)) {
            if (!results.length()) {
                continue;
            }

            // -- Tags are always grouped together by type so if we've found one already and now
            // -- we have a different type this means our search is over.
            break;
        }

        results.append(objectIDFound);
    }

    return results;
}

template<typename Type, typename PersistentType, typename BasePersistentType>
    inline void p_replaceExistingTagsOfTypeWith(Array<Type> newProperties,
                                                const Array<ObjectID>& existingObjectIDs,
                                                PersistentRelationship<std::shared_ptr<BasePersistentType>, RekordBuddy::CollectionImplementation::RBSchema, PersistentTrack>& existingProperties,
                                                std::shared_ptr<PersistentTrack>& persistentTrack,
                                                Track::AndDeleteOrphans deleteOrphans)
    {
        MutableArray<Type> objectsToDelete;

        // -- We put this code in its own scope so that it releases its lock when its done.
        {
            auto trackObjectID = persistentTrack->objectID();
            auto context = persistentTrack->getContext();
            auto contextLock = context->ensureUnfaultedAndBorrowLock(trackObjectID);

            context->updateObject(trackObjectID);

            count numberOfPropertiesOfTypeFound = 0;
            auto schemaTypeWeAreLookingFor = RekordBuddy::CollectionImplementation::Property::persistentObjectTypeForPropertyTypeID(Type::typeID);
            for (count index = 0; index < existingObjectIDs.length(); ++index) {
                auto& objectIDFound = existingObjectIDs[index];
                if (!RekordBuddy::CollectionImplementation::RBSchema::typeIs(objectIDFound, schemaTypeWeAreLookingFor)) {
                    if (numberOfPropertiesOfTypeFound == 0) {
                        continue;
                    }

                    // -- Tags are always grouped together by type so if we've found one already and now
                    // -- we have a different type this means our search is over.
                    break;
                }

                existingProperties.removeAtIndex(index - numberOfPropertiesOfTypeFound);

                boolean isAPropertyThatWillRemain = false;
                for (auto&& property : newProperties) {
                    auto objectID = static_cast<Type>(property).persistentObject()->objectID();
                    if (objectID == objectIDFound) {
                        isAPropertyThatWillRemain = true;
                        break;
                    }
                }

                if (!isAPropertyThatWillRemain) {
                    context->updateObject(objectIDFound);

                    auto maybeTag = context->fetchObject<PersistentType>(objectIDFound);
                    if (maybeTag.isValid()) {
                        auto tag = *maybeTag;

                        auto& parentTracks = tag->parentTracks();
                        parentTracks.removeObjectWithID(trackObjectID);

                        if ((deleteOrphans == Track::AndDeleteOrphans::Yes) && !parentTracks.length()) {
                            objectsToDelete.emplaceAppend(std::move(tag));
                        }
                    }
                }

                ++numberOfPropertiesOfTypeFound;
            }

            if (Type::canOnlyHaveOneTagAddedToATrack) {
                NXA_ASSERT_TRUE(numberOfPropertiesOfTypeFound <= 1);
                NXA_ASSERT_TRUE(newProperties.length() <= 1);
            }

            for (auto&& property : newProperties) {
                auto persistentObject = static_cast<Type>(property).persistentObject();
                auto objectID = persistentObject->objectID();

                context->updateObject(objectID);
                persistentObject->parentTracks().append(persistentTrack);
                existingProperties.append(persistentObject);
            }
        }

        for (auto&& object : objectsToDelete) {
            object.deleteFromCollection();
        }
    }

template<typename Type>
    inline boolean p_containsAtLeastOneTagIn(const Array<ObjectID> existingObjectIDs)
    {
        auto typeWeAreLookingFor = Property::persistentObjectTypeForPropertyTypeID(Type::typeID);

        for (auto&& objectID : existingObjectIDs) {
            if (objectID.objectType() == typeWeAreLookingFor) {
                return true;
            }
        }

        return false;
    }

template<typename T>
    inline Array<T> p_tagsOfTypeIn(const PersistentTrack* persistentTrack)
    {
        MutableArray<T> result;
        for (auto&& objectID : p_objectIDsOfPropertiesOfTypeInObjectIDs(T::typeID, (persistentTrack->*MemberTraits<T>::objectIDMember)())) {
            auto maybeTag = persistentTrack->getContext()->fetchObject<typename PersistentTypeFromTypeID<T::typeID>::Type>(objectID);
            if (maybeTag.isValid()) {
                result.append(T(std::move(*maybeTag)));
            }
        }

        return std::move(result);
    }

template<typename T>
    inline Optional<T> p_maybeFirstTagOfTypeIn(const PersistentTrack* persistentTrack)
    {
        auto maybeFirstObjectIDOfType = p_maybeFirstObjectIDOfPropertiesOfTypeInObjectIDs(T::typeID, (persistentTrack->*MemberTraits<T>::objectIDMember)());
        if (maybeFirstObjectIDOfType.isValid()) {
            auto maybeTag = persistentTrack->getContext()->fetchObject<typename PersistentTypeFromTypeID<T::typeID>::Type>(*maybeFirstObjectIDOfType);
            if (maybeTag.isValid()) {
                return { T(std::move(*maybeTag)) };
            }
        }

        return nothing;
    }

template<typename T>
    inline Optional<String> p_maybeStringValueOfFirstTagInTrack(const PersistentTrack* persistentTrack)
    {
        auto maybeTag = p_maybeFirstTagOfTypeIn<T>(persistentTrack);
        return maybeTag.isValid() ? Optional<String>(maybeTag->stringValue()) : nothing;
    }

template<typename T>
    inline Array<String> p_stringValuesOfTagsInTrack(const PersistentTrack* persistentTrack)
    {
        MutableArray<String> values;
        for (auto&& tag : p_tagsOfTypeIn<T>(persistentTrack)) {
            values.append(tag.stringValue());
        }

        return std::move(values);
    }

template<typename T>
    void p_replaceExistingTagsInWith(std::shared_ptr<PersistentTrack>& persistentTrack, Array<T> tags, Track::AndDeleteOrphans deleteOrphans)
    {
        auto internal = persistentTrack.get();

        p_replaceExistingTagsOfTypeWith<T, typename PersistentTypeFromTypeID<T::typeID>::Type, typename BasePersistentTypeFromTypeID<T::typeID>::Type>(tags,
                                                                                                                                                       (internal->*MemberTraits<T>::objectIDMember)(),
                                                                                                                                                       (internal->*MemberTraits<T>::propertyMember)(),
                                                                                                                                                       persistentTrack,
                                                                                                                                                       deleteOrphans);
    }

template<typename T>
    inline void p_replaceExistingTagInTrackWithStringValueInCollection(std::shared_ptr<PersistentTrack>& persistentTrack,
                                                                       const Optional<String>& maybeValue,
                                                                       Collection& collection,
                                                                       Track::AndDeleteOrphans deleteOrphans)
    {
        p_replaceExistingTagsInWith<T>(persistentTrack, (maybeValue.isValid() && !maybeValue->isEmpty()) ? Array<T>{ T::withValueInCollection(*maybeValue, collection) } : Array<T>(), deleteOrphans);
    }

template<typename T, typename V>
    inline void p_replaceExistingTagInTrackWithIntegerValueInCollection(std::shared_ptr<PersistentTrack>& persistentTrack,
                                                                        Optional<V> maybeValue,
                                                                        Collection& collection,
                                                                        Track::AndDeleteOrphans deleteOrphans)
    {
        p_replaceExistingTagsInWith<T>(persistentTrack, maybeValue.isValid() ? Array<T>{ T::withValueInCollection(*maybeValue, collection) } : Array<T>{ }, deleteOrphans);
    }

template<typename T>
    inline void p_replaceExistingTagInTrackWithDecimalValueInCollection(std::shared_ptr<PersistentTrack>& persistentTrack,
                                                                        Optional<DecimalNumber> maybeValue,
                                                                        Collection& collection,
                                                                        Track::AndDeleteOrphans deleteOrphans)
    {
        p_replaceExistingTagsInWith<T>(persistentTrack, maybeValue.isValid() ? Array<T>{ T::withValueInCollection(*maybeValue, collection) } : Array<T>{ }, deleteOrphans);
    }

template<typename T>
    inline void p_maybeReplaceExistingTagInTrackWithStringValueInCollection(std::shared_ptr<PersistentTrack>& persistentTrack,
                                                                            const Optional<String>& maybeValue,
                                                                            Collection& collection,
                                                                            Track::AndDeleteOrphans deleteOrphans)
    {
        auto maybeValueResult = (maybeValue.isValid() && maybeValue->length()) ? Optional<T>{ T::maybeWithValueInCollection(*maybeValue, collection) } : nothing;
        p_replaceExistingTagsInWith<T>(persistentTrack, maybeValueResult.isValid() ? Array<T>{ *maybeValueResult } : Array<T>(), deleteOrphans);
    }

template<typename T>
    inline void p_replaceExistingTagsInTrackWithStringValuesInCollection(std::shared_ptr<PersistentTrack>& persistentTrack,
                                                                         const Array<String>& values,
                                                                         Collection& collection,
                                                                         Track::AndDeleteOrphans deleteOrphans)
    {
        MutableArray<T> newTags;
        for (auto&& value : values) {
            if (!value.isEmpty()) {
                newTags.append(T::withValueInCollection(value, collection));
            }
        }

        p_replaceExistingTagsInWith<T>(persistentTrack, std::move(newTags), deleteOrphans);
    }

inline Pointer<const Collection> p_trackCollection(const PersistentTrack* internal)
{
    return { internal->getContext()->userData<CollectionImplementation::Collection>() };
}

inline Pointer<Collection> p_trackCollection(CollectionImplementation::PersistentTrack* internal)
{
    return { internal->getContext()->userData<CollectionImplementation::Collection>() };
}

// -- Forced Template Instantiations
#define P_REPLACE_EXISTING_TAGS_WITH(type, typeID, argument) \
        template void p_replaceExistingTagsInWith(std::shared_ptr<PersistentTrack>& persistentTrack, Array<type> tags, Track::AndDeleteOrphans deleteOrphans);
ALL_TAG_TYPES(P_REPLACE_EXISTING_TAGS_WITH, None)

} } }
