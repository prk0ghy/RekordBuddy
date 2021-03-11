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

#include <ViewCollection/Predicate/Predicate.hpp>
#include <ViewCollection/Predicate/EnumerationItem.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>
#include <utility>

using namespace NxA;
using namespace RekordBuddy;
using namespace Common;

struct EnumerationMethodPredicateInspector
{
    using Result = EnumerationMatchMethod;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        switch (comparisonOperator) {
            case TrackPredicate::ComparisonOperator::Equal:
                return EnumerationMatchMethod::Is;
            case TrackPredicate::ComparisonOperator::NotEqual:
                return EnumerationMatchMethod::IsNot;
            default:
                NXA_ALOG("Unsupported comparison operator");
        }
    }
};

struct EnumerationPredicateInspector
{
    using Result = QString;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID tagTypeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        return fromString(value);
    }
};

EnumerationItem::EnumerationItem(ChoiceData data, const TrackPredicate& withPredicate)
    : AbstractEnumerationItem{ std::move(data), inspectComparisonTrackPredicateUsing(withPredicate, EnumerationPredicateInspector{}),
                               inspectComparisonTrackPredicateUsing(withPredicate, EnumerationMethodPredicateInspector{}) }
{
}

Optional<TrackPredicate> EnumerationItem::asTrackPredicate() const
{
    TrackPredicate::ComparisonOperator comparisonOperator;

    switch (this->getMethod()) {
        case EnumerationMatchMethod::Is: {
            comparisonOperator = TrackPredicate::ComparisonOperator::Equal;
            break;
        }
        case EnumerationMatchMethod::IsNot: {
            comparisonOperator = TrackPredicate::ComparisonOperator::NotEqual;
            break;
        }
        default:
            NXA_ALOG("Unsupported comparison operator");
    }

    auto&& choice = this->getChoiceData();
    NXA_ASSERT_TRUE(choice.maybeIdentity.isValid());

    auto value = getValue();
    return TrackPredicate::comparisonWith(static_cast<Property::TypeID>(*choice.maybeIdentity), comparisonOperator, fromString(value));
}
