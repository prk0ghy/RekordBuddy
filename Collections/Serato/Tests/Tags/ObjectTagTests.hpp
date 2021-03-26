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

#include <SeratoCollectionImplementation/Tags/ObjectTag.hpp>
#include <SeratoCollectionImplementation/Tags/BlobTag.hpp>
#include <SeratoCollectionImplementation/Tags/BooleanTag.hpp>
#include <SeratoCollectionImplementation/Tags/PathTag.hpp>
#include <SeratoCollectionImplementation/Tags/TextTag.hpp>
#include <SeratoCollectionImplementation/Tags/UInteger16Tag.hpp>
#include <SeratoCollectionImplementation/Tags/UInteger32Tag.hpp>
#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>

#include <Base/FilePath.hpp>
#include <Base/Test.hpp>

namespace NxA { namespace Serato { namespace CollectionImplementation {

class ObjectTagTests : public NxA::Test
{
public:
    // -- Class Variables
    static byte objectTagTestsTestData[];
    static Blob objectTagTestsTestBlob;
    static String objectTagTestsTestText;
    static String objectTagTestsTestVersion;

    // -- Class Methods
    static FilePath objectTagTestsTestPath()
    {
        return NXA_FILEPATH("Some/Cool/path.mp3");
    }

    static ObjectTag testObjectTag()
    {
        Array<Tag> tags{ BlobTag::tagWithIdentifierAndValue('atst', ObjectTagTests::objectTagTestsTestBlob),
                         BooleanTag::tagWithIdentifierAndValue('btst', true),
                         PathTag::tagWithIdentifierAndValue('ptst', ObjectTagTests::objectTagTestsTestPath()),
                         TextTag::tagWithIdentifierAndValue('ttst', ObjectTagTests::objectTagTestsTestText),
                         UInteger16Tag::tagWithIdentifierAndValue('stst', 0xBEEF),
                         UInteger32Tag::tagWithIdentifierAndValue('utst', 0xDEADBEEF),
                         VersionTag::tagWithIdentifierAndValue('vtst', ObjectTagTests::objectTagTestsTestVersion) };

        return ObjectTag::tagWithIdentifierAndValue('otst', std::move(tags));
    }
};

} } }
