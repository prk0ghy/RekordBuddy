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

#include <Base/Assert.hpp>
#include <Base/NotNull.hpp>
#include <Widgets/ColorEditor.hpp>
#include <Widgets/ClickableColor.hpp>
#include <qcolor.h>
#include <qevent.h>
#include "ui_ColorEditor.h"
#include "Widgets/moc_ColorEditor.cpp"
#include "Widgets/moc_ClickableColor.cpp"

using namespace NxA;
using namespace RekordBuddy;

static QList<QColor> colors = {
    {255,   0, 127},
    {187,   0,   0},
    {255,   0,   0},
    {255, 165,   0},
    {255, 255,   0},
    {  0, 255,   0},
    {  0, 187,   0},
    {  0,   0, 255},
    {102,   0, 153},
    { 37, 253, 233},
    {  0,   0,   0},
    { 40,  40,  40},
    { 90,  90,  90},
    {142, 142, 142},
    {167, 167, 167},
    {200, 200, 200},
    {255, 255, 255},
    {153, 187, 255},
    {153, 153, 255},
    {187, 153, 255},
    {255, 153, 255},
    {255, 153, 221},
    {255, 153, 187},
    {255, 153, 153},
    {255, 187, 153},
    {255, 221, 153},
    {255, 255, 153},
    {221, 255, 153},
    {187, 255, 153},
    {153, 255, 153},
    {153, 255, 187},
    {153, 255, 221},
    {153, 255, 255},
    {153, 221, 255},
};

ColorEditor::ColorEditor(QWidget* parent) : QWidget{parent}, ui{new Ui::ColorEditor}, currentColor{Qt::white}
{
    this->ui->setupUi(this);

    int count = 0;
    for (auto&& color : colors) {
        auto pushButton = new ClickableColor(color, this->ui->widget);
        pushButton->setObjectName(QStringLiteral("pushButton_%1_%2_%3").arg(color.red()).arg(color.green()).arg(color.blue()));
        pushButton->setMinimumSize(QSize(8, 8));
        pushButton->setMaximumSize(QSize(128, 128));
        pushButton->setPalette(QPalette{color});
        pushButton->setFocusPolicy(Qt::NoFocus);
        pushButton->setContextMenuPolicy(Qt::NoContextMenu);
        pushButton->setAutoFillBackground(true);
        connect(pushButton, &ClickableColor::colorClicked, this, &ColorEditor::colorClicked);
        auto perRow = (colors.length() + 1) / 2;
        this->ui->gridLayout->addWidget(pushButton, count / perRow, count % perRow, 1, 1);
        count++;
    }
    this->setColor(QColor{Qt::black});
}

QColor ColorEditor::color()
{
    return this->currentColor;
}

void ColorEditor::colorClicked(const QColor& newValue)
{
    this->setColor(newValue);
}

void ColorEditor::setColor(const QColor& newValue)
{
    if (newValue == this->currentColor) {
        return;
    }
    QPalette palette{newValue};
    this->ui->currentColor->setPalette(palette);
    this->currentColor = newValue;
    emit colorChanged();
}
