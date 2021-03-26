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

#include <SeratoCollectionImplementation/Tags/TagFactory.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include "Tags/Internal/ObjectTagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            ObjectTag
#define NXA_OBJECT_BASE_CLASS                       Tag

#include <Base/ObjectDefinition.ipp>

// -- Constructors/Destructors

ObjectTag::ObjectTag() : Tag{ std::make_shared<Internal>() } { }

// -- Factory Methods

ObjectTag ObjectTag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    auto dataSize = Tag::dataSizeForTagAt(tagAddress);
    auto tagData = Internal::dataForTagAt(tagAddress);
    auto subTags = TagFactory::parseTagsAt(tagData, dataSize);

    auto result = ObjectTag::tagWithIdentifierAndValue(Tag::identifierForTagAt(tagAddress), std::move(subTags));

    return result;
}

ObjectTag ObjectTag::tagWithIdentifierAndValue(uinteger32 identifier, Array<Tag> content)
{
    NXA_ASSERT_NOT_EQ(content.length(), 0);
    NXA_ASSERT_EQ((identifier & 0xFF000000) >> 24, 'o');

    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = identifier;

    for (auto&& subTag : content) {
        newInternal->subTagForIdentifier.setValueForKey(subTag, subTag.identifier());
    }

    return ObjectTag{ std::move(newInternal) };
}

// -- Instance Methods

Optional<Tag> ObjectTag::maybeSubTagForIdentifier(uinteger32 identifier) const
{
    return nxa_const_internal->subTagForIdentifier.maybeValueForKey(identifier);
}

void ObjectTag::setSubTag(Tag tag)
{
    nxa_internal->subTagForIdentifier.setValueForKey(tag, tag.identifier());
}

count ObjectTag::numberOfSubTags() const
{
    return nxa_const_internal->subTagForIdentifier.length();
}

void ObjectTag::addInSeratoTrackOrderTo(MutableBlob& destination) const
{
    std::vector<uinteger32> identifierOrder = {
        'ttyp', 'pfil', 'tsng', 'tart', 'talb', 'tgen', 'tlen', 'tsiz', 'tbit', 'tsmp', 'tbpm', 'tcom', 'tgrp',
        'trmx', 'tlbl', 'tcmp', 'ttyr', 'tadd', 'tkey', 'uadd', 'utkn', 'ulbl', 'utme', 'utpc', 'ufsb', 'udsc',
        'sbav', 'bhrt', 'bmis', 'bply', 'blop', 'bitu', 'bovc', 'bcrt', 'biro', 'bwlb', 'bwll', 'buns', 'bbgl', 'bkrk'
    };

    auto internal = nxa_const_internal;

    MutableArray<uinteger32> identifiersToOutput;
    for (auto&& identifierAndTag : internal->subTagForIdentifier) {
        identifiersToOutput.append(identifierAndTag.first);
    }

    MutableBlob subTagsRepresentation;
    for (auto&& identifier : identifierOrder) {
        auto maybeSubTag = this->maybeSubTagForIdentifier(identifier);
        if (maybeSubTag.isValid()) {
            maybeSubTag->addTo(subTagsRepresentation);
            identifiersToOutput.remove(identifier);
        }
    }

    for (auto&& identifier : identifiersToOutput) {
        internal->subTagForIdentifier[identifier].addTo(subTagsRepresentation);
    }

    auto sizeNeededForHeader = Internal::memoryNeededForTagHeader();
    auto headerRepresentation = MutableBlob::withCapacity(sizeNeededForHeader);
    auto tagAddress = headerRepresentation.data();
    Internal::setIdentifierForTagAt(this->identifier(), tagAddress);
    Internal::setDataSizeForTagAt(subTagsRepresentation.size(), tagAddress);

    destination.append(std::move(headerRepresentation));
    destination.append(std::move(subTagsRepresentation));
}
