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
#include <Persistence/GenericPersistentObject.hpp>
#include <RekordBuddyCollectionImplementationPersistence/V10/Crate/PersistentFilterPlaylist.hpp>
#pragma once
namespace NxA::RekordBuddy::CollectionImplementation {
inline namespace V10 {
class PersistentSortingPlaylist : public PersistentFilterPlaylist
{
    using LocalType = PersistentSortingPlaylist;
    friend class PersistentDecimalProperty;
    Optional<RBSchema::ObjectID> maybeDecimalPropertyUsedToSortByOptionalId;
    friend class PersistentIntegerProperty;
    Optional<RBSchema::ObjectID> maybeIntegerPropertyUsedToSortByOptionalId;
    friend class PersistentSortingCriteria;
    RBSchema::ObjectID criteriaId;
    friend class PersistentStringProperty;
    Optional<RBSchema::ObjectID> maybeStringPropertyUsedToSortByOptionalId;
    friend class PersistentTimeProperty;
    Optional<RBSchema::ObjectID> maybeTimePropertyUsedToSortByOptionalId;
    // -- Constructors, Destructors, Friends, Move and Copy
    
protected:
    friend struct RBSchema;
    PersistentSortingPlaylist(RBSchema::Type type, Context context)
        : PersistentSortingPlaylist{ObjectID{type}, std::move(context)}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(type, TypeEnumeration<PersistentSortingPlaylist>::value));}
    
public:
    PersistentSortingPlaylist(ObjectID id, Context context)
        : PersistentFilterPlaylist{id, context},
          maybeDecimalPropertyUsedToSortByOptionalId{},
          maybeIntegerPropertyUsedToSortByOptionalId{},
          criteriaId{},
          maybeStringPropertyUsedToSortByOptionalId{},
          maybeTimePropertyUsedToSortByOptionalId{}
    {NXA_ASSERT_TRUE(RBSchema::typeIs(id.objectType(), TypeEnumeration<PersistentSortingPlaylist>::value));}
    ~PersistentSortingPlaylist() override = default;
    PersistentSortingPlaylist(const PersistentSortingPlaylist&) = delete;
    PersistentSortingPlaylist(PersistentSortingPlaylist&&) = default;
    PersistentSortingPlaylist& operator=(const PersistentSortingPlaylist&) = delete;
    
    
    void deleteObject() override;
    void faultObject() override;
    // -- Attributes & Relationships
    // -- maybeDecimalPropertyUsedToSortBy
    Optional<RBSchema::ObjectID> maybeDecimalPropertyUsedToSortByObjectID() const;
    Optional<std::shared_ptr<PersistentDecimalProperty>> maybeDecimalPropertyUsedToSortBy() const;
    void setMaybeDecimalPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentDecimalProperty>>);
    // -- maybeIntegerPropertyUsedToSortBy
    Optional<RBSchema::ObjectID> maybeIntegerPropertyUsedToSortByObjectID() const;
    Optional<std::shared_ptr<PersistentIntegerProperty>> maybeIntegerPropertyUsedToSortBy() const;
    void setMaybeIntegerPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentIntegerProperty>>);
    // -- criteria
    RBSchema::ObjectID criteriaObjectID() const;
    std::shared_ptr<PersistentSortingCriteria> criteria() const;
    void setCriteria(std::shared_ptr<PersistentSortingCriteria>);
    // -- maybeStringPropertyUsedToSortBy
    Optional<RBSchema::ObjectID> maybeStringPropertyUsedToSortByObjectID() const;
    Optional<std::shared_ptr<PersistentStringProperty>> maybeStringPropertyUsedToSortBy() const;
    void setMaybeStringPropertyUsedToSortBy(Optional<std::shared_ptr<PersistentStringProperty>>);
    // -- maybeTimePropertyUsedToSortBy
    Optional<RBSchema::ObjectID> maybeTimePropertyUsedToSortByObjectID() const;
    Optional<std::shared_ptr<PersistentTimeProperty>> maybeTimePropertyUsedToSortBy() const;
    void setMaybeTimePropertyUsedToSortBy(Optional<std::shared_ptr<PersistentTimeProperty>>);
    
    
    // -- Serialization
    void bind(SourceBinder &parentBinder) override;
    
    
};
}
}
