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

#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackTag.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/StringPropertyInternal.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Operator Specialization

namespace NxA {

inline constexpr bool operator==(const PersistentTrackTag& first, const PersistentTrackTag& second) noexcept
{
    return first.objectID() == second.objectID();
}

inline constexpr bool operator<(const PersistentTrackTag& first, const PersistentTrackTag& second) noexcept
{
    return first.objectID() < second.objectID();
}

}

// -- Implementation

#define NXA_OBJECT_CLASS                                        TrackTag
#define NXA_OBJECT_BASE_CLASS                                   StringProperty
#define NXA_OBJECT_INTERNAL_CLASS_IS_PERSISTENT_IN_NAMESPACE    NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION

#include <Base/ObjectDefinition.ipp>

// -- Class Methods

Array<TrackTag> TrackTag::parentTrackTagsFoundInCollection(Collection& collection)
{
    MutableArray<TrackTag> results;

    for (auto persistentTrackTag : collection.persistentContext()->fetchObjects<RekordBuddy::CollectionImplementation::PersistentTrackTag>()) {
        // -- We only want 'parent' tags, as in the ones that have no parent themselves.
        if (!persistentTrackTag->maybeParentTagObjectID().isValid()) {
            results.append(TrackTag{ std::move(persistentTrackTag) });
        }
    }

    return std::move(results);
}

// -- Factory Methods

TrackTag TrackTag::withValueInCollection(const String& value, Collection& collection)
{
    auto components = value.splitBySeparator(':');
    if (components.length() > 2) {
        // -- For now we only support tags with only two components, ie: Mytag:SugTag
        components = components.subArrayWithRange(0, 1);
    }

    MutableString recomposedValue;
    Optional<std::shared_ptr<PersistentTrackTag>> lastTagCreated;

    for (auto&& component : components) {
        if (!recomposedValue.isEmpty()) {
            recomposedValue.append(":");
        }

        recomposedValue.append(component);

        auto newTag = StringPropertyInternal::stringPropertyWithValueInCollection<PersistentTrackTag>(recomposedValue, collection);

        if (lastTagCreated.isValid()) {
            newTag->setMaybeParentTag(lastTagCreated);
        }

        lastTagCreated = newTag;
    }

    NXA_ASSERT_TRUE(lastTagCreated.isValid());

    return TrackTag{ *lastTagCreated };
}

Array<TrackTag> TrackTag::tagsInCollection(Collection& collection)
{
    auto context = collection.persistentContext();
    auto trackTags = context->fetchObjects<PersistentTrackTag>();

    MutableArray<TrackTag> result;
    for (auto trackTag : trackTags) {
        result.append(TrackTag{ std::move(trackTag) });
    }

    result.sortWith([](auto&& a, auto&& b) {
        return a.stringValue() < b.stringValue();
    });

    return std::move(result);
}

// -- Instance Methods

String TrackTag::value() const
{
    return nxa_const_internal->stringValue();
}

Optional<Const<TrackTag>> TrackTag::maybeParentTag() const
{
    return nxa_const_internal->maybeParentTag().maybe([](auto&& object) {
        return Const<TrackTag>{ TrackTag(object) };
    });
}

Optional<TrackTag> TrackTag::maybeParentTag()
{
    return nxa_const_internal->maybeParentTag().maybe([](auto&& object) {
        return TrackTag(object);
    });
}

boolean TrackTag::hasChildrenTags() const
{
    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& childrenTags = nxa_internal->childrenTags();
    return childrenTags.length() != 0;
}

Array<Const<TrackTag>> TrackTag::childrenTags() const
{
    MutableArray<Const<TrackTag>> results;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& childrenTags = nxa_internal->childrenTags();
    for (auto&& tag : childrenTags) {
        results.append({ TrackTag(tag) });
    }

    return std::move(results);
}

Array<TrackTag> TrackTag::childrenTags()
{
    MutableArray<TrackTag> results;
    for (auto&& tag : nxa_internal->childrenTags()) {
        results.append(TrackTag(tag));
    }

    return std::move(results);
}

String TrackTag::propertyDescription() const
{
    // -- The result of this method is stored in user data and should not be modified.
    auto components = nxa_const_internal->stringValue().splitBySeparator(':');
    auto numberOfComponents = components.length();
    NXA_ASSERT_TRUE((numberOfComponents > 0) && (numberOfComponents <= 2));

    return components[numberOfComponents - 1];
}

Array<ObjectID> TrackTag::trackObjectIDs() const
{
    if (!this->hasChildrenTags()) {
        return this->parentObjectIDs();
    }

    MutableSet<ObjectID> results;

    // -- We are using nxa_internal instead of nxa_const_internal because Persistence does not have const version of the relationship.
    // -- We make sure our reference is const though in the end to avoid mistakes.
    const auto& childrenTags = nxa_internal->childrenTags();
    for (auto&& tag : childrenTags) {
        results.add(TrackTag(tag).trackObjectIDs());
    }

    return { std::move(results) };
}
