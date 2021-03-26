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

#include <RekordBuddyCollection/Crates/Folder.hpp>
#include <CommonCollection/Crates/CratePath.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Optional.hpp>
#include <Base/Pointers.hpp>
#include <Base/String.hpp>
#include <Base/Time.hpp>
#include <Base/Types.hpp>

namespace NxA { namespace RekordBuddy {

class MutableFolderClone : public MutableFolder
{
    // -- Friends
    friend class CollectionImplementation::Folder;
    friend class CollectionImplementation::Crate;
    friend class MutableFolder;

    // -- Private Instance Variables
    CollectionImplementation::CrateClone p_rekordBuddyCrateClone;

protected:
    // -- Protected Class Variables

    // -- This is used to make sure only friend or derived classes can call certain methods or constructors.
    // -- Making those methods or constructors protected or private would prevent things like Shared<> to
    // -- use them when being constructed themselves.
    constexpr inline static struct Protected { } p_isProtected = Protected{ };

    // -- Protected Instance Methods
    MutableArray<CollectionImplementation::Folder::MutableSubCrate>& p_ensureSubCratesAreLoaded() const override;

public:
    // -- Constructors & Destructors
    MutableFolderClone(CollectionImplementation::CrateClone&& from,
                       NotNull<MutableFolder*> parentFolder,
                       const Protected&) : MutableFolder{ std::move(get<CollectionImplementation::Folder>(from.cloneOf())),
                                                          parentFolder,
                                                          MutableFolder::p_isProtected },
                                           p_rekordBuddyCrateClone{ std::move(from) } { }
    ~MutableFolderClone() override = default;

    // -- Instance Methods
    CollectionImplementation::Folder& cloneOf()
    {
        return get<CollectionImplementation::Folder>(this->p_rekordBuddyCrateClone.cloneOf());
    }

    // -- Overridden MutableFolder Instance Methods
    NotNull<Common::MutableFolder*> cloneWithName(const String&) override;

    void setName(const String& name) override
    {
        this->p_rekordBuddyCrateClone.setName(name);
    }

    // -- Overridden Folder Instance Methods
    Time lastModificationTime() const override
    {
        return this->p_rekordBuddyCrateClone.lastModificationTime();
    }
    String name() const override
    {
        return this->p_rekordBuddyCrateClone.name();
    }
};

} }
