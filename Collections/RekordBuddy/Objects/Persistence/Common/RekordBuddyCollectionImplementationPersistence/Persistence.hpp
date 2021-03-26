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

#include <RekordBuddyCollectionImplementationPersistence/Version.hpp>

// -- Included in this file will be the complete declarations for all objects, only useful in some cases. It really exists to restrict where version has to be updated. Including this file will have heavy compile-time implications so limit to places that need access to all types in the schema.
#include <RekordBuddyCollectionImplementationPersistence/RekordBuddySchema.hpp>

#include NXA_PERSISTENCE_CURRENT_SCHEMA_INCLUDE(NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION, AllPersistentObjects.ipp)
#include NXA_PERSISTENCE_CURRENT_SCHEMA_INCLUDE(NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION, MetaMacroAllTypes.ipp)

// -- At this point we can use all the types concretely.
