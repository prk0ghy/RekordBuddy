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

#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentCueMarker.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Marker/PersistentMarker.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentCueMarker;

template class std::shared_ptr<PersistentCueMarker>;

template class NxA::Array<std::shared_ptr<PersistentCueMarker>>;
void PersistentCueMarker::bind(SourceBinder& parentBinder)
{
    PersistentMarker::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::CueMarker,
                      RBSchema::Type::Marker,
                      ATTRIBUTE(PersistentCueMarker::maybeHotCueNumberAttribute, maybeHotCueNumber, Unindexed),
                      ATTRIBUTE(PersistentCueMarker::maybeNameAttribute, maybeName, Unindexed),
                      ATTRIBUTE(PersistentCueMarker::maybeRgbColorAttribute, maybeRgbColor, Unindexed));
}
void PersistentCueMarker::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentMarker::faultObject();
}
void PersistentCueMarker::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentMarker::deleteObject();
}

// -- maybeHotCueNumber
Optional<integer16> PersistentCueMarker::maybeHotCueNumber() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeHotCueNumberAttribute;
}
void PersistentCueMarker::setMaybeHotCueNumber(Optional<integer16> maybeHotCueNumber)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeHotCueNumberAttribute, maybeHotCueNumber);
}



// -- maybeName
Optional<String> PersistentCueMarker::maybeName() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeNameAttribute;
}
void PersistentCueMarker::setMaybeName(Optional<String> maybeName)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeNameAttribute, maybeName);
}



// -- maybeRgbColor
Optional<integer32> PersistentCueMarker::maybeRgbColor() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeRgbColorAttribute;
}
void PersistentCueMarker::setMaybeRgbColor(Optional<integer32> maybeRgbColor)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeRgbColorAttribute, maybeRgbColor);
}


