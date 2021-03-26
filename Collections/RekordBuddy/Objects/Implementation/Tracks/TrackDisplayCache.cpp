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

#include <RekordBuddyCollection/Implementation/Tracks/TrackDisplayCache.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollection/Implementation/Tracks/Tracks.hpp>

#include <CommonCollection/Tracks/TrackDisplayCache.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Operator Specialization

namespace NxA {

inline constexpr bool operator==(const PersistentTrackDisplayCache& first, const PersistentTrackDisplayCache& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const PersistentTrackDisplayCache& first, const PersistentTrackDisplayCache& second) noexcept
{
    return first.objectID() < second.objectID();
}

}

// -- Implementation

#define NXA_OBJECT_CLASS                                        TrackDisplayCache
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

TrackDisplayCache TrackDisplayCache::usingTrackInCollection(const Track& track, Collection& collection)
{
    auto newPersistentCache = collection.persistentContext()->createObject<PersistentTrackDisplayCache>();
    newPersistentCache->setCacheData(track.p_displayCacheData());
    newPersistentCache->setParentTrack(track.persistentObject());

    return TrackDisplayCache(std::move(newPersistentCache));
}

// -- Instance Methods

void TrackDisplayCache::updateUsingTrack(const Track& track)
{
    nxa_internal->setCacheData(track.p_displayCacheData());
}

Array<String> TrackDisplayCache::stringValuesForTagOfType(Common::Property::TypeID typeID) const
{
    return Common::TrackDisplayCache::stringValuesForTagOfTypeInBlob(typeID, nxa_const_internal->cacheData());
}

Array<integer32> TrackDisplayCache::integerValuesForTagOfType(Common::Property::TypeID typeID) const
{
    return Common::TrackDisplayCache::integerValuesForTagOfTypeInBlob(typeID, nxa_const_internal->cacheData());
}

Array<DecimalNumber> TrackDisplayCache::decimalValuesForTagOfType(Common::Property::TypeID typeID) const
{
    return Common::TrackDisplayCache::decimalValuesForTagOfTypeInBlob(typeID, nxa_const_internal->cacheData());
}

Array<Time> TrackDisplayCache::timeValuesForTagOfType(Common::Property::TypeID typeID) const
{
    return Common::TrackDisplayCache::timeValuesForTagOfTypeInBlob(typeID, nxa_const_internal->cacheData());
}

boolean TrackDisplayCache::isDeletedOrNotInAnyCollection() const
{
    auto internal = nxa_const_internal;
    return internal->isDeleted() || !internal->hasContext();
}

void TrackDisplayCache::deleteFromCollection()
{
    // -- This is a collection object deletion method. This comment can help find them all.
    // -- Collection objects must also delete any orphan children they have but only after deleting
    // -- themselves to prevent infinite recursions. It is assumed that Persistence
    // -- will remove them from any parents (unless the parent is already deleted).
    auto internal = nxa_internal;
    if (internal->isDeleted()) {
        return;
    }

    internal->deleteObject();
}
