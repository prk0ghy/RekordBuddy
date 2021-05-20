#
#  Rekord Buddy - The future proof music collection tool made by DJs for DJs.
#  Copyright (C) 2020-2021 Didier Malenfant (didier@rekordbuddy.org)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.13)

if("${NXA_QT_DIR}" STREQUAL "")
    message(FATAL_ERROR "NXA_QT_DIR is not defined. Must point to your Qt install.")
endif()

set(CMAKE_PREFIX_PATH "${NXA_QT_DIR}")
message(STATUS "Defaulting CMAKE_PREFIX_PATH to '${CMAKE_PREFIX_PATH}'")

# -- Find some packages we use (must be before we add other subdirectories, or we run into problems)
find_package(Qt5 COMPONENTS Core Gui Multimedia Widgets REQUIRED)

if(${NXA_CMAKE_PLATFORM} STREQUAL "Windows")
    include(${NXA_QT_DIR}/windeployqt.cmake)
endif()

add_subdirectory(RkbBuildDefines)
add_subdirectory(UiBase)
add_subdirectory(UiModel)
add_subdirectory(UiMainWindow)
add_subdirectory(UiPreferences)
add_subdirectory(UiUpdater)
add_subdirectory(Widgets)
add_subdirectory(WorkQueue)
add_subdirectory(ViewCollection)
add_subdirectory(MutableViewCollection)
add_subdirectory(AbstractViewCollection)

nxa_project(RekordBuddy VERSION ${RKB_CMAKE_PACKAGE_VERSION})

if(NXA_CMAKE_BETA_BUILD)
    set(NXA_ICON_NAME "Rekord Buddy Beta")
else()
    set(NXA_ICON_NAME "Rekord Buddy")
endif()

set(ICON_QRC_FILE "Icons.qrc")

init_qt()

add_executable(RekordBuddy
               ${OS_BUNDLE}
               main.cpp

               ${ICON_QRC_FILE}
               Fonts.qrc
               style.qrc
               splash.qrc)

target_include_directories(RekordBuddy
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.)

set_target_properties(RekordBuddy PROPERTIES OUTPUT_NAME "Rekord Buddy")

target_add_metadata(RekordBuddy)
set_win32_executable(RekordBuddy)

if(${NXA_CMAKE_PLATFORM} STREQUAL "macOS")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Rekord Buddy")

    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${RKB_CMAKE_VERSION_STRING}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${RKB_CMAKE_VERSION_NUMBER}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${RKB_CMAKE_BUNDLE_VERSION}")

    set_target_properties(RekordBuddy PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/RekordBuddy.plist.in)
    target_sources(RekordBuddy PRIVATE RekordBuddy.plist.in)

    find_library(QuartzCoreFramework QuartzCore)
    find_library(CarbonFramework Carbon)
    find_library(CupsLib cups)
    find_library(AudioToolboxFramework AudioToolbox)
    find_library(AVFoundationFramework AVFoundation)
    find_library(CoreAudioFramework CoreAudio)
    find_library(AudioUnitLib AudioUnit)
    find_library(ApplicationServicesFramework ApplicationServices)
    find_library(OpenGLFramework OpenGL)
    find_library(SysConfFramework SystemConfiguration)

    mark_as_advanced(SysConfFramework AudioToolboxFramework CoreAudio AVFoundationFramework CarbonFramework
                     OpenGLFramework QuartzCoreFramework ApplicationServicesFramework)

    set(EXTRA_QT_REQUIREMENTS ${CupsLib} ${AudioUnitLib}
        ${SysConfFramework} ${OpenGLFramework} ${QuartzCoreFramework} ${CarbonFramework}
        ${ApplicationServicesFramework} ${CoreAudioFramework} ${AudioToolboxFramework} ${AVFoundationFramework})

    if(NOT "${NXA_BUGSNAG_DIR}" STREQUAL "")
        deploy_framework(RekordBuddy FROM "${NXA_BUGSNAG_DIR}" NAME "Bugsnag")
    endif()

    deploy_framework(RekordBuddy FROM "${NXA_TAGLIB_DIR}" NAME "tag") # VERSION 1.17.0)
    deploy_framework(RekordBuddy FROM "${NXA_VENDOR_DIR}/Sparkle/${NXA_CMAKE_PLATFORM}" NAME "Sparkle")

    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtCore" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtDBus" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtGui" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtMultimedia" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtMultimediaWidgets" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtNetwork" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtOpenGL" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtPrintSupport" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtSvg" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtWidgets" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtPositioning" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtQuick" VERSION "5")
    deploy_framework(RekordBuddy FROM "${NXA_QT_DIR}/lib" NAME "QtQuickWidgets" VERSION "5")

    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/audio" NAME "qtaudio_coreaudio" OUTSUBDIR "../Plugins/audio")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/bearer" NAME "qgenericbearer" OUTSUBDIR "../Plugins/bearer")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/iconengines" NAME "qsvgicon" OUTSUBDIR "../Plugins/iconengines")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qgif" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qicns" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qico" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qjpeg" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qmacheif" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qmacjp2" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qtga" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qtiff" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qwbmp" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/imageformats" NAME "qwebp" OUTSUBDIR "../Plugins/imageformats")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/mediaservice" NAME "qavfcamera" OUTSUBDIR "../Plugins/mediaservice")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/mediaservice" NAME "qavfmediaplayer" OUTSUBDIR "../Plugins/mediaservice")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/mediaservice" NAME "qtmedia_audioengine" OUTSUBDIR "../Plugins/mediaservice")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/platforms" NAME "qcocoa" OUTSUBDIR "../Plugins/platforms")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/printsupport" NAME "cocoaprintersupport" OUTSUBDIR "../Plugins/printsupport")
    inject_binary(RekordBuddy LIBRARY WITHOUT_PLATFORM FROM "${NXA_QT_DIR}/plugins/styles" NAME "qmacstyle" OUTSUBDIR "../Plugins/styles")

    fix_qt_deps(RekordBuddy "${NXA_QT_DIR}")
