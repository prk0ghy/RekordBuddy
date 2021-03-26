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

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/TrackProperties/TrackProperties.hpp>
#include <RekordBuddyCollection/Implementation/Tags/Properties/Internal/StringPropertyInternal.hpp>

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>
#include <TestRekordBuddyCollectionImplementation/TrackHelpers.hpp>

#include <Base/Test.hpp>

using namespace testing;

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

class StringPropertyTests : public CollectionTestsBase
{

};

TEST_F(StringPropertyTests, isAnOrphan_AnOrphanTag_ReturnsTrue)
{
    // -- Given.
    StringProperty tag = TrackTag::withValueInCollection({ "10A" }, this->collection().asReference());

    // -- When.
    // -- Then.
    EXPECT_TRUE(tag.isAnOrphan());
}

TEST_F(StringPropertyTests, isAnOrphan_AnNonOrphanTag_ReturnsFalse)
{
    // -- Given.
    this->setTestCurrentTime("2014-07-23 07:00:15");
    auto& collection = this->collection().asReference();
    StringProperty tag = TrackTag::withValueInCollection({ "10A" }, collection);
    auto track = emptyTrackInCollection(collection);
    track->setTags({ "10A" });
    track->markAsModifiedNow();

    // -- When.
    // -- Then.
    EXPECT_FALSE(tag.isAnOrphan());
}

} } }
