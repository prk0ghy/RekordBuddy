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

#include <Widgets/RatingEditor.hpp>

#include <Base/Assert.hpp>
#include <Base/NotNull.hpp>

#include <qevent.h>

#include "ui_RatingEditor.h"
#include "Widgets/moc_RatingEditor.cpp"

using namespace NxA;
using namespace RekordBuddy;

RatingEditor::RatingEditor(QWidget* parent) : QWidget{ parent }, ui{ new Ui::RatingEditor }
{
    this->ui->setupUi(this);
    this->setRating(Common::TrackRating{ Common::TrackRating::Stars::Zero });
}

void RatingEditor::mouseReleaseEvent(QMouseEvent *ev)
{
    auto which = static_cast<integer64>(255 * double(ev->x()) / this->width()) - 14;
    auto maybeNewRating = Common::TrackRating::maybeWithValue(which);
    this->setRating(maybeNewRating.valueOr(Common::TrackRating::Stars::Zero));
    QWidget::mouseReleaseEvent(ev);
}

Common::TrackRating RatingEditor::rating()
{
    return this->currentRating;
}

void RatingEditor::setRating(const Common::TrackRating& newValue)
{
    if (newValue == this->currentRating) {
        return;
    }

    switch (newValue.asStarCount()) {
        case Common::TrackRating::Stars::Zero: {
            this->ui->star1->setText("・");
            this->ui->star2->setText("・");
            this->ui->star3->setText("・");
            this->ui->star4->setText("・");
            this->ui->star5->setText("・");
            break;
        }
        case Common::TrackRating::Stars::One: {
            this->ui->star1->setText("⭐️");
            this->ui->star2->setText("・");
            this->ui->star3->setText("・");
            this->ui->star4->setText("・");
            this->ui->star5->setText("・");
            break;
        }
        case Common::TrackRating::Stars::Two: {
            this->ui->star1->setText("⭐️");
            this->ui->star2->setText("⭐️");
            this->ui->star3->setText("・");
            this->ui->star4->setText("・");
            this->ui->star5->setText("・");
            break;
        }
        case Common::TrackRating::Stars::Three: {
            this->ui->star1->setText("⭐️");
            this->ui->star2->setText("⭐️");
            this->ui->star3->setText("⭐️");
            this->ui->star4->setText("・");
            this->ui->star5->setText("・");
            break;
        }
        case Common::TrackRating::Stars::Four: {
            this->ui->star1->setText("⭐️");
            this->ui->star2->setText("⭐️");
            this->ui->star3->setText("⭐️");
            this->ui->star4->setText("⭐️");
            this->ui->star5->setText("・");
            break;
        }
        case Common::TrackRating::Stars::Five: {
            this->ui->star1->setText("⭐️");
            this->ui->star2->setText("⭐️");
            this->ui->star3->setText("⭐️");
            this->ui->star4->setText("⭐️");
            this->ui->star5->setText("⭐️");
            break;
        }
        default: {
            NXA_ALOG("Invalid rating");
        }
    }

    this->currentRating = newValue;

    emit ratingChanged();
}
