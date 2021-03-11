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

#include <Widgets/ProgressWidget.hpp>
#include <WorkQueue/WorkQueue.hpp>
#include <qmediacontent.h>
#include <qtimer.h>
#include "ui_ProgressWidget.h"
#include "Widgets/moc_ProgressWidget.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors
ProgressWidget::ProgressWidget(QWidget* parent) : QWidget{parent}, ui{ new Ui::ProgressWidget }
{
    this->ui->setupUi(this);
    this->periodicUpdater = new QTimer{ this };
    this->laggedUpdater = new QTimer{ this };

    connect(laggedUpdater, &QTimer::timeout, this, &ProgressWidget::initialDelay);
    connect(periodicUpdater, &QTimer::timeout, this, &ProgressWidget::timeoutForUpdate);

    this->periodicUpdater->start(30);
    this->laggedUpdater->setSingleShot(true);
    this->didPauseWorking();
}

ProgressWidget::~ProgressWidget()
{
    delete this->laggedUpdater;
    delete this->periodicUpdater;
    delete this->ui.get();
}

void ProgressWidget::associateWithWorkQueue(NotNull<WorkQueue*> newQueue)
{
    if (this->queue != nullptr) {
        disconnect(this->queue, &WorkQueue::didBeginWorking, this, &ProgressWidget::didBeginWorking);
        disconnect(this->queue, &WorkQueue::didPauseWorking, this, &ProgressWidget::didPauseWorking);
        disconnect(this->queue, &WorkQueue::queueDidUpdateStatus, this, &ProgressWidget::queueDidUpdateStatus);
        disconnect(this->queue, &WorkQueue::queueDidProgressOnEffort, this, &ProgressWidget::queueDidProgressOnEffort);
    }

    this->queue = newQueue.get();

    // -- Connect to the work queue and reflect it's state
    connect(this->queue, &WorkQueue::didBeginWorking, this, &ProgressWidget::didBeginWorking);
    connect(this->queue, &WorkQueue::didPauseWorking, this, &ProgressWidget::didPauseWorking);
    connect(this->queue, &WorkQueue::queueDidUpdateStatus, this, &ProgressWidget::queueDidUpdateStatus);
    connect(this->queue, &WorkQueue::queueDidProgressOnEffort, this, &ProgressWidget::queueDidProgressOnEffort);

    this->queue->requestStatus();
}

void ProgressWidget::initialDelay()
{
    if (!this->waitingToShowProgress) {
        return;
    }

    this->waitingToShowProgress = false;

    this->ui->progressBar->setEnabled(true);
    this->ui->progressBar->setMinimum(0);
    this->ui->progressBar->setValue(this->currentProgress);
    this->ui->progressBar->setMaximum(this->currentEffort);
    this->ui->label->setText(QStringLiteral(""));
}

void ProgressWidget::didBeginWorking()
{
    QMetaObject::invokeMethod(this->ui->label, "setText", Q_ARG(QString, QStringLiteral("")));
    if (this->waitingToShowProgress) {
        return;
    }

    this->waitingToShowProgress = true;

    QMetaObject::invokeMethod(this->laggedUpdater, "start", Q_ARG(int, 250));
}

void ProgressWidget::resetAfterWorking()
{
    this->currentDescription = QStringLiteral("");
    this->currentEffort = 1;
    this->currentProgress = 0;
    this->ui->progressBar->reset();
    this->ui->progressBar->setMinimum(0);
    this->ui->progressBar->setMaximum(1);
    this->ui->progressBar->setEnabled(false);
    this->ui->label->setText(QStringLiteral(""));
}

void ProgressWidget::didPauseWorking()
{
    this->waitingToShowProgress = false;

    QMetaObject::invokeMethod(this, "resetAfterWorking");
}

void ProgressWidget::queueDidUpdateStatus(const QString& newStatus)
{
    this->currentDescription = newStatus;
}

void ProgressWidget::timeoutForUpdate()
{
    static int lastEffort = -1;
    static int lastProgress = -1;
    static QString lastDescription = QStringLiteral("");

    if (this->waitingToShowProgress) {
        return;
    }

    if (lastEffort != this->currentEffort) {
        lastEffort = this->currentEffort;
        this->ui->progressBar->setMaximum(this->currentEffort);
    }

    if (lastProgress != this->currentProgress) {
        lastProgress = this->currentProgress;
        this->ui->progressBar->setValue(this->currentProgress);
    }

    if (lastDescription != this->currentDescription) {
        lastDescription = this->currentDescription;
        this->ui->label->setText(this->currentDescription);
    }
}

void ProgressWidget::queueDidProgressOnEffort(int progress, int effort)
{
    this->currentProgress = progress;
    this->currentEffort = effort;
}
