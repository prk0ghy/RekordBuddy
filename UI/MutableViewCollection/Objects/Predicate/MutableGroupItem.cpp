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

#include <Base/String.hpp>
#include <Base/Variant.hpp>
#include <MutableViewCollection/Predicate/MutableGroupItem.hpp>
#include <MutableViewCollection/Predicate/MutableStringItem.hpp>
#include <MutableViewCollection/Predicate/MutableNumberItem.hpp>
#include <MutableViewCollection/Predicate/MutableTagItem.hpp>
#include <MutableViewCollection/Predicate/MutableEnumerationItem.hpp>
#include <MutableViewCollection/Predicate/MutableDateItem.hpp>
#include <CommonCollection/Tracks/TrackPredicateInspector.hpp>
#include <list>
#include <QObject>
#include <utility>

using namespace NxA;
using namespace RekordBuddy;
using namespace Common;

static auto childChoice = ChoiceData{ {}, {}, PredicateRule::ChildRules };

struct NxA::RekordBuddy::GroupPredicateInspectorData
{
    GroupMatchMethod type;

    // std::list because of move is required too much, and it's tricky to get MutableArray/Array
    // infrastructure to work with unique_ptr
    std::list<std::unique_ptr<AbstractPredicateItem>> list;

    GroupPredicateInspectorData(const GroupPredicateInspectorData&) = delete;
    GroupPredicateInspectorData(GroupPredicateInspectorData&& other) noexcept : type{ other.type }, list{ std::move(other.list) } {}

    std::unique_ptr<MutableGroupItem> asGroup()
    {
        return std::make_unique<MutableGroupItem>(childChoice, std::move(this->list), this->type);
    }

    GroupPredicateInspectorData(GroupMatchMethod withType, std::unique_ptr<AbstractPredicateItem>&& left, GroupPredicateInspectorData&& right)
        : type{ right.type }, list{ std::move(right.list) }
    {
        if (withType == right.type) {
            this->list.push_front(std::move(left));
        }
        else {
            auto oldGroup = this->asGroup();
            this->list = std::list<std::unique_ptr<AbstractPredicateItem>>{};
            this->type = withType;
            this->list.push_back(std::move(left));
            this->list.push_back(std::move(oldGroup));
        }
    }
    GroupPredicateInspectorData(GroupMatchMethod withType, GroupPredicateInspectorData&& left, std::unique_ptr<AbstractPredicateItem>&& right)
        : type{ left.type }, list{ std::move(left.list) }
    {
        if (withType == left.type) {
            this->list.push_back(std::move(right));
        }
        else {
            auto oldGroup = this->asGroup();
            this->list = std::list<std::unique_ptr<AbstractPredicateItem>>{};
            this->type = withType;
            this->list.push_back(std::move(oldGroup));
            this->list.push_back(std::move(right));
        }
    }
    GroupPredicateInspectorData(GroupMatchMethod withType, GroupPredicateInspectorData&& left, GroupPredicateInspectorData&& right) : type{ withType }
    {
        if (left.type == right.type && withType == left.type) {
            list = std::move(left.list);
            for (auto&& rightElem : right.list) {
                list.push_back(std::move(rightElem));
            }
        }
        else if (right.type == withType) {
            list = std::move(right.list);
            list.push_front(left.asGroup());
        }
        else if (left.type == withType) {
            list = std::move(left.list);
            list.push_back(right.asGroup());
        }
        else {
            std::list<std::unique_ptr<AbstractPredicateItem>> newList{};
            for (auto&& elem : left.list) {
                newList.push_back(std::move(elem));
            }
            for (auto&& elem : right.list) {
                newList.push_back(std::move(elem));
            }
            this->list = std::move(newList);
        }
    }
    GroupPredicateInspectorData(GroupMatchMethod withType, std::unique_ptr<AbstractPredicateItem>&& left,
                                std::unique_ptr<AbstractPredicateItem>&& right)
        : type{ withType }
    {
        this->list.push_back(std::move(left));
        this->list.push_back(std::move(right));
    }
    GroupPredicateInspectorData(GroupMatchMethod withType, std::list<std::unique_ptr<AbstractPredicateItem>>&& withList)
        : type{ withType }, list{ std::move(withList) }
    {
    }
};

