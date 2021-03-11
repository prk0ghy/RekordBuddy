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

#include <Widgets/PlayerWidget.hpp>
#include <qmediaplayer.h>
#include <qmediacontent.h>
#include "ui_PlayerWidget.h"
#include "Widgets/moc_PlayerWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors
PlayerWidget::PlayerWidget(QWidget* parent) : QWidget{parent}, ui{new Ui::PlayerWidget}, player{new QMediaPlayer}
{
    ui->setupUi(this);
    connect(this->ui->songScroller, &QSlider::sliderPressed, this, &PlayerWidget::startScrolling);
    connect(this->ui->songScroller, &QSlider::sliderReleased, this, &PlayerWidget::stopScrolling);
    connect(this->ui->songScroller, &QSlider::sliderMoved, this, &PlayerWidget::scrollToPosition);
    connect(this->ui->playButton, &QAbstractButton::clicked, this, &PlayerWidget::clickTogglePlay);
    connect(this->player.get(), &QMediaPlayer::durationChanged, this, &PlayerWidget::durationChanged);
    connect(this->player.get(), &QMediaPlayer::positionChanged, this, &PlayerWidget::positionChanged);
    connect(this->player.get(), &QMediaPlayer::stateChanged, this, &PlayerWidget::mediaStateChanged);
    this->player->setAudioRole(QAudio::Role::MusicRole);
    this->ui->playButton->setEnabled(false);
    this->ui->songScroller->setEnabled(false);
}

PlayerWidget::~PlayerWidget()
{
    disconnect(this->ui->playButton, &QAbstractButton::clicked, this, &PlayerWidget::clickTogglePlay);
    disconnect(this->player.get(), &QMediaPlayer::durationChanged, this, &PlayerWidget::durationChanged);
    disconnect(this->player.get(), &QMediaPlayer::positionChanged, this, &PlayerWidget::positionChanged);
    disconnect(this->ui->songScroller, &QSlider::sliderPressed, this, &PlayerWidget::startScrolling);
    disconnect(this->ui->songScroller, &QSlider::sliderReleased, this, &PlayerWidget::stopScrolling);
    disconnect(this->ui->songScroller, &QSlider::sliderMoved, this, &PlayerWidget::scrollToPosition);
    disconnect(this->player.get(), &QMediaPlayer::stateChanged, this, &PlayerWidget::mediaStateChanged);
    delete ui.get();
    delete player.get();
}

void PlayerWidget::mediaStateChanged(QMediaPlayer::State state)
{
    switch (state) {
        case QMediaPlayer::State::PausedState:
            this->ui->playButton->setText(QStringLiteral("▶︎"));
            break;
        case QMediaPlayer::State::StoppedState:
            this->ui->playButton->setText(QStringLiteral("▶︎"));
            this->ui->songScroller->setValue(0);
            break;
        case QMediaPlayer::State::PlayingState:
            this->ui->playButton->setText(QStringLiteral("◼︎"));
            break;
    }
}

void PlayerWidget::startScrolling()
{
    this->scrolling = true;
}

void PlayerWidget::scrollToPosition(int position)
{
    this->player->setPosition(position);
}

void PlayerWidget::stopScrolling()
{
    this->scrolling = false;
}

void PlayerWidget::clickTogglePlay()
{
    player->setVolume(50); // TODO: maybe pull this from a user preference
    switch (player->state()) {
        case QMediaPlayer::State::StoppedState:
            if (!currentSelectionToLoad.isValid()) {
                return;
            }
            player->setMedia(QMediaContent{*currentSelectionToLoad});
            player->play();
            break;
        case QMediaPlayer::State::PausedState:
            player->play();
            break;
        case QMediaPlayer::State::PlayingState:
            player->pause();
            break;
    }
}

void PlayerWidget::durationChanged(qint64 duration)
{
    this->ui->songScroller->setMaximum(int(duration));
}

void PlayerWidget::positionChanged(qint64 position)
{
    if (!this->scrolling) {
        this->ui->songScroller->setValue(int(position));
    }
}

void PlayerWidget::selectPlayableTrack(const Optional<QUrl>& maybeUrl)
{
    player->stop();
    this->ui->playButton->setChecked(false);
    this->ui->songScroller->setMaximum(1);
    this->ui->songScroller->setValue(0);
    currentSelectionToLoad = maybeUrl;
    if (maybeUrl.isValid()) {
        this->ui->playButton->setEnabled(true);
        this->ui->songScroller->setEnabled(true);
    }
    else {
        this->ui->playButton->setEnabled(false);
        this->ui->songScroller->setEnabled(false);
    }
}
