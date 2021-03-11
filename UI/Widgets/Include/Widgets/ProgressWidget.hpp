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

#include <Base/NotNull.hpp>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qurl.h>
#include <qwidget.h>
#include <memory>
#include <atomic>

class QObject;

namespace Ui {
class ProgressWidget;
}

namespace NxA::RekordBuddy {

class WorkQueue;

class ProgressWidget : public QWidget
{
    Q_OBJECT

    NotNull<Ui::ProgressWidget*> ui;
    WorkQueue* queue{nullptr};
    std::atomic<boolean> waitingToShowProgress{false};
    QTimer* laggedUpdater{nullptr}, *periodicUpdater{nullptr};
    QString currentDescription{};
    int currentProgress{0};
    int currentEffort{0};

public:
    explicit ProgressWidget(QWidget* parent = nullptr);
    ~ProgressWidget() override;
    void associateWithWorkQueue(NotNull<WorkQueue*>);

private slots:
    // -- Progress slots
    void didBeginWorking();
    void didPauseWorking();
    void queueDidProgressOnEffort(int, int);
    void queueDidUpdateStatus(const QString&);
    void initialDelay();
    void timeoutForUpdate();
    void resetAfterWorking();
};
}
