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

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrateClone.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentCrate.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentCrateClone;

template class NxA::Array<std::shared_ptr<PersistentCrateClone>>;
void PersistentCrateClone::bind(SourceBinder& parentBinder)
{
    PersistentCrate::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::CrateClone,
                      RBSchema::Type::Crate,
                      RELATION(PersistentCrateClone::cloneOfId, cloneOf, Crate, Unordered, ToOne, Nullify, clones, Unordered, ToMany));
}
void PersistentCrateClone::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentCrate::faultObject();

}
void PersistentCrateClone::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentCrate::deleteObject();
    setCloneOf({});
}



// -- to-one relationship: cloneOf
RBSchema::ObjectID PersistentCrateClone::cloneOfObjectID() const
{
    return cloneOfId;
}
std::shared_ptr<PersistentCrate> PersistentCrateClone::cloneOf() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!cloneOfId.isValid()) {
        return {};
    }
    auto fetchedObject = lockedContext->fetchObject<PersistentCrate>(cloneOfId);
    if (!fetchedObject) {
        return {};
    }
    return *fetchedObject;
}
void PersistentCrateClone::setCloneOf(std::shared_ptr<PersistentCrate> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    auto fetchedObject = lockedContext->fetchObject<PersistentCrate>(this->cloneOfId);
    if (fetchedObject) {
        (*fetchedObject)->manyClones->removeObjectWithID(objectId);
    }
    if (!inverse) {
        cloneOfId = {};
    }
    else {
        cloneOfId = inverse->objectId;
        inverse->manyClones->append(sharedFromThis<PersistentCrateClone>());
    }
}
