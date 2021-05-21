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

#include <RekordBuddyCollection/RekordBuddy.hpp>
#include <RekordBuddyCollection/UserPreferences.hpp>

#include <RekordboxCollection/Rekordbox.hpp>

#include <UiMainWindow/MainWindow.hpp>
#include <UiMainWindow/AboutDialog.hpp>

#include <UiModel/Preferences/PreferencesModuleManager.hpp>

#include <UiPreferences/PreferencesCategoryGeneral.hpp>
#include <UiPreferences/PreferencesCategoryRekordbox.hpp>
#include <UiPreferences/PreferencesCategorySerato.hpp>
#include <UiPreferences/PreferencesCategoryTraktor.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <UiBase/Types.hpp>
#include <UiBase/AlertMessageBox.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <Base/CrashLog.hpp>
#include <Base/Directory.hpp>
#include <Base/Duration.hpp>
#include <Base/FilePath.hpp>
#include <Base/Threading.hpp>

#include "rkb_build_defines.h"

#if defined(NXA_BETA_BUILD)
#include <UiMainWindow/BetaSoftwareWarning.hpp>
#else
#include <UiMainWindow/WelcomeDialog.hpp>
#endif

#define dec QT_dec
#include <QApplication>
#include <QFontDatabase>
#include <QTextStream>
#include <QFile>
#include <QStyleFactory>
#include <QThread>
#include <QtPlugin>
#undef dec

using namespace NxA;
using namespace NxA::RekordBuddy;

static void loadPreferences()
{
    auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());

    // -- Populate with userPreferences in case any unset preferences is requested.
    PreferencesCategoryGeneral::registerDefaultPreferenceValuesIn(userPreferences);
    PreferencesCategoryRekordbox::registerDefaultPreferenceValuesIn(userPreferences);
    PreferencesCategorySerato::registerDefaultPreferenceValuesIn(userPreferences);
    PreferencesCategoryTraktor::registerDefaultPreferenceValuesIn(userPreferences);
    PreferencesModuleManager::migrateUserPreferencesValuesIn(userPreferences);

    // -- Then load the user preferences panels using those preferences.
    PreferencesModuleManager::loadPreferencesModule(std::make_unique<PreferencesCategoryGeneral>(userPreferences));
    PreferencesModuleManager::loadPreferencesModule(std::make_unique<PreferencesCategoryRekordbox>(userPreferences));
    PreferencesModuleManager::loadPreferencesModule(std::make_unique<PreferencesCategorySerato>(userPreferences));
    PreferencesModuleManager::loadPreferencesModule(std::make_unique<PreferencesCategoryTraktor>(userPreferences));
}

static void checkForRunningApp()
{
#if !defined(DEBUG)
    for (auto&& appName : Platform::namesOfOtherRunningApplications()) {
        for (count preferenceManagerIndex = 0; preferenceManagerIndex < PreferencesModuleManager::numberAvailableModules(); ++ preferenceManagerIndex) {
            auto preferencesManager = PreferencesModuleManager::getModuleInterface(preferenceManagerIndex);
            if (preferencesManager->appClashesWithAppNamed(appName)) {
                Platform::alertWithTextAndInformativeText(String::stringWithFormat("It looks like %s is currently running. Running it together with Rekord Buddy could lead to data corruption.", preferencesManager->CategoryName().asUTF8()),
                                                          "Please close the app and try again."_String);
                exit(0);
            }
        }
    }
#endif
}

static void loadApplicationFonts(QApplication& app)
{
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-Regular.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-Medium.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-MediumItalic.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-Bold.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-BoldItalic.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-Black.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/Fonts/Inter-TTF-hinted/Inter-UI-Italic.ttf"));
    app.setFont(QFont{ QStringLiteral("Inter UI"), NXA_NORMAL_FONT, QFont::Normal, false });
}

static int setupApplicationStylesheet(QApplication& app)
{
    loadApplicationFonts(app);

    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(218, 130, 42));
    darkPalette.setColor(QPalette::Highlight, QColor(218, 130, 42));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);
    app.setStyleSheet(R"(
                      QToolTip { color: #ffffff; background-color: #da822a; border: 1px solid white; }
                      QHeaderView::section {padding:6px 6px;}
                      QListView::item, QTreeView::item {padding:4px;}
                      QListView::item:selected, QTreeView::item:selected { padding:4px; }
                      )");

    return 0;
}

void setupThreading()
{
    // -- We setup our threading glue code to use Qt's threading model.
    Threading::setIsOnMainThreadFunctionToUse([]() {
        return (QThread::currentThread() == QCoreApplication::instance()->thread());
    });

    Threading::setRunOnMainThreadFunctionToUse([](std::function<void(void)> function,
                                                  Threading::Blocking blocking) {
        auto timer = new QTimer();
        timer->moveToThread(qApp->thread());
        timer->setSingleShot(true);

        Semaphore doneRunningFunction;
        NxA::boolean checkMe = false;

        QObject::connect(timer, &QTimer::timeout, [function, timer, blocking, &doneRunningFunction, &checkMe]() {
            function();

            timer->deleteLater();

            if (blocking == Threading::Blocking::Yes) {
                checkMe = true;

                // -- If we're not blocking then this semaphore will probably be destroyed by now
                // -- plus we don't need to notify in this case.
                doneRunningFunction.notify();
            }
        });

        QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));

        if (blocking == Threading::Blocking::Yes) {
            // -- We should be able to use ConnectionType::BlockingQueuedConnection instead of this here but
            // -- I ran into so odd behaviors while debugging an issue and this seemed to guarantee the block.
            // -- To be revisited later for sure.
            doneRunningFunction.wait();

            NXA_ASSERT_TRUE(checkMe);
        }
    });

    Threading::setSleepInMillisecondsFunctionToUse([](count milliseconds) {
        QThread::currentThread()->msleep(milliseconds);
    });
}

