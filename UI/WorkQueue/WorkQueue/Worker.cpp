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

#include <WorkQueue/Job.hpp>
#include <WorkQueue/WorkQueue.hpp>
#include <WorkQueue/Worker.hpp>

#include <Base/Types.hpp>

#include <qqueue.h>
#include <qstringliteral.h>

class QObject;

// -- These are certainly not to be used outside of WorkQueue, so we 'hide' them in a namesapce that makes it clear
QMutex NxA::RekordBuddy::WorkQueueDetail::queueMutex{};
QWaitCondition NxA::RekordBuddy::WorkQueueDetail::jobsToProcess{};

using namespace NxA;
using namespace RekordBuddy;
using namespace WorkQueueDetail;

Worker::Worker(WorkQueue* forQueue, QObject* parent) : QThread{parent}, queue{forQueue}
{
    this->setObjectName(QStringLiteral("JobWorkerThread"));
    this->shouldRun = true;
    this->start();
}

Worker::~Worker()
{
    this->quit();
    this->wait();
}

boolean Worker::isWorkerRunning() const
{
    return this->isRunning;
}

QString Worker::currentStatus() const
{
    return this->status;
}

void Worker::run()
{
    while (this->shouldRun) {
        Job* nextJob{nullptr};
        {
            QMutexLocker locker{&queueMutex};
            jobsToProcess.wait(&queueMutex);
            if (this->shouldRun && !queue->queue.empty()) {
                nextJob = queue->queue.dequeue();
            }
        }

        if (nextJob == nullptr) {
            // -- shouldn't run, or queue empty
            continue;
        }
        isRunning = true;

        nextJob->moveToThread(this);

        connect(nextJob, &Job::didAddJobEffort, this, &Worker::workerDidAddJobEffort);
        connect(nextJob, &Job::didBeginJob, this, &Worker::workerDidBeginJob);
        connect(nextJob, &Job::didFinishJob, this, &Worker::workerDidFinishJob);
        connect(nextJob, &Job::didProgressJob, this, &Worker::workerDidProgressJob);
        connect(nextJob, &Job::didUpdateStatus, this, &Worker::didUpdateStatus);

        nextJob->runJob();

        disconnect(nextJob, &Job::didAddJobEffort, this, &Worker::workerDidAddJobEffort);
        disconnect(nextJob, &Job::didBeginJob, this, &Worker::workerDidBeginJob);
        disconnect(nextJob, &Job::didFinishJob, this, &Worker::workerDidFinishJob);
        disconnect(nextJob, &Job::didProgressJob, this, &Worker::workerDidProgressJob);
        disconnect(nextJob, &Job::didUpdateStatus, this, &Worker::didUpdateStatus);

        delete nextJob;
        isRunning = false;
    }
}

void Worker::didUpdateStatus(const QString& newStatus)
{
    this->status = newStatus;
    emit workerDidUpdateStatus(newStatus);
}

void Worker::startTakingJobs()
{
    this->shouldRun = true;
    jobsToProcess.wakeAll();
}

void Worker::stopTakingJobs()
{
    this->shouldRun = false;
    jobsToProcess.wakeAll();
}
