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

#include <Widgets/DurationViewer.hpp>
#include "Widgets/moc_DurationViewer.cpp"

using namespace NxA;
using namespace RekordBuddy;

Duration DurationViewer::duration() const
{return p_duration; }

void DurationViewer::setDuration(Duration newDuration) {
    p_duration = newDuration;

    auto hms = p_duration.toHoursMinutesSeconds();
    QString newValue;
    if (hms.hours == 0) {
        newValue = QStringLiteral("%1:%2").arg(hms.minutes, 2, 10, QLatin1Char('0')).arg(hms.seconds, 2, 10, QLatin1Char('0'));
    }
    else {
        newValue = QStringLiteral("%1:%2:%3").arg(hms.hours, 2, 10, QLatin1Char('0')).arg(hms.minutes, 2, 10, QLatin1Char('0')).arg(hms.seconds, 2, 10, QLatin1Char('0'));
    }
    this->setText(newValue);
}