// We pick an operand from the root (and, or) then walk children.
// ComparisonPredicate are added to the list, Compound predicates are (perhaps) inverted with demorgan's law and then recursively walked.
//
//  (a | ((x & y) | (c | d))) =>  |[a, &[x, y], c, d]
struct NxA::RekordBuddy::GroupPredicateInspector
{
    using Result = Variant<GroupPredicateInspectorData, std::unique_ptr<AbstractPredicateItem>>;

    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID typeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        // TODO: More than just NameMatch
        return std::make_unique<MutableStringItem>(ChoiceData{ static_cast<ChoiceData::IdentityType>(typeID), {}, PredicateRule::NameMatch },
                                                   predicate);
    }

    Result fromCompoundPredicate(const TrackPredicate& predicate, Result left, TrackPredicate::CompoundOperator compoundOperator, Result right)
    {
        GroupMatchMethod method{ compoundOperator == TrackPredicate::CompoundOperator::And ? GroupMatchMethod::All : GroupMatchMethod::Any };
        return left.apply([&](auto&& leftCasted) {
            return right.apply([&](auto&& rightCasted) {
                return GroupPredicateInspectorData{ method, std::move(leftCasted), std::move(rightCasted) };
            });
        });
    }
};

struct GroupMethodInspector
{
    using Result = GroupMatchMethod;
    Result fromComparisonPredicate(const TrackPredicate& predicate, Property::TypeID typeID, TrackPredicate::ComparisonOperator comparisonOperator,
                                   const String& value)
    {
        return GroupMatchMethod::All;
    }
    Result fromCompoundPredicate(const TrackPredicate& predicate, const GroupMatchMethod& left, TrackPredicate::CompoundOperator compoundOperator,
                                 const GroupMatchMethod& right)
    {
        return compoundOperator == TrackPredicate::CompoundOperator::And ? GroupMatchMethod::All : GroupMatchMethod::Any;
    }
};

MutableGroupItem::MutableGroupItem(ChoiceData rule, std::list<std::unique_ptr<AbstractPredicateItem>>&& list, GroupMatchMethod method)
    : AbstractGroupItem{ std::move(rule), std::move(list), method }
{
}

MutableGroupItem::MutableGroupItem(ChoiceData rule, const Optional<TrackPredicate>& withPredicate)
    : AbstractGroupItem{ std::move(rule), MutableGroupItem::listFromPredicate(withPredicate), MutableGroupItem::methodFromPredicate(withPredicate) }
{
}

GroupMatchMethod MutableGroupItem::methodFromPredicate(const Optional<TrackPredicate>& withPredicate)
{
    if (!withPredicate.isValid()) {
        return GroupMatchMethod::All;
    }
    return inspectTrackPredicateUsing(*withPredicate, GroupMethodInspector{});
}

std::list<std::unique_ptr<AbstractPredicateItem>>&& MutableGroupItem::getValues()
{
    return std::move(this->value);
}

std::list<std::unique_ptr<AbstractPredicateItem>> MutableGroupItem::listFromPredicate(Optional<TrackPredicate> withPredicate)
{
    if (!withPredicate.isValid()) {
        return {};
    }
    struct Visitor
    {
        std::list<std::unique_ptr<AbstractPredicateItem>> operator()(std::unique_ptr<AbstractPredicateItem>& item)
        {
            std::list<std::unique_ptr<AbstractPredicateItem>> singleElement;
            singleElement.push_back(std::move(item));
            return singleElement;
        }
        std::list<std::unique_ptr<AbstractPredicateItem>> operator()(GroupPredicateInspectorData& item)
        {
            return std::move(item.list);
        }
    };
    return inspectTrackPredicateUsing(*withPredicate, GroupPredicateInspector{}).apply(Visitor{});
}

