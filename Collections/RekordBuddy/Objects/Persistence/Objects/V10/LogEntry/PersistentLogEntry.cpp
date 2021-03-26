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

#include <RekordBuddyCollectionImplementationPersistence/V10/LogEntry/PersistentLogEntry.hpp>

#include <RekordBuddyCollectionImplementationPersistence/V10/Track/PersistentTrack.hpp>
using namespace NxA;

using namespace NxA::RekordBuddy::CollectionImplementation::V10;

using LocalType = PersistentLogEntry;

template class std::shared_ptr<PersistentLogEntry>;

template class NxA::Array<std::shared_ptr<PersistentLogEntry>>;
void PersistentLogEntry::bind(SourceBinder& parentBinder)
{
    PersistentObject::bind(parentBinder);
    parentBinder.bind(RBSchema::Type::LogEntry,
                      RBSchema::Type::PersistentObject,
                      RELATION(PersistentLogEntry::maybeRelatedToTrackOptionalId, maybeRelatedToTrack, Track, Unordered, ToOptionalOne, Nullify, logEntries, Unordered, ToMany),
                      ATTRIBUTE(PersistentLogEntry::idAttribute, id, Unindexed),
                      ATTRIBUTE(PersistentLogEntry::maybeMessageAttribute, maybeMessage, Unindexed),
                      ATTRIBUTE(PersistentLogEntry::timeStampAttribute, timeStamp, Unindexed));
}
void PersistentLogEntry::faultObject()
{
    if (isFaulted()) {
        return;
    }
    PersistentObject::faultObject();
    
}
void PersistentLogEntry::deleteObject()
{
    if (isDeleted()) {
        return;
    }
    PersistentObject::deleteObject();
    setMaybeRelatedToTrack({});
}

// -- id
integer16 PersistentLogEntry::id() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return idAttribute;
}
void PersistentLogEntry::setId(integer16 id)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, idAttribute, id);
}



// -- maybeMessage
Optional<String> PersistentLogEntry::maybeMessage() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return maybeMessageAttribute;
}
void PersistentLogEntry::setMaybeMessage(Optional<String> maybeMessage)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, maybeMessageAttribute, maybeMessage);
}



// -- timeStamp
Time PersistentLogEntry::timeStamp() const
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    return timeStampAttribute;
}
void PersistentLogEntry::setTimeStamp(Time timeStamp)
{
    auto contextLock = getContext()->ensureUnfaultedAndBorrowLock(objectId);
    getContext()->updateObject(objectId, timeStampAttribute, timeStamp);
}



// -- to-one relationship: maybeRelatedToTrack
Optional<RBSchema::ObjectID> PersistentLogEntry::maybeRelatedToTrackObjectID() const
{
    return maybeRelatedToTrackOptionalId;
}
Optional<std::shared_ptr<PersistentTrack>> PersistentLogEntry::maybeRelatedToTrack() const
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    if (!maybeRelatedToTrackOptionalId) {
        return {};
    }
    return {lockedContext->fetchObject<PersistentTrack>(*maybeRelatedToTrackOptionalId)};
}
void PersistentLogEntry::setMaybeRelatedToTrack(Optional<std::shared_ptr<PersistentTrack>> inverse)
{
    auto lockedContext = context.lock();
    auto contextLock = lockedContext->ensureUnfaultedAndBorrowLock(objectId);
    lockedContext->updateObject(objectId);
    if (this->maybeRelatedToTrackOptionalId) {
        auto fetchedObject = lockedContext->fetchObject<PersistentTrack>((*this->maybeRelatedToTrackOptionalId));
        if (fetchedObject) {
            (*fetchedObject)->manyLogEntries->removeObjectWithID(objectId);
        }
    }
    if (!inverse) {
        maybeRelatedToTrackOptionalId = nothing;
    }
    else {
        auto derel = *inverse;
        maybeRelatedToTrackOptionalId = derel->objectId;
        derel->manyLogEntries->append(sharedFromThis<PersistentLogEntry>());
    }
}


