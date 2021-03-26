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

#include <RekordBuddyCollectionImplementationPersistence/Version.hpp>

#define NXA_PERSISTENCE_QUOTE_ME(M)                             #M
#define NXA_PERSISTENCE_CURRENT_SCHEMA_INCLUDE(V, M)            NXA_PERSISTENCE_QUOTE_ME(RekordBuddyCollectionImplementationPersistence/V/M)

#pragma once

#include <Persistence/Persistence.hpp>

#include <Base/Base.hpp>

#include <memory>

namespace NxA { namespace RekordBuddy { namespace CollectionImplementation {

// -- inline namespace should be last
inline namespace V10 {

#include NXA_PERSISTENCE_CURRENT_SCHEMA_INCLUDE(NXA_PERSISTENCE_CURRENT_REKORD_BUDDY_SCHEMA_VERSION, Version.ipp)
#include "RekordBuddyCollectionImplementationPersistence/SchemaCommon.ipp"

}

} } }

extern template class NxA::PersistentContext<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
extern template struct NxA::PersistentObjectID<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
extern template struct NxA::GenericPersistentObject<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
extern template struct NxA::SqlLiteBinder<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
extern template struct NxA::SqlLiteStore<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
extern template class NxA::Variant<std::shared_ptr<typename NxA::RekordBuddy::CollectionImplementation::RBSchema::Store>,
                                   std::shared_ptr<NxA::PersistentContext<NxA::RekordBuddy::CollectionImplementation::RBSchema>>>;

#if defined(NXA_GENERIC_SOMESOURCE)
    extern template struct NxA::ContextBinder<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
    extern template struct NxA::SomeSourceBinder<NxA::RekordBuddy::CollectionImplementation::RBSchema>;
#endif
