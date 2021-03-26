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

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentTrackAlbum.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/StringProperty/PersistentStringProperty.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentTrackAlbum;

template class std::shared_ptr<PersistentTrackAlbum>;

template class NxA::Array<std::shared_ptr<PersistentTrackAlbum>>;
void PersistentTrackAlbum::bind(SourceBinder& parentBinder)
{
    PersistentStringProperty::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::TrackAlbum,
                      RBSchema::Type::StringProperty,
                      ATTRIBUTE(PersistentTrackAlbum::maybeNumberOfTracksAttribute, maybeNumberOfTracks, Unindexed));
}
void PersistentTrackAlbum::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentStringProperty::faultObject();
}
void PersistentTrackAlbum::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentStringProperty::deleteObject();
}

// -- maybeNumberOfTracks
Optional<integer16> PersistentTrackAlbum::maybeNumberOfTracks() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeNumberOfTracksAttribute;
}
void PersistentTrackAlbum::setMaybeNumberOfTracks(Optional<integer16> maybeNumberOfTracks)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeNumberOfTracksAttribute, maybeNumberOfTracks);
}


