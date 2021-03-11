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

#include <WorkQueue/Job.hpp>

#include <Base/NotNull.hpp>
#include <Base/Types.hpp>

#include <functional>
#include <future>
#include <qfuture.h>
#include <qlist.h>
#include <qmutex.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqueue.h>
#include <qstring.h>
#include <qwaitcondition.h>
#include <type_traits>

namespace NxA { namespace RekordBuddy {

namespace {

// -- Constants
constexpr integer32 numberOfWorkers{ 4 };

}

namespace WorkQueueDetail {

// -- Forward Declarations
extern QWaitCondition jobsToProcess;
extern QMutex queueMutex;

}

// -- Forward Declarations
class Worker;

// -- Public Interface
class WorkQueue final : public QObject
{
    Q_OBJECT

    // -- Friends
    friend class Worker;

    // -- Instance Variables
    QQueue<Job*> queue{ };
    QList<Worker*> workers{ };
    int accumulatedProgress{ 0 }, accumulatedEffort{ 0 };

    void emitProgress();

private slots:
    // -- MOC requires 'int' for inter-thread signals/slot types
    void workerDidAddJobEffort(int);
    void workerDidBeginJob();
    void workerDidFinishJob();
    void workerDidProgressJob(int);
    void workerDidUpdateStatus(const QString&);

public:
    // -- Constructors & Destructors
    WorkQueue();
    WorkQueue(WorkQueue&&) = delete;
    WorkQueue(const WorkQueue&) = delete;
    ~WorkQueue() override = default;

    // -- Operators
    WorkQueue& operator=(const WorkQueue&) = delete;

    // -- Instance methods
    int currentProgress()
    {
        return this->accumulatedProgress;
    }
    int currentEffort()
    {
        return this->accumulatedEffort;
    }

    template <typename T>
        std::future<T> enqueueJobWithStdFuture(int effort, std::function<T(JobWithStdFuture<T>*)>&& funct)
        {
            auto runnable = new JobWithStdFuture<T>{ effort, std::move(funct) };
            this->enqueueJob(runnable);
            return runnable->getFuture();
        }
    template <typename T>
        QFuture<T> enqueueJobWithQFuture(int effort, std::function<T(JobWithQFuture<T>*)>&& funct)
        {
            auto runnable = new JobWithQFuture<T>{ effort, std::move(funct) };
            this->enqueueJob(runnable);
            return runnable->getFuture();
        }
    void enqueueJob(int effort, std::function<void(RunnableJob*)>&& funct)
    {
        auto runnable = new RunnableJob{ effort, std::move(funct) };
        this->enqueueJob(runnable);
    }
    void enqueueJob(NotNull<Job*>);

    boolean isWorking() const;
    boolean anyRunning() const;

    void startWorkers();
    void stopWorkers();
    void requestStatus();

signals:
    void didBeginWorking();
    void didPauseWorking();
    void queueDidProgressOnEffort(int progress, int effort);
    void queueDidUpdateStatus(const QString&);
};

} }
