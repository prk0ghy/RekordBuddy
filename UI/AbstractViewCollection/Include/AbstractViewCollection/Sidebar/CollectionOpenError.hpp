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

#include <CommonCollection/Collection.hpp>

#include <Base/Volume.hpp>
#include <Base/String.hpp>

#include <QVariant>

namespace NxA { namespace RekordBuddy {

class CollectionOpenError
{
public:
    // -- Types
    enum class Type {
        IgnoreCollection,
        ResetCollection,
        InvalidLicence
    };

private:
    // -- Private Instance Variables
    Type p_errorType;

    Common::Collection::Type p_collectionType;

    String p_collectionTypeName;

    Volume p_volume;

public:
    // -- Constructors & Destructors
    CollectionOpenError() = default;
    CollectionOpenError(Type errorType,
                        Common::Collection::Type collectionType,
                        const String& collectionTypeName,
                        const Volume& volume) : p_errorType{ errorType },
                                                p_collectionType{ collectionType },
                                                p_collectionTypeName{ collectionTypeName },
                                                p_volume{ volume } { }

    // -- Instance Methods
    inline Type errorType()
    {
        return this->p_errorType;
    }
    inline Common::Collection::Type collectionType()
    {
        return this->p_collectionType;
    }
    inline String collectionTypeName()
    {
        return this->p_collectionTypeName;
    }
    inline const Volume& volume()
    {
        return this->p_volume;
    }
};

} }

// -- Allow this type to be passed as User Data in Qt operations
Q_DECLARE_METATYPE(NxA::RekordBuddy::CollectionOpenError);
