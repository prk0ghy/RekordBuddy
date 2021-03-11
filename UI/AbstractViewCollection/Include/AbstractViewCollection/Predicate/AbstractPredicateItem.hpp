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

#include <Base/Types.hpp>

#include <UiBase/Types.hpp>

#include <QString>
#include <QMetaType>
#include <QStringList>

#pragma once

namespace NxA::RekordBuddy {

// all of the rule types that RuleWidget supports
enum class PredicateRule { None, ChildRules, NameMatch, TagMatch, DateMatch, NumberMatch, EnumerationMatch };
}

struct ChoiceData
{
    // -- ChoiceData is abstract; so we can't store Collection-specific stuff in it
    using IdentityType = NxA::byte;
    NxA::Optional<IdentityType> maybeIdentity;
    QString name;
    NxA::RekordBuddy::PredicateRule rule = NxA::RekordBuddy::PredicateRule::None;
    QStringList options = {};
    bool operator==(const ChoiceData& other)
    {
        return other.maybeIdentity == this->maybeIdentity && other.rule == this->rule;
    }
};

Q_DECLARE_METATYPE(ChoiceData)

namespace NxA::RekordBuddy {

class AbstractPredicateItem
{
    ChoiceData base;

public:
    virtual ~AbstractPredicateItem() = default;
    AbstractPredicateItem(ChoiceData withRule);
    ChoiceData getChoiceData() const;
};
}
