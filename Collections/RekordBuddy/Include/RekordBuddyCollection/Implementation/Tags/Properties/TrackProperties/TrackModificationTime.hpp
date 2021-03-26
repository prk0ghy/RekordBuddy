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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TimeProperty.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

#include <Base/Base.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

#include <Base/ResetObjectForwardDeclarations.ipp>

// -- Forward Declarations
#define NXA_OBJECT_CLASS                                        TrackModificationTime
#define NXA_OBJECT_BASE_CLASS                                   TimeProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectForwardDeclarations.ipp>

class Collection;
    
// -- Public Interface
class TrackModificationTime : public NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

public:
    // -- Constants
    static constexpr boolean canOnlyHaveOneTagAddedToATrack = true;
    // -- These are stored in user data and should not be modified.
    static constexpr Common::Property::TypeID typeID = Common::Property::TypeID::LastModifiedOn;

    // -- Factory Methods
    static TrackModificationTime withValueInCollection(const Time&, Collection&);

    // -- Instance Methods
    Time value() const;
};
    
} } }
