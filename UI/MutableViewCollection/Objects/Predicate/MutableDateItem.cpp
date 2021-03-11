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

#include <MutableViewCollection/Predicate/MutableDateItem.hpp>
#include <UiBase/Types.hpp>

#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>

#include <utility>

using namespace NxA;
using namespace RekordBuddy;
using namespace Common;

struct DateMethodPredicateInspector
{
    using Result = DateMatchMethod;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        switch (comparisonOperator) {
            case TrackPredicate::ComparisonOperator::Equal:
                return DateMatchMethod::Is;
            case TrackPredicate::ComparisonOperator::NotEqual:
                return DateMatchMethod::IsNot;
            case TrackPredicate::ComparisonOperator::GreaterThan:
                return DateMatchMethod::After;
            case TrackPredicate::ComparisonOperator::LessThan:
                return DateMatchMethod::Before;
            default:
                NXA_ALOG("Unsupported comparison operator");
        }
    }
};

struct DatePredicateInspector
{
    using Result = QDate;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        return QDate::fromString(fromString(value));
    }
};

MutableDateItem::MutableDateItem(ChoiceData data, const TrackPredicate& withPredicate)
    : AbstractDateItem{ std::move(data), inspectComparisonTrackPredicateUsing(withPredicate, DatePredicateInspector{}),
                        inspectComparisonTrackPredicateUsing(withPredicate, DateMethodPredicateInspector{}) }
{
}

Optional<TrackPredicate> MutableDateItem::asTrackPredicate() const
{
    TrackPredicate::ComparisonOperator comparisonOperator;

    switch (this->getMethod()) {
        case DateMatchMethod::Is: {
            comparisonOperator = TrackPredicate::ComparisonOperator::Equal;
            break;
        }
        case DateMatchMethod::IsNot: {
            comparisonOperator = TrackPredicate::ComparisonOperator::NotEqual;
            break;
        }
        case DateMatchMethod::After: {
            comparisonOperator = TrackPredicate::ComparisonOperator::GreaterThan;
            break;
        }
        case DateMatchMethod::Before: {
            comparisonOperator = TrackPredicate::ComparisonOperator::LessThan;
            break;
        }
        default:
            NXA_ALOG("Unsupported comparison operator");
    }

    auto&& choice = this->getChoiceData();
    NXA_ASSERT_TRUE(choice.maybeIdentity.isValid());

    auto value = getValue().toString();
    return TrackPredicate::comparisonWith(static_cast<Property::TypeID>(*choice.maybeIdentity), comparisonOperator, fromString(value));
}
