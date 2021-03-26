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

#include <RekordBuddyCollection/Tracks/Track.hpp>
#include <RekordBuddyCollection/Tracks/Tag.hpp>

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTag.hpp>

#include <CommonCollection/Tracks/Tag.hpp>

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations

class MutableCollection;

// -- Public Interface
class Tag : public Common::Tag
{
    // -- Friends
    friend class MutableTag;
    friend class MutableCollection;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Variables
    CollectionImplementation::TrackTag p_rekordBuddyTrackTag;

    // -- Protected Instance Methods
    Pointer<const RekordBuddy::MutableCollection> p_rekordBuddyCollection() const;
    Pointer<RekordBuddy::MutableCollection> p_rekordBuddyCollection();

public:
    // -- Constructors & Destructors
    Tag(CollectionImplementation::TrackTag&& from, const Protected&) : p_rekordBuddyTrackTag{ std::move(from) } { }
    ~Tag() override = default;

    // -- Instance Methods
    String propertyDescription() const
    {
        return this->p_rekordBuddyTrackTag.propertyDescription();
    }
    boolean hasChildrenTags() const
    {
        return this->p_rekordBuddyTrackTag.childrenTags().length() != 0;
    }

    // -- Overridden Common::Tag Instance Methods
    NotNull<const Common::Collection*> collection() const override;

    String value() const override
    {
        return this->p_rekordBuddyTrackTag.propertyDescription();
    }

    Array<Unique<Common::Tag>> children() const override
    {
        // -- Const cast is ok here because we are returning immutable Tags anyway.
        auto rekordBuddyTag = const_cast<Tag*>(this)->p_rekordBuddyTrackTag;
        if (!rekordBuddyTag.hasChildrenTags()) {
            return { };
        }

        MutableArray<Unique<Common::Tag>> results;

        for (auto&& childTag : rekordBuddyTag.childrenTags()) {
            results.append(Unique<RekordBuddy::Tag>::with(std::move(childTag), Tag::p_isProtected));
        }

        return std::move(results);
    }

    Array<NotNull<const Common::Track*>> tracks() const override;
};

class MutableTag final : public Common::MutableTag, private Tag
{
    // -- Friends
    friend class MutableCollection;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

public:
    // -- Constructors & Destructors
    MutableTag(CollectionImplementation::TrackTag&& from,
               const Protected&) : Tag{ std::move(from), Tag::p_isProtected } { }
    ~MutableTag() override = default;

    // -- Instance Methods
    inline String propertyDescription() const
    {
        return this->Tag::propertyDescription();
    }
    inline boolean hasChildrenTags() const
    {
        return this->Tag::hasChildrenTags();
    }

    // -- Overridden Common::MutableTag Instance Methods
    NotNull<const Common::Collection*> collection() const override;
    NotNull<Common::MutableCollection*> collection() override;

    inline String value() const override
    {
        return this->p_rekordBuddyTrackTag.propertyDescription();
    }

    inline Array<Unique<Common::Tag>> children() const override
    {
        return this->Tag::children();
    }
    Array<Unique<Common::MutableTag>> children() override
    {
        auto& rekordBuddyTag = this->p_rekordBuddyTrackTag;
        if (!rekordBuddyTag.hasChildrenTags()) {
            return { };
        }

        MutableArray<Unique<Common::MutableTag>> results;

        for (auto&& childTag : rekordBuddyTag.childrenTags()) {
            results.append(Unique<RekordBuddy::MutableTag>::with(std::move(childTag),
                                                                 RekordBuddy::MutableTag::p_isProtected));
        }

        return std::move(results);
    }

    inline Array<NotNull<const Common::Track*>> tracks() const override
    {
        return this->Tag::tracks();
    }
    Array<NotNull<Common::MutableTrack*>> tracks() override;

    const Common::Tag& asImmutableReference() const override
    {
        return *this;
    }
};

} }