elseif(${NXA_CMAKE_PLATFORM} STREQUAL "Windows")
    target_link_libraries(RekordBuddy PUBLIC USER32.LIB Shlwapi.lib)
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

target_compile_definitions(RekordBuddy PRIVATE ${QT_DEFINITIONS})

target_link_libraries(RekordBuddy PRIVATE Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Widgets
                      Base Collections UiUpdater UiPreferences UiBase UiModel UiMainWindow
                      MutableViewCollection ViewCollection AbstractViewCollection WorkQueue Widgets)

if(${NXA_CMAKE_PLATFORM} STREQUAL "Windows")
    # -- Install commands are executed in order so if we the executable there for other command then this line needs to be first.
    install(TARGETS RekordBuddy RUNTIME DESTINATION .)
    inject_all_binaries_for(RekordBuddy)
    windeployqt(RekordBuddy ".")

    # -- Create the installer
    set(CPACK_GENERATOR WIX)
    set(CPACK_WIX_UPGRADE_GUID "86570623-d2e5-4bec-aa87-c079ce3b6dd4")
    set(CPACK_WIX_LICENSE_RTF "${NXA_INSTALLER_DIR}/Windows/eula.rtf")
    #set(CPACK_WIX_PRODUCT_ICON ???)

    set(CPACK_PACKAGE_NAME "Rekord Buddy")
    if(NXA_CMAKE_BETA_BUILD)
        set(CPACK_PACKAGE_FILE_NAME "Rekord Buddy Beta")
    else()
        set(CPACK_PACKAGE_FILE_NAME "Rekord Buddy")
    endif()
    set(CPACK_PACKAGE_CONTACT ${COMPANY})
    set(CPACK_PACKAGE_VENDOR ${COMPANY})
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ",${PROJECT_VERSION_PATCH}")
    # -- For now we don't use tweak versions since they don't match well with our beta numbers.
    set(CPACK_PACKAGE_VERSION_TWEAK "")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The future-proof music collection manager built by DJs, for DJS.")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Rekord Buddy")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Rekord Buddy")

    set_property(INSTALL "Rekord Buddy.exe" PROPERTY CPACK_START_MENU_SHORTCUTS "Rekord Buddy")

    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")

    include(CPack)
endif()

nxa_project_end()
