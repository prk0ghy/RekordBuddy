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

#include <RekordBuddyCollection/Implementation/Artist.hpp>
#include <RekordBuddyCollection/Tracks/Track.hpp>

#include <CommonCollection/Artist.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class Artist final : public Common::Artist
{
    // -- Friends
    friend class MutableArtist;
    friend class MutableCollection;

    // -- Private Instance Variables
    CollectionImplementation::Artist p_rekordBuddyArtist;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    inline Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const
    {
        return this->p_rekordBuddyArtist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }
    inline Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection()
    {
        return this->p_rekordBuddyArtist.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }

public:
    // -- Constructors & Destructors
    Artist(CollectionImplementation::Artist&& from, const Protected&) : p_rekordBuddyArtist(std::move(from)) { }
    ~Artist() override = default;

    // -- Common::Artist Overridden Instance Methods
    NotNull<const Common::Collection*> collection() const override
    {
        return this->p_rekordBuddyCollection().asNotNull();
    }

    String name() const override
    {
        return this->p_rekordBuddyArtist.name();
    }

    Array<NotNull<const Common::Track*>> tracks() const override
    {
        auto& collection = this->p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<const Common::Track*>> results;

        for (auto&& objectID : this->p_rekordBuddyArtist.objectIDsOfAllTracksCredited()) {
            results.append(collection.trackWithObjectID(objectID).as<const Common::Track*>());
        }

        return std::move(results);
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsArtist() const override
    {
        auto& collection = this->p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<const Common::Track*>> results;

        for (auto&& objectID : this->p_rekordBuddyArtist.objectIDsOfTracksCreditedAsArtist()) {
            results.append(collection.trackWithObjectID(objectID).as<const Common::Track*>());
        }

        return std::move(results);
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsRemixer() const override
    {
        auto& collection = this->p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<const Common::Track*>> results;

        for (auto&& objectID : this->p_rekordBuddyArtist.objectIDsOfTracksCreditedAsRemixer()) {
            results.append(collection.trackWithObjectID(objectID).as<const Common::Track*>());
        }

        return std::move(results);
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsProducer() const override
    {
        auto& collection = this->p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<const Common::Track*>> results;

        for (auto&& objectID : this->p_rekordBuddyArtist.objectIDsOfTracksCreditedAsProducer()) {
            results.append(collection.trackWithObjectID(objectID).as<const Common::Track*>());
        }

        return std::move(results);
    }
};

class MutableArtist final : public Common::MutableArtist
{
    // -- Private Instance Variables
    RekordBuddy::Artist p_artist;

public:
    // -- Constructors & Destructors
    MutableArtist(CollectionImplementation::Artist&& from,
                  const RekordBuddy::Artist::Protected& isProtected) : p_artist(std::move(from),
                                                                                isProtected) { }
    ~MutableArtist() override = default;

    // -- Overridden Common::MutableArtist Instance Methods
    NotNull<Common::MutableCollection*> collection() override
    {
        return this->p_artist.p_rekordBuddyCollection().asNotNull();
    }

    void setName(const String& name) override
    {
        auto& artist = this->p_artist.p_rekordBuddyArtist;
        (*artist.collection()).renameArtistTo(artist, name);
    }

    Array<NotNull<Common::MutableTrack*>> tracks() override
    {
        auto& collection = this->p_artist.p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<Common::MutableTrack*>> results;

        for (auto&& objectID : this->p_artist.p_rekordBuddyArtist.objectIDsOfAllTracksCredited()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }
    Array<NotNull<Common::MutableTrack*>> tracksCreditedAsArtist() override
    {
        auto& collection = this->p_artist.p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<Common::MutableTrack*>> results;

        for (auto&& objectID : this->p_artist.p_rekordBuddyArtist.objectIDsOfTracksCreditedAsArtist()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }
    Array<NotNull<Common::MutableTrack*>> tracksCreditedAsRemixer() override
    {
        auto& collection = this->p_artist.p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<Common::MutableTrack*>> results;

        for (auto&& objectID : this->p_artist.p_rekordBuddyArtist.objectIDsOfTracksCreditedAsRemixer()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }
    Array<NotNull<Common::MutableTrack*>> tracksCreditedAsProducer() override
    {
        auto& collection = this->p_artist.p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<Common::MutableTrack*>> results;

        for (auto&& objectID : this->p_artist.p_rekordBuddyArtist.objectIDsOfTracksCreditedAsProducer()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }

    // -- Overridden Common::Artist Instance Methods
    NotNull<const Common::Collection*> collection() const override
    {
        return this->p_artist.collection();
    }

    String name() const override
    {
        return this->p_artist.name();
    }

    Array<NotNull<const Common::Track*>> tracks() const override
    {
        return this->p_artist.tracks();
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsArtist() const override
    {
        return this->p_artist.tracksCreditedAsArtist();
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsRemixer() const override
    {
        return this->p_artist.tracksCreditedAsArtist();
    }
    Array<NotNull<const Common::Track*>> tracksCreditedAsProducer() const override
    {
        return this->p_artist.tracksCreditedAsArtist();
    }
};

} }
