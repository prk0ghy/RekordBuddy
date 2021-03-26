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

#include <RekordBuddyCollection/Implementation/Tags/Properties/DecimalProperty.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Internal/TagInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>

namespace NxA {

// -- Operator Specialization
inline constexpr bool operator==(const RekordBuddy::CollectionImplementation::PersistentDecimalProperty& first,
                                 const RekordBuddy::CollectionImplementation::PersistentDecimalProperty& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const RekordBuddy::CollectionImplementation::PersistentDecimalProperty& first,
                                const RekordBuddy::CollectionImplementation::PersistentDecimalProperty& second) noexcept
{
    return first.objectID() < second.objectID();
}

namespace RekordBuddy { namespace CollectionImplementation {

// -- Internal Interface
class DecimalPropertyInternal
{
public:
    template<typename T>
    static std::shared_ptr<T> decimalPropertyWithValueInCollection(const DecimalNumber& value, Collection& collection)
    {
#if defined(NXA_BETA_BUILD)
        // -- This is debugging code for an issue where propertyTypeIDForPersistentObjectType() is being called with an invalid type.
        static_assert(RBSchema::TypeEnumeration<T>::value != RekordBuddy::CollectionImplementation::RBSchema::Type::Track, "Invalid object type.");
        static_assert(RBSchema::TypeEnumeration<T>::value != RekordBuddy::CollectionImplementation::RBSchema::Type::Playlist, "Invalid object type.");
#endif
        auto context = collection.persistentContext();
        auto cacheMap = collection.p_decimalPropertyTagsCache().valueForKeyOrSetWith(RekordBuddy::CollectionImplementation::Property::propertyTypeIDForPersistentObjectType(RBSchema::TypeEnumeration<T>::value),
                                                                                     []() {
                                                                                         return Shared<MutableMap<DecimalNumber, ObjectID>>::with();
                                                                                     });
        auto maybeObjectID = cacheMap->maybeValueForKey(value);
        if (maybeObjectID.isValid()) {
            auto objectID = *maybeObjectID;
            if (objectID.isValid() && !context->isDeleted(objectID)) {
                // -- We found a valid tag in the cache.
                auto maybeObjectInCache = context->fetchObject<T>(*maybeObjectID);
                if (maybeObjectInCache.isValid()) {
                    return *maybeObjectInCache;
                }
            }

            // -- We found an invalid tag so we ignore it.
            cacheMap->removeValueForKey(value);
        }

        // -- If all else fails, we create a new tag for the value.
        auto newTag = context->createObject<T>();
        newTag->setDecimalValue(value);

        // -- And we add it to the cache.
        cacheMap->setValueForKey(newTag->objectID(), value);

        return newTag;
    }

    template<typename T>
    static std::shared_ptr<T> decimalPropertyWithValueInCollection(const String& value, Collection& collection)
    {
        return DecimalPropertyInternal::decimalPropertyWithValueInCollection<T>(DecimalNumber(value), collection);
    }
};

// -- Private Functions
inline void p_deleteDecimalPropertyFromCollection(PersistentDecimalProperty& property)
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto parentTracksCopy = property.parentTracks().objects();
    auto sharedProperty = property.sharedFromThis<PersistentDecimalProperty>();

    p_deleteTagFromCollection<PersistentDecimalProperty>(property);

    for (auto&& parentTrack : parentTracksCopy) {
        if (parentTrack->objectID().isValid()) {
            parentTrack->removeDecimalPropertiesItem(sharedProperty);
        }
    }
}

} } }
