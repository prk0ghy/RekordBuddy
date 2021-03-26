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
#include <RekordBuddyCollection/RekordBuddy.hpp>
#include <RekordBuddyCollection/UserPreferences.hpp>

#include <RekordBuddyCollection/../../Tests/Test.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>

#include <Base/Test.hpp>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- Public Interface
class CollectionTestsBase : public NxA::RekordBuddy::Test
{
    Optional<Shared<RekordBuddy::MutableCollection>> p_maybeCollection;
    Optional<FilePath> p_maybeCollectionPath;

public:
    // -- Constructors & Destructors
    CollectionTestsBase() { }
    ~CollectionTestsBase()
    {
        if (this->p_maybeCollection.isValid()) {
            this->p_maybeCollection = nothing;
        }

        File::deleteFileAt(*this->p_maybeCollectionPath);
    }

    // -- Instance Methods
    Pointer<Collection> collection();
};

} } }
