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

#include <SeratoCollectionImplementation/Tags/BlobTag.hpp>
#include "Tags/Internal/UnknownTagInternal.hpp"

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

#define NXA_OBJECT_CLASS                            UnknownTag
#define NXA_OBJECT_BASE_CLASS                       Tag

#include <Base/ObjectDefinition.ipp>

// -- Factory Methods

UnknownTag UnknownTag::tagWithMemoryAt(NotNull<const byte*> tagAddress)
{
    auto newInternal = std::make_shared<Internal>();
    newInternal->identifier = Tag::identifierForTagAt(tagAddress);
    newInternal->value = Blob::withMemoryAndSize(Internal::dataForTagAt(tagAddress),
                                                     Tag::dataSizeForTagAt(tagAddress));

    return UnknownTag{ std::move(newInternal) };
}

// -- Constructors/Destructors

UnknownTag::UnknownTag() : Tag{ std::make_shared<Internal>() } { }
