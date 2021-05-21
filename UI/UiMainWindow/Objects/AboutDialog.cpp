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

#include <UiMainWindow/AboutDialog.hpp>

#include <Base/Duration.hpp>

#include "rkb_build_defines.h"

#define dec QT_dec
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QLayout>
#undef dec

#include "ui_AboutDialog.h"
#include "UiMainWindow/moc_AboutDialog.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

AboutDialog::AboutDialog(WorkQueue& withWorkQueue, QWidget* parent, Mode withMode) : QDialog{ parent, Qt::FramelessWindowHint },
                                                                                     p_ui{ new Ui::AboutDialog },
                                                                                     p_usageMode{ withMode }
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    this->setObjectName("AboutDialog");

    this->p_ui->setupUi(this);

    // -- Populate version number of about dialog with version of program.
    this->p_ui->versionLabel->setAttribute(Qt::WA_TranslucentBackground);
    this->p_ui->versionLabel->setText(QString{ QObject::tr("v%1 (%2)") }.arg(RKB_VERSION_STRING).arg(RKB_GIT_COMMIT_HASH));

#if !defined(NXA_BETA_BUILD)
    this->p_ui->topWarningTape->setVisible(false);
    this->p_ui->bottomWarningTape->setVisible(false);
#endif

    switch (this->p_usageMode) {
        case Mode::About: {
            QFile acknowledgementsFile{ ":/splash/rc/Splash/Acknowledgements.htm" };
            acknowledgementsFile.open(QFile::ReadOnly | QFile::Text);
            QTextStream fileParser{ &acknowledgementsFile };
            this->p_ui->acknowledgements->setText(fileParser.readAll());

            this->p_ui->progressWidget->setVisible(false);
            break;
        }
        case Mode::Splash: {
            this->p_ui->progressWidget->setVisible(true);
            this->p_ui->progressWidget->associateWithWorkQueue(&withWorkQueue);

            this->p_ui->acknowledgements->setVisible(false);
            break;
        }
    }

}

AboutDialog::~AboutDialog()
{
    delete this->p_ui.get();
}

// -- Instance Methods

void AboutDialog::dismissWindow()
{
    this->close();
}

// -- Overridden Instance Methods

void AboutDialog::keyPressEvent(QKeyEvent* keyEvent)
{
    if (p_usageMode != Mode::About) {
        QDialog::keyPressEvent(keyEvent);
        return;
    }

    if (keyEvent->key() == Qt::Key_Escape) {
        this->dismissWindow();
    }

    QDialog::keyPressEvent(keyEvent);
}

void AboutDialog::mouseReleaseEvent(QMouseEvent* event)
{
    if (p_usageMode != Mode::About) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        this->dismissWindow();
    }

    QDialog::mouseReleaseEvent(event);
}
