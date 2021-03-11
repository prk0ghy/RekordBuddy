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

#include <MutableViewCollection/Predicate/MutablePredicate.hpp>
#include <AbstractViewCollection/Predicate/AbstractGroupItem.hpp>
#include <CommonCollection/Tracks/TrackPredicate.hpp>
#include <Base/Types.hpp>
#include <list>
#include <QString>

#pragma once

namespace NxA::RekordBuddy {

struct GroupPredicateInspectorData;
struct GroupPredicateInspector;

class MutableGroupItem : public AbstractGroupItem, public MutablePredicate
{
protected:
    static std::list<std::unique_ptr<AbstractPredicateItem>> listFromPredicate(Optional<Common::TrackPredicate> withPredicate);
    static GroupMatchMethod methodFromPredicate(const Optional<Common::TrackPredicate>& withPredicate);

private:
    friend struct GroupPredicateInspectorData;
    friend struct GroupPredicateInspector;
    std::list<std::unique_ptr<AbstractPredicateItem>>&& getValues() override;

public:
    MutableGroupItem(ChoiceData, std::list<std::unique_ptr<AbstractPredicateItem>>&&, GroupMatchMethod);
    MutableGroupItem(ChoiceData rule, const Optional<Common::TrackPredicate>& withPredicate);

    std::unique_ptr<AbstractPredicateItem> createDefaultElement(ChoiceData rule) const override;
    QList<ChoiceData> getPossibleRules() const override;
    Optional<Common::TrackPredicate> asTrackPredicate() const override;
};
}
