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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackKey.hpp>

#include <CommonCollection/Tracks/MusicalKey.hpp>

namespace NxA { namespace RekordBuddy {

// -- Public Interface
class MusicalKey final : public Common::MusicalKey
{
    // -- Friends
    friend class MutableMusicalKey;
    friend class MutableCollection;

    // -- Private Instance Variables
    CollectionImplementation::TrackKey p_rekordBuddyMusicalKey;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    inline Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const
    {
        return this->p_rekordBuddyMusicalKey.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }
    inline Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection()
    {
        return this->p_rekordBuddyMusicalKey.persistentObject()->getContext()->userData<RekordBuddy::MutableCollection>();
    }

public:
    // -- Constructors & Destructors
    MusicalKey(CollectionImplementation::TrackKey&& from, const Protected&) : p_rekordBuddyMusicalKey{ std::move(from) } { }
    ~MusicalKey() override = default;

    // -- Overridden Common::MusicKeyInternal Instance Methods
    NotNull<const Common::Collection*> collection() const override
    {
        return this->p_rekordBuddyCollection().asNotNull();
    }

    Common::MusicalKey::Value value() const override
    {
        return this->p_rekordBuddyMusicalKey.value();
    }
    String stringValue() const override
    {
        return this->p_rekordBuddyMusicalKey.stringValue();
    }

    Array<NotNull<const Common::Track*>> tracks() const override
    {
        auto& collection = this->p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<const Common::Track*>> results;

        for (auto&& objectID : this->p_rekordBuddyMusicalKey.trackObjectIDs()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }
};

// -- Internal Interface
class MutableMusicalKey final : public Common::MutableMusicalKey
{
    // -- Private Instance Variables
    RekordBuddy::MusicalKey p_musicalKey;

public:
    // -- Constructors & Destructors
    MutableMusicalKey(CollectionImplementation::TrackKey&& from,
                      const RekordBuddy::MusicalKey::Protected& isProtected) : p_musicalKey{ std::move(from),
                                                                                             isProtected } { }
    ~MutableMusicalKey() override = default;

    // -- Overridden Common::MutableMusicalKey Instance Methods
    NotNull<Common::MutableCollection*> collection() override
    {
        return this->p_musicalKey.p_rekordBuddyCollection().asNotNull();
    }

    Array<NotNull<Common::MutableTrack*>> tracks() override
    {
        auto& collection = this->p_musicalKey.p_rekordBuddyCollection().asReference();

        MutableArray<NotNull<Common::MutableTrack*>> results;

        for (auto&& objectID : this->p_musicalKey.p_rekordBuddyMusicalKey.trackObjectIDs()) {
            results.append(collection.trackWithObjectID(objectID));
        }

        return std::move(results);
    }

    // -- Overridden Common::MusicalKey Instance Methods
    NotNull<const Common::Collection*> collection() const override
    {
        return this->p_musicalKey.collection();
    }

    Common::MusicalKey::Value value() const override
    {
        return this->p_musicalKey.value();
    }
    String stringValue() const override
    {
        return this->p_musicalKey.stringValue();
    }

    Array<NotNull<const Common::Track*>> tracks() const override
    {
        return this->p_musicalKey.tracks();
    }
};

} }
