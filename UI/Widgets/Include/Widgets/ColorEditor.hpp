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

#include <qobjectdefs.h>
#include <qwidget.h>
#include <Base/NotNull.hpp>

namespace Ui {
class ColorEditor;
}

class QLabel;
class QObject;

namespace NxA::RekordBuddy {

class FlowLayout;
class TagChildWidget;

class ColorEditor : public QWidget
{
    Q_OBJECT
    NotNull<Ui::ColorEditor*> ui;
    QColor currentColor;

public:

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    explicit ColorEditor(QWidget* parent = nullptr);
    ~ColorEditor() override = default;

    QColor color();
    void setColor(const QColor& newValue);

public slots:
    void colorClicked(const QColor& newValue);

signals:
    void colorChanged();

};

}
