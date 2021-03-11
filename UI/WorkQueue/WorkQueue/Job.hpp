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

#include <Base/Assert.hpp>

#define dec dec_qt
#include <qfuture.h>
#include <qfutureinterface.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#undef dec

#include <atomic>
#include <functional>
#include <future>
#include <type_traits>

namespace NxA { namespace RekordBuddy {

class Job : public QObject
{
    Q_OBJECT

protected:
    // -- Note: We have to use 'int' because QT's MOC tool (for signals and slots) need to parse these types when passing between threads.
    std::atomic<int> p_progress{ 0 };
    std::atomic<int> p_effort{ 1 };

public:
    // -- Constructors & Destructors
    explicit Job(int withEffort);
    Job() = delete;
    Job(const Job&) = delete;
    Job(Job&&) = delete;
    ~Job() override = default;

    // -- Operators
    Job& operator=(const Job&) = delete;

    // -- Instance methods
    virtual void runJob() = 0;

    int remainingEffort() const;
    int totalEffort() const;
    void updateStatus(const QString&);
    int progress();
    void setProgress(int);
    void addProgress(int);
    void incrementProgressByOne();
    void addEffort(int);

signals:
    void didAddJobEffort(int);
    void didBeginJob();
    void didFinishJob();
    void didProgressJob(int);
    void didUpdateStatus(const QString&);
};

class RunnableJob : public Job
{
    std::function<void(RunnableJob*)> job;

public:
    // -- Constructors & Destructors
    RunnableJob(int withEffort, std::function<void(RunnableJob*)>&& withJob) : Job{ withEffort }, job{ std::move(withJob) } { }

    // -- Instance methods
    void runJob() override
    {
        emit didBeginJob();
        emit didAddJobEffort(this->p_effort);

        this->job(this);

        emit didProgressJob(0);
        emit didUpdateStatus("");
        emit didFinishJob();
    }
};

template <typename T>
    class JobWithStdFuture : public Job
    {
        std::function<T(JobWithStdFuture*)> job;
        std::promise<T> promise;

    public:
        // -- Constructors & Destructors
        JobWithStdFuture(int withEffort, std::function<T(JobWithStdFuture*)>&& withJob) : Job{ withEffort }, job{ std::move(withJob) } { }

        // -- Instance methods
        void runJob() override
        {
            emit didBeginJob();
            emit didAddJobEffort(this->p_effort);

            auto result = this->job(this);

            promise.set_value(result);

            emit didFinishJob();
        }

        std::future<T> getFuture()
        {
            return this->promise.get_future();
        }
    };

template <typename T>
    class JobWithQFuture : public Job
    {
        QFutureInterface<T> resultOfJob{};
        std::function<T(JobWithQFuture*)> job;

    public:
        // -- Constructors & Destructors
        JobWithQFuture(int withEffort, std::function<T(JobWithQFuture*)>&& withJob) : Job{ withEffort }, job{ std::move(withJob) } { }

        // -- Instance methods
        void runJob() override
        {
            emit didBeginJob();

            if (this->p_effort) {
                emit didAddJobEffort(this->p_effort);
            }

            auto result = this->job(this);
            resultOfJob.reportFinished(&result);

            emit didFinishJob();
        }

        QFuture<T> getFuture()
        {
            return this->resultOfJob.future();
        }

    };

} }
