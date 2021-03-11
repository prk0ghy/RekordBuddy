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

#include <Widgets/TagChildWidget.hpp>

#include <qbrush.h>
#include <qchar.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qwidget.h>

#include <utility>

#include "Widgets/moc_TagChildWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

TagChildWidget::TagChildWidget(int index, QColor color, int hmargin, int vmargin, QWidget* parent)
    : QLabel{parent}
    , indexInParent{index}
    , currentCursor{0}
    , currentBubbleColor{std::move(color)}
    , currentHorizontalMargin{hmargin}
    , currentVerticalMargin{vmargin}
{
    this->setTagText("");
    this->setFocus();
}

TagChildWidget::TagChildWidget(const QString& label, int index, QColor color, int hmargin, int vmargin, QWidget* parent)
    : QLabel{label, parent}
    , indexInParent{index}
    , currentCursor{-1}
    , currentBubbleColor{std::move(color)}
    , currentHorizontalMargin{hmargin}
    , currentVerticalMargin{vmargin}
{
    this->setTagText(label);
}

void TagChildWidget::setTagText(const QString& text)
{
    this->textLayout.setText(text);
    this->textLayout.beginLayout();
    this->textLayout.createLine();
    this->textLayout.endLayout();
    auto width = this->textLayout.lineAt(0).naturalTextWidth();
    auto height = this->textLayout.lineAt(0).height();
    this->currentLineHeight = int(height);
    this->setFixedWidth(int(width + this->currentHorizontalMargin * 2.0));
    this->setFixedHeight(int(height + this->currentVerticalMargin * 4.0));
}

void TagChildWidget::paintEvent(QPaintEvent* event)
{
    QStyle* style = QWidget::style();
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);

    QRect bubbleFrame = frameRect();

    QStyleOption opt;
    opt.initFrom(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush{this->currentBubbleColor});
    painter.drawRoundedRect(bubbleFrame, bubbleFrame.height() / 3.0, bubbleFrame.height() / 3.0);

    bubbleFrame.setY(this->currentVerticalMargin * 2);
    bubbleFrame.setHeight(this->currentLineHeight);
    style->drawItemText(&painter, bubbleFrame, Qt::AlignHCenter, opt.palette, true, this->textLayout.text(), this->foregroundRole());

    if (this->currentCursor == -1) {
        // TODO: put the X to delete this thing
    }
    else {
        QPointF topLeft = bubbleFrame.topLeft();
        topLeft.setX(topLeft.x() + this->currentHorizontalMargin - 2);
        topLeft.setY(topLeft.y());
        this->textLayout.drawCursor(&painter, topLeft, this->currentCursor, 1);
    }
}

void TagChildWidget::mousePressEvent(QMouseEvent* event)
{
    this->setFocus();

    auto&& line = this->textLayout.lineAt(0);
    int x = event->pos().x();
    this->currentCursor = line.xToCursor(x - this->currentHorizontalMargin);
}

void TagChildWidget::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        this->clearFocus();
        emit tagCreated(this);
        return;
    }

    if (key == Qt::Key_Left) {
        if (this->currentCursor > 0) {
            this->currentCursor -= 1;
            this->update();
        }
        return;
    }

    if (key == Qt::Key_Right) {
        if (this->currentCursor < this->textLayout.text().length()) {
            this->currentCursor += 1;
            this->update();
        }
        return;
    }

    if (key == Qt::Key_Backspace) {
        if (this->currentCursor > 0) {
            QString text = this->textLayout.text().remove(this->currentCursor - 1, 1);
            this->setTagText(text);

            if (text.length()) {
                this->currentCursor -= 1;
            }
            else {
                emit tagChanged(this);
            }
        }
        return;
    }

    if (key == Qt::Key_Delete) {
        QString text = this->textLayout.text();
        if (this->currentCursor < text.length()) {
            text = text.remove(this->currentCursor, 1);
            this->setTagText(text);

            if (!text.length()) {
                emit tagChanged(this);
            }
        }
        return;
    }

    QString text = event->text();
    if (text.length() != 1) {
        return;
    }

    QChar firstCharacter = text[0];
    if (firstCharacter.isPrint() || firstCharacter.isSpace()) {
        text = this->textLayout.text().insert(this->currentCursor, firstCharacter);
        this->setTagText(text);
        this->currentCursor += 1;
    }
}

const QString TagChildWidget::tagText()
{
    return this->textLayout.text();
}

int TagChildWidget::index() const
{
    return this->indexInParent;
}

const QColor& TagChildWidget::color() const
{
    return this->currentBubbleColor;
}

int TagChildWidget::horizontalMargin() const
{
    return this->currentHorizontalMargin;
}

int TagChildWidget::verticalMargin() const
{
    return this->currentVerticalMargin;
}

void TagChildWidget::focusOutEvent(QFocusEvent* event)
{
    this->currentCursor = -1;
    this->update();
    emit tagChanged(this);
}

void TagChildWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange) {
        this->textLayout.setFont(this->font());
        this->setTagText(this->textLayout.text());
    }
}

void TagChildWidget::setColor(const QColor& color)
{
    this->currentBubbleColor = color;
    this->repaint();
}

void TagChildWidget::setHorizontalMargin(int margin)
{
    this->currentHorizontalMargin = margin;
    this->setTagText(this->textLayout.text());
    this->repaint();
}

void TagChildWidget::setVerticalMargin(int margin)
{
    this->currentVerticalMargin = margin;
    this->setTagText(this->textLayout.text());
    this->repaint();
}
