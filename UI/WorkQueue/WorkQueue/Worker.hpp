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

#include <Base/Types.hpp>

#include <qmutex.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qthread.h>
#include <qwaitcondition.h>
#include <atomic>

class QObject;
namespace NxA::RekordBuddy {

class WorkQueue;

namespace WorkQueueDetail {
extern QWaitCondition jobsToProcess;
extern QMutex queueMutex;
}

// Workers are long-lived and operate on a remote thread
// they poll the work queue for work and atomically take move them into theirselves in order to process them.
// They connect their events up to the parent work queue as well.
class Worker final : public QThread
{
    Q_OBJECT

    std::atomic<bool> shouldRun{false}, isRunning{false};
    QString status;
    WorkQueue* queue;

public:
    // -- Constructors and Destructor

    Worker(WorkQueue* forQueue, QObject* parent = nullptr);
    ~Worker() override;

    Worker(Worker&&) = delete;
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    boolean isWorkerRunning() const;
    QString currentStatus() const;

    // -- Protected instances
protected:
    void run() override;

    // -- Signals & Slots

signals:
    void workerDidAddJobEffort(int);
    void workerDidBeginJob();
    void workerDidFinishJob();
    void workerDidProgressJob(int);
    void workerDidUpdateStatus(const QString&);

public slots:
    void startTakingJobs();
    void didUpdateStatus(const QString&);
    void stopTakingJobs();
};
}
