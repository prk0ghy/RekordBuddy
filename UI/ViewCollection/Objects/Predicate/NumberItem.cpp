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

#include <ViewCollection/Predicate/NumberItem.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>
#include <utility>

using namespace NxA;
using namespace RekordBuddy;
using namespace Common;

struct NumberMethodPredicateInspector
{
    using Result = NumberMatchMethod;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        switch (comparisonOperator) {
            case TrackPredicate::ComparisonOperator::Equal:
                return NumberMatchMethod::Equal;
            case TrackPredicate::ComparisonOperator::NotEqual:
                return NumberMatchMethod::NotEqual;
            case TrackPredicate::ComparisonOperator::LessThanOrEqual:
                return NumberMatchMethod::LessThanOrEqual;
            case TrackPredicate::ComparisonOperator::LessThan:
                return NumberMatchMethod::LessThan;
            case TrackPredicate::ComparisonOperator::GreaterThan:
                return NumberMatchMethod::GreaterThan;
            case TrackPredicate::ComparisonOperator::GreaterThanOrEqual:
                return NumberMatchMethod::GreaterThanOrEqual;
            default:
                NXA_ALOG("Unsupported comparison operator");
        }
    }
};

struct NumberPredicateInspector
{
    using Result = integer64;

    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        // TODO: figure out TrackPredicate's number format. double?
        return -1;
        //return QNumber::fromString(QString{value.asUTF8()});
    }
};

NumberItem::NumberItem(ChoiceData data, const TrackPredicate& predicate)
    : AbstractNumberItem{ std::move(data), inspectComparisonTrackPredicateUsing(predicate, NumberPredicateInspector{}),
                          inspectComparisonTrackPredicateUsing(predicate, NumberMethodPredicateInspector{}) }
{
}

Optional<TrackPredicate> NumberItem::asTrackPredicate() const
{
    TrackPredicate::ComparisonOperator comparisonOperator;

    switch (this->getMethod()) {
        case NumberMatchMethod::Equal: {
            comparisonOperator = TrackPredicate::ComparisonOperator::Equal;
            break;
        }
        case NumberMatchMethod::NotEqual: {
            comparisonOperator = TrackPredicate::ComparisonOperator::NotEqual;
            break;
        }
        case NumberMatchMethod::GreaterThan: {
            comparisonOperator = TrackPredicate::ComparisonOperator::GreaterThan;
            break;
        }
        case NumberMatchMethod::LessThan: {
            comparisonOperator = TrackPredicate::ComparisonOperator::LessThan;
            break;
        }
        case NumberMatchMethod::GreaterThanOrEqual: {
            comparisonOperator = TrackPredicate::ComparisonOperator::GreaterThanOrEqual;
            break;
        }
        case NumberMatchMethod::LessThanOrEqual: {
            comparisonOperator = TrackPredicate::ComparisonOperator::LessThanOrEqual;
            break;
        }
        default:
            NXA_ALOG("Unsupported comparison operator");
    }

    auto&& choice = this->getChoiceData();
    NXA_ASSERT_TRUE(choice.maybeIdentity.isValid());

    auto value = getValue();
    return TrackPredicate::comparisonWith(static_cast<Property::TypeID>(*choice.maybeIdentity), comparisonOperator,
                                          fromString(QString{ "%1" }.arg(value)));
}
