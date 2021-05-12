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

#include <RekordBuddyCollection/Implementation/Tracks/Property.hpp>

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Class Methods

// -- These are temporary and will be removed when the use of the persistent classes are fully encapsulated everywhere in the code.
Common::Property::TypeID Property::propertyTypeIDForPersistentObjectType(RekordBuddy::CollectionImplementation::RBSchema::Type persistentObjectType)
{
    static const Map<RekordBuddy::CollectionImplementation::RBSchema::Type, Common::Property::TypeID> convertionTable = {
        #define NXA_PERSISTENT_TO_TAG_TYPE(type)        { RekordBuddy::CollectionImplementation::RBSchema::Type::Track ## type, Common::Property::TypeID::type }

        NXA_PERSISTENT_TO_TAG_TYPE(Album),
        NXA_PERSISTENT_TO_TAG_TYPE(BeatGridLockedFlag),
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBitDepth, Common::Property::TypeID::BitDepthInBits },
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBitRate, Common::Property::TypeID::BitRateInKiloBitsPerSecond },
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBpm, Common::Property::TypeID::BeatsPerMinute },
        NXA_PERSISTENT_TO_TAG_TYPE(Color),
        NXA_PERSISTENT_TO_TAG_TYPE(Comments),
        NXA_PERSISTENT_TO_TAG_TYPE(DateAdded),
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackModificationTime, Common::Property::TypeID::LastModifiedOn },
        NXA_PERSISTENT_TO_TAG_TYPE(DateReleased),
        NXA_PERSISTENT_TO_TAG_TYPE(FileSize),
        NXA_PERSISTENT_TO_TAG_TYPE(FileType),
        NXA_PERSISTENT_TO_TAG_TYPE(Genre),
        NXA_PERSISTENT_TO_TAG_TYPE(Grouping),
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackKey, Common::Property::TypeID::MusicalKey },
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackLength, Common::Property::TypeID::LengthInSeconds },
        NXA_PERSISTENT_TO_TAG_TYPE(MixName),
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackNumber, Common::Property::TypeID::TrackNumber },
        NXA_PERSISTENT_TO_TAG_TYPE(DiscNumber),
        NXA_PERSISTENT_TO_TAG_TYPE(PlayCount),
        NXA_PERSISTENT_TO_TAG_TYPE(Rating),
        NXA_PERSISTENT_TO_TAG_TYPE(RecordLabel),
        { RekordBuddy::CollectionImplementation::RBSchema::Type::TrackSampleRate, Common::Property::TypeID::SampleRateInHertz },
        NXA_PERSISTENT_TO_TAG_TYPE(Tag),
        NXA_PERSISTENT_TO_TAG_TYPE(Title),

        #undef NXA_PERSISTENT_TO_TAG_TYPE
    };

    auto result = convertionTable.maybeValueForKey(persistentObjectType);
#if defined(NXA_BUGSNAG_APP_ID)
    NXA_ASSERT_TRUE_WITH_BLOCK(result.isValid(), [&persistentObjectType]{
        CrashLog::addUserInfoWithKey(String::stringWithFormat("%d", static_cast<integer>(persistentObjectType)), "persistentObjectType");
    });
#endif

    return *result;
}

RekordBuddy::CollectionImplementation::RBSchema::Type Property::persistentObjectTypeForPropertyTypeID(Common::Property::TypeID type)
{
    static const Map<Common::Property::TypeID, RekordBuddy::CollectionImplementation::RBSchema::Type> convertionTable = {
        #define NXA_TAG_TYPE_TO_PERSISTENT(type)        { Common::Property::TypeID::type, RekordBuddy::CollectionImplementation::RBSchema::Type::Track ## type }

        NXA_TAG_TYPE_TO_PERSISTENT(Album),
        NXA_TAG_TYPE_TO_PERSISTENT(BeatGridLockedFlag),
        { Common::Property::TypeID::BitDepthInBits, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBitDepth },
        { Common::Property::TypeID::BitRateInKiloBitsPerSecond, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBitRate },
        { Common::Property::TypeID::BeatsPerMinute, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackBpm, },
        NXA_TAG_TYPE_TO_PERSISTENT(Color),
        NXA_TAG_TYPE_TO_PERSISTENT(Comments),
        NXA_TAG_TYPE_TO_PERSISTENT(DateAdded),
        { Common::Property::TypeID::LastModifiedOn, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackModificationTime },
        NXA_TAG_TYPE_TO_PERSISTENT(DateReleased),
        NXA_TAG_TYPE_TO_PERSISTENT(FileSize),
        NXA_TAG_TYPE_TO_PERSISTENT(FileType),
        NXA_TAG_TYPE_TO_PERSISTENT(Genre),
        NXA_TAG_TYPE_TO_PERSISTENT(Grouping),
        { Common::Property::TypeID::MusicalKey, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackKey },
        { Common::Property::TypeID::LengthInSeconds, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackLength },
        NXA_TAG_TYPE_TO_PERSISTENT(MixName),
        { Common::Property::TypeID::TrackNumber, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackNumber },
        NXA_TAG_TYPE_TO_PERSISTENT(DiscNumber),
        NXA_TAG_TYPE_TO_PERSISTENT(PlayCount),
        NXA_TAG_TYPE_TO_PERSISTENT(Rating),
        NXA_TAG_TYPE_TO_PERSISTENT(RecordLabel),
        { Common::Property::TypeID::SampleRateInHertz, RekordBuddy::CollectionImplementation::RBSchema::Type::TrackSampleRate },
        NXA_TAG_TYPE_TO_PERSISTENT(Tag),
        NXA_TAG_TYPE_TO_PERSISTENT(Title),

        #undef NXA_TAG_TYPE_TO_PERSISTENT
    };

    const auto result = convertionTable.maybeValueForKey(type);
#if defined(NXA_BUGSNAG_APP_ID)
    NXA_ASSERT_TRUE_WITH_BLOCK(result.isValid(), [&type]{
        CrashLog::addUserInfoWithKey(String::stringWithFormat("%d", static_cast<integer>(type)), "propertyType");
    });
#endif

    return *result;
}
