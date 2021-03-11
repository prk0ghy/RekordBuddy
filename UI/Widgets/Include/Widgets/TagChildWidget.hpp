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

#include <qcolor.h>
#include <qlabel.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtextlayout.h>

class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QObject;
class QPaintEvent;
class QWidget;

namespace NxA::RekordBuddy {

class TagWidget;

class TagChildWidget : public QLabel
{
    Q_OBJECT

    friend class TagWidget;

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int horizontalMargin READ horizontalMargin WRITE setHorizontalMargin)
    Q_PROPERTY(int verticalMargin READ verticalMargin WRITE setVerticalMargin)

public:
    explicit TagChildWidget(int index, QColor color, int hmargin, int vmargin, QWidget* parent = nullptr);
    explicit TagChildWidget(const QString& label, int index, QColor color, int hmargin, int vmargin, QWidget* parent = nullptr);

    const QString tagText();
    int index() const;
    const QColor& color() const;
    int horizontalMargin() const;
    int verticalMargin() const;

public slots:

    void setTagText(const QString& text);
    void setColor(const QColor& color);
    void setHorizontalMargin(int margin);
    void setVerticalMargin(int margin);

protected:
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    QTextLayout textLayout;

    int indexInParent;
    int currentCursor;

    QColor currentBubbleColor;
    int currentLineHeight;

    int currentHorizontalMargin;
    int currentVerticalMargin;

signals:

    void tagChanged(TagChildWidget*);
    void tagCreated(TagChildWidget*);
};
}
