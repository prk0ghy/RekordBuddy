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

#include <Base/Assert.hpp>

#include <qapplication.h>
#include <qnamespace.h>
#include <qtimer.h>

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

Job::Job(int withEffort) : p_effort{ withEffort } { }

// -- Instance Methods

int Job::totalEffort() const
{
    return this->p_effort;
}

int Job::remainingEffort() const
{
    return this->p_effort - this->p_progress;
}

void Job::addEffort(int amount)
{
    if (amount > 0) {
        this->p_effort += amount;

        emit didAddJobEffort(amount);
        emit didProgressJob(0);
    }
}

void Job::updateStatus(const QString& newStatus)
{
    emit didUpdateStatus(newStatus);
}

int Job::progress()
{
    return this->p_progress;
}

void Job::setProgress(int newValue)
{
    if ((newValue >= 0) && (newValue <= this->p_effort)) {
        int oldProgress = this->p_progress.exchange(newValue);
        emit didProgressJob(newValue - oldProgress);
    }
}

void Job::addProgress(int amount)
{
    if ((amount >= 0) && ((amount + this->p_progress) <= this->p_effort)) {
        this->p_progress += amount;
        emit didProgressJob(amount);
    }
}

void Job::incrementProgressByOne()
{
    if (this->p_progress < this->p_effort) {
        ++this->p_progress;
        emit didProgressJob(1);
    }
}
