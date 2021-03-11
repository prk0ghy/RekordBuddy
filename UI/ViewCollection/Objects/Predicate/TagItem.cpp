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

#include <ViewCollection/Predicate/TagItem.hpp>

#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;
using namespace Common;

struct TrackMethodPredicateInspector
{
    using Result = TagMatchMethod;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Common::Property::TypeID tagTypeID,
                                   TrackPredicate::ComparisonOperator comparisonOperator, const String& value)
    {
        switch (comparisonOperator) {
            case TrackPredicate::ComparisonOperator::Equal:
                return TagMatchMethod::Tagged;
            case TrackPredicate::ComparisonOperator::NotEqual:
                return TagMatchMethod::NotTagged;
            default:
                NXA_ALOG("Unsupported comparison operator");
        }
    }
};

struct TrackPredicateInspector
{
    using Result = QString;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        return QString{ value.asUTF8() };
    }
};

TagItem::TagItem(ChoiceData data, const TrackPredicate& predicate)
    : AbstractTagItem{ std::move(data), inspectComparisonTrackPredicateUsing(predicate, TrackPredicateInspector{}),
                       inspectComparisonTrackPredicateUsing(predicate, TrackMethodPredicateInspector{}) }
{
}

Optional<TrackPredicate> TagItem::asTrackPredicate() const
{
    TrackPredicate::ComparisonOperator comparaisonOperator;

    switch (this->getMethod()) {
        case TagMatchMethod::Tagged: {
            comparaisonOperator = TrackPredicate::ComparisonOperator::Equal;
            break;
        }
        case TagMatchMethod::NotTagged: {
            comparaisonOperator = TrackPredicate::ComparisonOperator::NotEqual;
            break;
        }
        default:
            NXA_ALOG("Unsupported comparison operator");
    }

    auto&& choice = this->getChoiceData();
    NXA_ASSERT_TRUE(choice.maybeIdentity.isValid());

    auto value = getValue();
    return TrackPredicate::comparisonWith(static_cast<Property::TypeID>(*choice.maybeIdentity), comparaisonOperator, fromString(value));
}
