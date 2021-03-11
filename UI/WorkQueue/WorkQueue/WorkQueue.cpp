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

#include <WorkQueue/WorkQueue.hpp>
#include <Base/NotNull.hpp>
#include <Base/Types.hpp>
#include <WorkQueue/Job.hpp>
#include <WorkQueue/Worker.hpp>

using namespace NxA;
using namespace RekordBuddy;
using namespace WorkQueueDetail;

WorkQueue::WorkQueue()
{
    {
        QMutexLocker locker{&queueMutex};
        for (auto i{0}; i < numberOfWorkers; ++i) {
            auto worker = new Worker{this};
            connect(worker, &Worker::workerDidAddJobEffort, this, &WorkQueue::workerDidAddJobEffort);
            connect(worker, &Worker::workerDidBeginJob, this, &WorkQueue::workerDidBeginJob);
            connect(worker, &Worker::workerDidFinishJob, this, &WorkQueue::workerDidFinishJob);
            connect(worker, &Worker::workerDidProgressJob, this, &WorkQueue::workerDidProgressJob);
            connect(worker, &Worker::workerDidUpdateStatus, this, &WorkQueue::workerDidUpdateStatus);
            this->workers.append(worker);
        }
    }
    this->startWorkers();
}

boolean WorkQueue::isWorking() const
{
    return !this->queue.empty() || this->anyRunning();
}

void WorkQueue::emitProgress()
{
    emit queueDidProgressOnEffort(this->accumulatedProgress, this->accumulatedEffort);
}

void WorkQueue::enqueueJob(NotNull<Job*> newJob)
{
    QMutexLocker locker{&queueMutex};
    if (!this->isWorking()) {
        this->accumulatedEffort = 0;
        this->accumulatedProgress = 0;
        emit this->didBeginWorking();
    }
    newJob->moveToThread(nullptr);
    this->queue.enqueue(newJob.get());
    jobsToProcess.wakeOne();
}

void WorkQueue::startWorkers()
{
    for (auto i{0}; i < numberOfWorkers; ++i) {
        emit this->workers[i]->startTakingJobs();
    }
}

void WorkQueue::stopWorkers()
{
    for (auto i{0}; i < numberOfWorkers; ++i) {
        emit this->workers[i]->stopTakingJobs();
    }
}

boolean WorkQueue::anyRunning() const
{
    for (auto i{0}; i < numberOfWorkers; ++i) {
        if (this->workers[i]->isWorkerRunning()) {
            return true;
        }
    }
    return false;
}

void WorkQueue::requestStatus()
{
    if (!this->isWorking()) {
        emit didPauseWorking();
        return;
    }
    emit didBeginWorking();
    this->workerDidProgressJob(0);
}

void WorkQueue::workerDidAddJobEffort(int effort)
{
    this->accumulatedEffort += effort;
    this->emitProgress();
}

void WorkQueue::workerDidBeginJob()
{
    this->emitProgress();
}

void WorkQueue::workerDidFinishJob()
{
    if (this->anyRunning()) {
        this->emitProgress();
        workerDidUpdateStatus("");
        WorkQueueDetail::jobsToProcess.wakeOne();
    }
    else {
        this->accumulatedEffort = 0;
        this->accumulatedProgress = 0;
        workerDidUpdateStatus("");
        this->emitProgress();
        emit didPauseWorking();
    }
}

void WorkQueue::workerDidUpdateStatus(const QString& status)
{
    QString newStatus{};
    for (auto i{0}; i < numberOfWorkers; ++i) {
        if (this->workers[i]->isWorkerRunning()) {
            auto curStatus = this->workers[i]->currentStatus();
            if (!curStatus.isEmpty()) {
                if (!newStatus.isEmpty()) {
                    newStatus += ", ";
                }

                newStatus += curStatus;
            }
        }
    }
    emit queueDidUpdateStatus(newStatus);
}

void WorkQueue::workerDidProgressJob(int progress)
{
    this->accumulatedProgress += progress;
    this->emitProgress();
}
