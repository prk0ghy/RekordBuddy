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
#include <qlist.h>
#include <qobjectdefs.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <qstring.h>
#include <qstringlist.h>

class QMouseEvent;
class QLabel;
class QObject;
class QWidget;

namespace NxA::RekordBuddy {

class FlowLayout;
class TagChildWidget;

class TagWidget : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

protected:
    void mousePressEvent(QMouseEvent* ev) override;
    void removeTagAtIndex(int);

public:
    explicit TagWidget(QWidget* parent = nullptr);
    ~TagWidget() override = default;

    QStringList tags();
    const QColor& color() const;
    void clearBlankTags();

public slots:

    void clear();
    void appendTags(const QStringList& tagList);
    void setTags(const QStringList& tagList);

    void setColor(const QColor& color);
    void setHorizontalMargin(int margin);
    void setVerticalMargin(int margin);
    void setHorizontalSpacing(int spacing);
    void setVerticalSpacing(int spacing);

protected slots:

    void tagChanged(TagChildWidget*);
    void tagCreated(TagChildWidget*);

signals:

    void tagsChanged(const QStringList&);
    void colorChanged();

protected:
    FlowLayout* flowLayout;

    QStringList currentTags;
    QList<TagChildWidget*> childWidgetList;

    QColor currentTagColor;
    int horizontalMargin;
    int verticalMargin;
};
}
