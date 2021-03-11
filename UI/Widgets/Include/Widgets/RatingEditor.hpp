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

#include <CommonCollection/Tracks/TrackRating.hpp>

#include <Base/NotNull.hpp>

#include <qobjectdefs.h>
#include <qwidget.h>

namespace Ui {

class RatingEditor;

}

class QLabel;
class QObject;

namespace NxA::RekordBuddy {

class FlowLayout;
class TagChildWidget;

class RatingEditor : public QWidget
{
    Q_OBJECT
    NotNull<Ui::RatingEditor*> ui;
    Common::TrackRating currentRating;

public:
    Q_PROPERTY(Common::TrackRating rating READ rating WRITE setRating NOTIFY ratingChanged)

    explicit RatingEditor(QWidget* parent = nullptr);
    ~RatingEditor() override = default;

    Common::TrackRating rating();
    void setRating(const Common::TrackRating& newValue);

signals:
    void ratingChanged();

protected:
    void mouseReleaseEvent(QMouseEvent *ev) override;
};

}