QList<ChoiceData> MutableGroupItem::getPossibleRules() const
{
    return { ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::ArtistCredit), "Artist", PredicateRule::NameMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::Title), "Track Name", PredicateRule::NameMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::DateAdded), "Date Added", PredicateRule::DateMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::DateReleased), "Date Released", PredicateRule::DateMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::Tag), "Tags", PredicateRule::TagMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::LengthInSeconds), "Track Length", PredicateRule::NumberMatch },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::MusicalKey),
                         "Track Key",
                         PredicateRule::EnumerationMatch,
                         { "A", "B", "C", "D", "E", "F", "G" } },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::FileType),
                         "File Type",
                         PredicateRule::EnumerationMatch,
                         { "MP3", "AAC", "OGG" } },
             ChoiceData{ static_cast<ChoiceData::IdentityType>(Property::TypeID::Grouping), "Group of Rules", PredicateRule::ChildRules } };
}

std::unique_ptr<AbstractPredicateItem> MutableGroupItem::createDefaultElement(ChoiceData rule) const
{
    if (rule.rule == PredicateRule::ChildRules) {
        return std::make_unique<MutableGroupItem>(rule, nothing);
    }

    if (!rule.maybeIdentity.isValid()) {
        auto rules = getPossibleRules();
        rule = rules.first();
        NXA_ASSERT_TRUE(rule.maybeIdentity.isValid());
    }

    auto id = static_cast<Property::TypeID>(*rule.maybeIdentity);

    switch (Property::valueTypeIDFromPropertyType(id)) {
        case Property::StringValueType: {
            switch (id) {
                case Property::TypeID::Genre:
                case Property::TypeID::RemixerCredit:
                case Property::TypeID::ProducerCredit:
                case Property::TypeID::ArtistCredit: {
                    // TODO: Decide to use tag editor? something else for autocomplete?
                    break;
                }
                case Property::TypeID::Tag: {
                    return std::make_unique<MutableTagItem>(rule, TrackPredicate::comparisonWith(id, TrackPredicate::ComparisonOperator::Equal, ""));
                }
                case Property::TypeID::DateAdded:
                case Property::TypeID::DateReleased: {
                    return std::make_unique<MutableDateItem>(rule, TrackPredicate::comparisonWith(id, TrackPredicate::ComparisonOperator::Equal,
                                                                                                  fromString(QDate::currentDate().toString())));
                }
                default:
                    break;
            }
            return std::make_unique<MutableStringItem>(rule, TrackPredicate::comparisonWith(id, TrackPredicate::ComparisonOperator::Contains, ""));
        }
        case Property::IntegerValueType: {
            switch (id) {
                case Property::TypeID::MusicalKey:
                case Property::TypeID::FileType: {
                    return std::make_unique<MutableEnumerationItem>(
                        rule, TrackPredicate::comparisonWith(id, TrackPredicate::ComparisonOperator::Equal, fromString(rule.options.first())));
                }
                default:
                    return std::make_unique<MutableNumberItem>(rule,
                                                               TrackPredicate::comparisonWith(id, TrackPredicate::ComparisonOperator::Equal, ""));
            }
        }
        default:
            break;
    }

    NXA_ALOG("Failed to create a default element");
    return nullptr;
}

Optional<TrackPredicate> MutableGroupItem::asTrackPredicate() const
{
    Optional<TrackPredicate> current{};
    auto method = this->getMethod();
    for (auto&& element : this->value) {
        if (!element) {
            continue; // todo?
        }
        auto dp = dynamic_cast<MutablePredicate*>(element.get());
        NXA_ASSERT_NOT_NULL(dp);

        if (!current.isValid()) {
            current = dp->asTrackPredicate();
            continue;
        }
        TrackPredicate::CompoundOperator compoundOperator;

        switch (method) {
            case GroupMatchMethod::All: {
                compoundOperator = TrackPredicate::CompoundOperator::And;
                break;
            }
            case GroupMatchMethod::Any: {
                compoundOperator = TrackPredicate::CompoundOperator::Or;
                break;
            }
            case GroupMatchMethod::None: {
                NXA_ALOG("Not Supported by Collection");
                break;
            }
        }
        auto rhs = dp->asTrackPredicate();
        NXA_ASSERT_TRUE(rhs.isValid());
        current = TrackPredicate::compoundWith(*current, compoundOperator, *rhs);
    }
    return current;
}
