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
#include <RekordBuddyCollectionImplementationPersistence/RekordBuddySchema.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/AllPersistentObjects.ipp>
#include <RekordBuddyCollectionImplementationPersistence/V10/MetaMacroAllTypes.ipp>

template class NxA::PersistentContext<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
template struct NxA::PersistentObjectID<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
template struct NxA::GenericPersistentObject<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
template struct NxA::SqlLiteBinder<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
template struct NxA::SqlLiteStore<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;

#if defined(NXA_GENERIC_SOMESOURCE)
template struct NxA::ContextBinder<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
#endif

#if defined(NXA_GENERIC_SOMESOURCE)
template struct NxA::SomeSourceBinder<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>;
#endif

template class NxA::Variant<std::shared_ptr<typename NxA::RekordBuddy::CollectionImplementation::V10::RBSchema::Store>,
                            std::shared_ptr<NxA::PersistentContext<NxA::RekordBuddy::CollectionImplementation::V10::RBSchema>>>;

namespace NxA::RekordBuddy::CollectionImplementation {

inline namespace V10 {

constexpr std::tuple<RBSchema::SourceVersion, RBSchema::DestinationVersion, RBSchema::MigrateFunctionT*> RBSchema::schemaMigrations[];

void RBSchema::emptyDatabaseToCurrentMigration(const std::shared_ptr<NxA::SqlLiteStore<RBSchema>>& store)
{
    // -- This is called when the database is first initialized (current version is 0).
    applyToTypes([&](auto&& objectType) {
        SomeSourceBinder<RBSchema> binder{ *store, Mode::Declare, objectType.objectID() };
        objectType.bind(binder);
    });
}

void RBSchema::version9ToCurrentMigration(const std::shared_ptr<NxA::SqlLiteStore<RBSchema>>& store)
{
    // -- This fixes an issue caused by deleted persistent objects only deleting their entry in the PersistentObject table
    // -- and not in any tables from objects inheriting from PersistentObject. This not only caused an SQL 'leak' in terms
    // -- of the database getting larger and larger, but also could cause a collision when new object where created reusing
    // -- an object index of a previously deleted object. Now we had an object index used by two types, one invalid.
    // -- This scans the database for all objects and makes sure that the PersistentObject entry for it exists and is of
    // -- the right type. If not, we have found a corruption caused by this bug and we delete it.
    // -- There are no schema change between V9 and V10 so I don't think we have to run through the database with a Declare bind.
    MutableMap<PersistentObjectID<RBSchema>::Index, RBSchema::Type> availableObjectsIndicesToTypes;
    for (auto&& objectID : store->fetchAllObjectIDs()) {
        NXA_DLOG_WITH_FORMAT("Index %llu is type %s.", objectID.objectIndex(), typeToString(objectID.objectType()));
        availableObjectsIndicesToTypes.setValueForKey(objectID.objectType(), objectID.objectIndex());
    }

    MutableArray<PersistentObjectID<RBSchema>> objectsToDelete;
    applyToTypes([&store, &availableObjectsIndicesToTypes, &objectsToDelete](auto&& objectType) {
        auto type = objectType.objectID().objectType();
        auto objectsIndicesFound = store->fetchObjectIndicesOfType(type);
        if (objectsIndicesFound.length()) {
            NXA_DLOG_WITH_FORMAT("Found %llu objects of type type %s.", objectsIndicesFound.length(), typeToString(type));
        }

        for (auto&& objectIndex : objectsIndicesFound) {
            auto maybeExistingType = availableObjectsIndicesToTypes.maybeValueForKey(objectIndex);
            // -- If this object doesn't have an existing type then it means its PersistentObject entry was deleted without the rest
            // -- of its child types, which is cause by a bug in the V9 format.
            // -- If the existing type is not the same that means the above took place and then the index was re-used to create an
            // -- object of a different type. But we to have to check that the existing type not related to the base type, in which
            // -- case this may mean we are looking at an object in the middle of the hierarchy and that is ok.
            if (!maybeExistingType.isValid() || ((*maybeExistingType != type) && !typeIs(*maybeExistingType, type))) {
                NXA_DLOG_WITH_FORMAT("Deleting index %lld type %s.", objectIndex, typeToString(type));
                objectsToDelete.append({ objectIndex, type });
            }
        }
    });

    MutableArray<PersistentObjectID<RBSchema>> newObjectsToDelete;

    while(objectsToDelete.length() != 0) {
        for (auto&& objectIDToDelete : objectsToDelete) {
            applyToTypes([&store, &newObjectsToDelete, &objectIDToDelete](auto&& objectType) {
                if (objectType.objectID().objectType() == objectIDToDelete.objectType()) {
                    SomeSourceBinder<RBSchema> binder{ *store, Mode::Delete, objectIDToDelete };
                    objectType.bind(binder);

                    newObjectsToDelete.appendObjectsFrom(binder.objectIDsToDeleteFollowingBindDelete());
                }
            });
        }

        objectsToDelete = newObjectsToDelete;
        newObjectsToDelete.removeAll();
    }
}

Optional<std::shared_ptr<GenericPersistentObject<RBSchema>>> RBSchema::constructDynamic(const ObjectID& id, std::shared_ptr<PersistentContext<RBSchema>> c)
{
    switch (id.objectType()) {
        ALL_TYPES(CONSTRUCT_DYNAMIC)
        default:
        return nothing;
    }
}

} }