int main(int argc, char* argv[])
{
#if defined(NXA_BUGSNAG_APP_ID)
#if defined(NXA_PLATFORM_MACOS)
    CrashLog::initWithAPIKeyAndAppVersion({ NXA_BUGSNAG_APP_ID }, String{ RKB_VERSION_CRASHLOG_STRING });
#elif defined(NXA_PLATFORM_WINDOWS)
    CrashLog::setAppBundleID("org.rekordbuddy.RekordBuddy");
    CrashLog::initWithAPIKeyAndAppVersion({ NXA_BUGSNAG_APP_ID }, String{ RKB_VERSION_CRASHLOG_STRING });
#else
    #error Unsupported platform.
#endif

    if (CrashLog::hasCrashedInLastSession()) {
        Platform::alertWithTextAndInformativeText("It looks like you encountered a crash during the last session."_String,
                                                  "A crash log will be sent to help us identify the issue."_String);
    }
#endif

    Rekordbox::setCurrentRekordBuddyVersion(String::stringWithFormat("%s (%s)", RKB_VERSION_STRING, RKB_GIT_COMMIT_HASH));

    auto osVersionString = Platform::osVersion();
    auto components = osVersionString.splitBySeparator('.');
#if defined(NXA_PLATFORM_MACOS)
    if ((components.length() < 2) ||
        (components[0].integerValue() < 10) ||
        (components[0].integerValue() > 11) ||
        ((components[0].integerValue() == 10) && (components[1].integerValue() < 13))) {
        Platform::alertWithTextAndInformativeText("Rekord Buddy only supports macOS versions between 10.13 and 11.x."_String,
                                                  String::stringWithFormat("You seem to be running version %s.", osVersionString.asUTF8()));
        exit(0);
    }
#elif defined(NXA_PLATFORM_WINDOWS)
    if (!((components.length() >= 2) && (components[0].integerValue() >= 10))) {
        Platform::alertWithTextAndInformativeText("Rekord Buddy only supports Windows 10 and above."_String,
                                                  String::stringWithFormat("You seem to be running version %s.", osVersionString.asUTF8()));
        exit(0);
    }
#endif

#if !defined(NXA_RELEASE_BUILD_FOR_DEBUGGING)
#if !defined(DEBUG)
    if (!Platform::isRunningFromInstallationFolder()) {
#if defined(NXA_PLATFORM_MACOS)
        Platform::alertWithTextAndInformativeText("Rekord Buddy needs to run from your Application folder."_String,
                                                  "This is a macOS requirement for signed applications."_String);
#endif
        exit(0);
    }
#endif
#endif

    // -- Start QT core
    QCoreApplication::setOrganizationName("Didier Malenfant");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{ argc, argv };
    QApplication::setQuitOnLastWindowClosed(true);

    // -- Early things that require QT to be loaded, but before app starts loading
    if (setupApplicationStylesheet(app) != 0) {
        return 1;
    }

#if defined(NXA_PLATFORM_WINDOWS)
    for (auto&& volume : Volume::availableVolumes()) {
        auto logString = String::stringWithFormat("Found volume '%s' at '%s'.",
                                                  volume.name().asUTF8(),
                                                  volume.asFilePath().asEncodedString().asUTF8());
#if defined(NXA_BUGSNAG_APP_ID)
        CrashLog::addBreadCrumb(logString);
#endif
        NXA_BETA_LOG_WITH_FORMAT("%s", logString.asUTF8());
    }
#endif

    // -- Stuff before QT starts; like crash handlers and work queue
    WorkQueue workQueue;

    setupThreading();

    loadPreferences();

    checkForRunningApp();

    if (QApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier)) {
        if (ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Reset Database?"),
                                                             QObject::tr("Do you want to reset your main Rekord Buddy library?"),
                                                             QObject::tr("Your collection on the volume where your music folder is will be lost."))) {
            RekordBuddy::deleteRekordBuddyCollectionOnVolume(Volume::musicFolderVolume());
        }
    }

    auto mainWindow = Shared<MainWindow>::with(workQueue);
    MainWindow* mainWindowPointer = mainWindow.asRawPointer();

    auto splash = new AboutDialog{ workQueue, nullptr, AboutDialog::Mode::Splash };
    splash->show();

    mainWindow->setupWindowThenCall([mainWindowPointer, splash] {
        QTimer::singleShot(0, mainWindowPointer, [mainWindowPointer, splash]() {
            // -- This preforms tasks that require it to be on the main thread.
            NXA_ASSERT_TRUE_DEBUG(Threading::isOnMainThread());

            mainWindowPointer->show();

#if defined(NXA_BETA_BUILD)
            auto betaSoftwareWarning = new BetaSoftwareWarning{ mainWindowPointer };
            betaSoftwareWarning->showThenCallWhenClosed([betaSoftwareWarning]() {
                betaSoftwareWarning->deleteLater();
            });
#else
            if (WelcomeDialog::shouldBeDisplayed()) {
                auto welcomeDialog = new WelcomeDialog{ mainWindowPointer };
                welcomeDialog->showThenCallWhenClosed([welcomeDialog]() {
                    welcomeDialog->deleteLater();
                });
            }
#endif

            splash->close();
            splash->deleteLater();
        });
    });

    return app.exec();
}
