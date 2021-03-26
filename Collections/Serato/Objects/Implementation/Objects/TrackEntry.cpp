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

#include <SeratoCollectionImplementation/TrackEntry.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>
#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/Tags/CrateV1Tags.hpp>

using namespace NxA;
using namespace NxA::Serato;
using namespace NxA::Serato::CollectionImplementation;

// -- Class Methods

Optional<FilePath> TrackEntry::maybeRelativePathForEntryWithTag(const ObjectTag& tag)
{
    auto maybeFilePathTag = tag.maybeSubTagForIdentifier(trackEntryPathTagIdentifier);
    if (!maybeFilePathTag.isValid()) {
        return nothing;
    }

    return static_cast<const PathTag&>(*maybeFilePathTag).value();
}

ObjectTag TrackEntry::tagForRelativeFilePath(const FilePath& relativePath)
{
    FilePath localRelativePath{relativePath.stripLeadingSeparators()};

    MutableArray<Tag> tags;
    tags.append(PathTag::tagWithIdentifierAndValue(trackEntryPathTagIdentifier, localRelativePath));

    return ObjectTag::tagWithIdentifierAndValue(trackEntryTagIdentifier, std::move(tags));
}
