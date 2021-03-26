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

#include <RekordBuddyCollection/Collection.hpp>

#include <Base/Test.hpp>

#if !defined(NXA_BUILD_FOR_TESTING)
#error "Can't build tests without NXA_BUILD_FOR_TESTING"
#endif

namespace NxA { namespace RekordBuddy {

// -- Class
class Test : public NxA::Test
{
public:
    // -- Class Methods
    static Shared<RekordBuddy::MutableCollection> mutableRekordBuddyCollectionAt(const FilePath& path)
    {
        auto collection = Shared<RekordBuddy::MutableCollection>::with(path, RekordBuddy::MutableCollection::p_isProtected);
        collection->p_testIsAUnitTestCollection = true;
        return collection;
    }

    // -- Constructors & Destructors
    Test()
    {
        // -- We setup our threading glue code to assume we're always on the main thread.
        Threading::setPassThruMethodsForAll();
    }
};

} }
