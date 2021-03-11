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

#pragma once

#include <qlabel.h>

namespace NxA::RekordBuddy {

class ClickableColor : public QLabel
{
    Q_OBJECT

    QColor storedColor{Qt::black};

public:
    ClickableColor(QColor withColor, QWidget* parent = nullptr) : QLabel{parent}, storedColor{withColor} {}

    Q_PROPERTY(QColor color READ color CONSTANT)
    QColor color() const;

protected:
    void mouseReleaseEvent(QMouseEvent *ev) override;

signals:
    void colorClicked(QColor);
    void colorChanged();
};

}
