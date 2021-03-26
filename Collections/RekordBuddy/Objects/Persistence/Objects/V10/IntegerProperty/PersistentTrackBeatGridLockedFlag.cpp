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

#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentTrackBeatGridLockedFlag.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/IntegerProperty/PersistentBooleanProperty.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTrackBeatGridLockedFlag;

template class std::shared_ptr<PersistentTrackBeatGridLockedFlag>;

template class NxA::Array<std::shared_ptr<PersistentTrackBeatGridLockedFlag>>;
void PersistentTrackBeatGridLockedFlag::bind(SourceBinder& parentBinder)
{
    PersistentBooleanProperty::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::TrackBeatGridLockedFlag, RBSchema::Type::BooleanProperty);
}
void PersistentTrackBeatGridLockedFlag::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentBooleanProperty::faultObject();
}
void PersistentTrackBeatGridLockedFlag::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentBooleanProperty::deleteObject();
}


