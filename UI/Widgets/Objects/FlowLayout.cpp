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

#include <Widgets/FlowLayout.hpp>

#include <qalgorithms.h>
#include <qglobal.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qwidget.h>

#include "Widgets/moc_FlowLayout.cpp"

using namespace NxA;
using namespace RekordBuddy;

FlowLayout::FlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing) : QLayout{parent}, hSpace{hSpacing}, vSpace{vSpacing}
{
    setContentsMargins(margin, margin, margin, margin);
    this->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);

}

FlowLayout::~FlowLayout()
{
    qDeleteAll(itemList);
}

void FlowLayout::addItem(QLayoutItem* item)
{
    itemList.append(item);
}

int FlowLayout::horizontalSpacing() const
{
    if (hSpace >= 0) {
        return hSpace;
    }
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing() const
{
    if (vSpace >= 0) {
        return vSpace;
    }
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem* FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem* FlowLayout::takeAt(int index)
{
    if (index < 0 || index >= itemList.size()) {
        return nullptr;
    }
    return itemList.takeAt(index);
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Orientation::Horizontal;
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    auto height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void FlowLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (auto&& item : itemList) {
        size = size.expandedTo(item->minimumSize());
    }

    size += QSize{2 * margin(), 2 * margin()};
    return size;
}

int FlowLayout::doLayout(QRect rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(left, top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (auto&& item : itemList) {
        QWidget* wid = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1) {
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = verticalSpacing();
        if (spaceY == -1) {
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject* parent = this->parent();
    if (parent == nullptr) {
        return -1;
    }
    if (parent->isWidgetType()) {
        auto* pw = static_cast<QWidget*>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }
    return static_cast<QLayout*>(parent)->spacing();
}

void FlowLayout::setHorizontalSpacing(int spacing)
{
    hSpace = spacing;
    update();
}

void FlowLayout::setVerticalSpacing(int spacing)
{
    vSpace = spacing;
    update();
}
