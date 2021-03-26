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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackModificationTime.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/TimePropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Operator Specialization

namespace NxA {

inline constexpr bool operator==(const PersistentTrackModificationTime& first, const PersistentTrackModificationTime& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const PersistentTrackModificationTime& first, const PersistentTrackModificationTime& second) noexcept
{
    return first.objectID() < second.objectID();
}

}

// -- Implementation

#define NXA_OBJECT_CLASS                                        TrackModificationTime
#define NXA_OBJECT_BASE_CLASS                                   TimeProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

TrackModificationTime TrackModificationTime::withValueInCollection(const Time& value, Collection& collection)
{
    return TrackModificationTime{ DecimalPropertyInternal::timePropertyWithValueInCollection<PersistentTrackModificationTime>(value, collection) };
}

// -- Instance Methods

Time TrackModificationTime::value() const
{
    return nxa_const_internal->timeValue();
}
