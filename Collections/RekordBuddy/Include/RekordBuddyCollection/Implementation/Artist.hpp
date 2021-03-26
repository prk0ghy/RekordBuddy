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

#include <Base/Base.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class MutableTrack;

namespace CollectionImplementation {

// -- Forward Declarations
class MutableCollection;
class Collection;

#define NXA_OBJECT_CLASS                                        Artist
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectForwardDeclarations.ipp>

class Track;
class Collection;

// -- Public Interface
class Artist : protected NXA_OBJECT
{
    #include <Base/ObjectDeclaration.ipp>

    friend class Collection;

protected:
    // -- These methods are used by the track collection and should not be called directly by anything else.

    // -- Protected Factory Methods
    static Artist p_artistNamedInCollection(const String&, Collection&);

    // -- Protected Instance Methods
    void p_setName(const String&);
    void p_deleteFromCollection();

public:
    // -- Class Methods
    static Array<Artist> artistsInCollection(Collection&);

    // -- Instance Methods
    String name() const;

    Array<ObjectID> objectIDsOfAllTracksCredited() const;
    Array<ObjectID> objectIDsOfTracksCreditedAsArtist() const;
    Array<ObjectID> objectIDsOfTracksCreditedAsRemixer() const;
    Array<ObjectID> objectIDsOfTracksCreditedAsProducer() const;

    Array<NotNull<const RekordBuddy::MutableTrack*>> allTracksCredited() const;
    Array<NotNull<const RekordBuddy::MutableTrack*>> tracksCreditedAsArtist() const;
    Array<NotNull<const RekordBuddy::MutableTrack*>> tracksCreditedAsRemixer() const;
    Array<NotNull<const RekordBuddy::MutableTrack*>> tracksCreditedAsProducer() const;

    Array<NotNull<RekordBuddy::MutableTrack*>> allTracksCredited();
    Array<NotNull<RekordBuddy::MutableTrack*>> tracksCreditedAsArtist();
    Array<NotNull<RekordBuddy::MutableTrack*>> tracksCreditedAsRemixer();
    Array<NotNull<RekordBuddy::MutableTrack*>> tracksCreditedAsProducer();

    void updateSmartContentForParentCratesOfParentTracks();

    Pointer<const Collection> collection() const;
    Pointer<Collection> collection();
    boolean isDeletedOrNotInAnyCollection() const;
    void deleteFromCollection();
};

} } }
