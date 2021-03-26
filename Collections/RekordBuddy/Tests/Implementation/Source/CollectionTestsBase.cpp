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

#include <TestRekordBuddyCollectionImplementation/CollectionTestsBase.hpp>

#include <RekordBuddyCollection/Crates/Crates.hpp>

#include "../../Test.hpp"
#include "../../UserPreferencesTests.hpp"

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Instance Methods

Pointer<Collection> CollectionTestsBase::collection()
{
    if (!this->p_maybeCollection.isValid()) {
        RekordBuddy::UserPreferencesTests::setUpTestUserPreferencesForVolumeAndTestClass(Volume::musicFolderVolume(), *this);

        this->p_maybeCollectionPath = FilePath::filePathByJoiningPaths(Directory::temporaryDirectory(), NXA_FILEPATH("collection.db"));
        File::deleteFileAt(*this->p_maybeCollectionPath);

        this->p_maybeCollection = RekordBuddy::Test::mutableRekordBuddyCollectionAt(*this->p_maybeCollectionPath);
        auto maybeError = (*this->p_maybeCollection)->open();
        NXA_ASSERT_FALSE(maybeError.isValid());
    }

    return (*this->p_maybeCollection)->asRekordBuddyCollection();
}
