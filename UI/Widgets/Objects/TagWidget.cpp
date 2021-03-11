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

#include <Widgets/TagWidget.hpp>
#include <qsizepolicy.h>
#include <qwidget.h>
#include <qstring.h>
#include <qtimer.h>
#include <Widgets/FlowLayout.hpp>
#include <Widgets/TagChildWidget.hpp>

#include "Widgets/moc_TagWidget.cpp"

class QMouseEvent;

using namespace NxA;
using namespace RekordBuddy;

TagWidget::TagWidget(QWidget* parent) : QLineEdit{parent}, currentTagColor{113, 51, 15}, horizontalMargin{15}, verticalMargin{2}
{
    this->setObjectName(QStringLiteral("TagWidget"));

    this->flowLayout = new FlowLayout{this, 4, 5, 2};
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setMinimumHeight(36);
    this->setLayout(this->flowLayout);
    this->setContentsMargins(1, 1, 1, 1);
    this->clear();
}

void TagWidget::clear()
{
    while (!this->childWidgetList.isEmpty()) {
        this->removeTagAtIndex(0);
    }
}

QStringList TagWidget::tags()
{
    this->clearBlankTags();
    return this->currentTags;
}

void TagWidget::setTags(const QStringList& tagList)
{
    this->clear();
    this->appendTags(tagList);
}

void TagWidget::appendTags(const QStringList& tagList)
{
    bool tagsDidChange = false;

    for (const auto& item : tagList) {
        this->currentTags << item;
        auto* childWidget = new TagChildWidget{item, this->childWidgetList.size(), this->currentTagColor, this->horizontalMargin, this->verticalMargin, this};
        this->childWidgetList.append(childWidget);
        this->flowLayout->addWidget(childWidget);
        connect(childWidget, &TagChildWidget::tagChanged, this, &TagWidget::tagChanged);
        connect(childWidget, &TagChildWidget::tagCreated, this, &TagWidget::tagCreated);
        tagsDidChange = true;
    }

    if (tagsDidChange) {
        emit tagsChanged(this->currentTags);
    }
}

const QColor& TagWidget::color() const
{
    return this->currentTagColor;
}

void TagWidget::clearBlankTags()
{
    int index = 0;
    for (auto&& toPoke : this->childWidgetList) {
        if (toPoke->tagText().isEmpty()) {
            this->removeTagAtIndex(index);
        }
        index++;
    }
}

void TagWidget::mousePressEvent(QMouseEvent* event)
{
    this->currentTags << QStringLiteral("");
    auto* item = new TagChildWidget{this->childWidgetList.size(), this->currentTagColor, this->horizontalMargin, this->verticalMargin, this};
    this->childWidgetList.append(item);
    this->flowLayout->addWidget(item);
    connect(item, &TagChildWidget::tagChanged, this, &TagWidget::tagChanged);
    connect(item, &TagChildWidget::tagCreated, this, &TagWidget::tagCreated);
}

void TagWidget::removeTagAtIndex(int index)
{
    this->currentTags.takeAt(index);
    auto item = this->childWidgetList.takeAt(index);
    this->flowLayout->removeWidget(item);
    item->deleteLater();
}

void TagWidget::tagChanged(TagChildWidget* item)
{
    if (item == nullptr) {
        return;
    }

    auto newEntry = item->tagText();
    if (newEntry.isEmpty()) {
        this->removeTagAtIndex(item->index());
        emit tagsChanged(this->currentTags);
        return;
    }

    if (this->currentTags[item->index()] != newEntry) {
        this->currentTags[item->index()] = newEntry;
        emit tagsChanged(this->currentTags);
    }
}

void TagWidget::tagCreated(TagChildWidget* item)
{
    if (item == nullptr) {
        return;
    }

    this->currentTags << QStringLiteral("");
    auto* newItem = new TagChildWidget{this->childWidgetList.size(), this->currentTagColor, this->horizontalMargin, this->verticalMargin, this};
    this->childWidgetList.append(newItem);
    this->flowLayout->addWidget(newItem);
    connect(newItem, &TagChildWidget::tagChanged, this, &TagWidget::tagChanged);
    connect(newItem, &TagChildWidget::tagCreated, this, &TagWidget::tagCreated);
}

void TagWidget::setColor(const QColor& color)
{
    currentTagColor = color;

    for (auto&& item : this->childWidgetList) {
        item->setColor(color);
    }
    this->repaint();
}

void TagWidget::setHorizontalMargin(int margin)
{
    if (margin < 15) {
        return;
    }

    this->horizontalMargin = margin;

    for (auto&& item : this->childWidgetList) {
        item->setHorizontalMargin(margin);
    }
}

void TagWidget::setVerticalMargin(int margin)
{
    if (margin < 2) {
        return;
    }

    this->verticalMargin = margin;

    for (auto&& item : this->childWidgetList) {
        item->setVerticalMargin(margin);
    }
}

void TagWidget::setHorizontalSpacing(int spacing)
{
    if (spacing < -1) {
        return;
    }

    this->flowLayout->setHorizontalSpacing(spacing);
}

void TagWidget::setVerticalSpacing(int spacing)
{
    if (spacing < -1) {
        return;
    }

    this->flowLayout->setVerticalSpacing(spacing);
}
