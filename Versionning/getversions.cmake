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

# -- Get some information on the current state of our local git repo.
execute_process(COMMAND git log -1 --format=%h
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE RKB_CMAKE_GIT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${RKB_CMAKE_GIT_COMMIT_HASH}" STREQUAL "")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(RKB_CMAKE_GIT_COMMIT_HASH "UNKNOWN")
    else()
        message(FATAL_ERROR "Couldn't find git commit hash (missing git executable?).")
    endif()
endif()

string(SUBSTRING ${RKB_CMAKE_GIT_COMMIT_HASH} 0 8 RKB_CMAKE_GIT_COMMIT_HASH)

execute_process(COMMAND git diff
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
    set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
else()
    execute_process(COMMAND git ls-files --modified
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
        set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
    else()
        execute_process(COMMAND git ls-files --others --exclude-standard
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

        if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
            set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
        else()
            execute_process(COMMAND git update-index -q --ignore-submodules --refresh > /dev/null
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                            OUTPUT_STRIP_TRAILING_WHITESPACE)

            if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
                set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
            else()
                execute_process(COMMAND git diff-files -q --ignore-submodules -- > /dev/null
                                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                                OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                                OUTPUT_STRIP_TRAILING_WHITESPACE)

                if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
                    set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
                else()
                    execute_process(COMMAND git diff-index --quiet HEAD -- > /dev/null
                                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                                    OUTPUT_VARIABLE NXA_TEMP_GIT_RESPONSE
                                    OUTPUT_STRIP_TRAILING_WHITESPACE)

                    if (NOT "${NXA_TEMP_GIT_RESPONSE}" STREQUAL "")
                        set(RKB_CMAKE_LOCAL_REPO_DIRTY "-dirty")
                    endif()
                endif()
            endif()
        endif()
    endif()
endif()

set(RKB_CMAKE_GIT_COMMIT_HASH "${RKB_CMAKE_GIT_COMMIT_HASH}${RKB_CMAKE_LOCAL_REPO_DIRTY}")

# -- Then we read our build numbers from the version files
execute_process(COMMAND cat major
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE RKB_CMAKE_MAJOR_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${RKB_CMAKE_MAJOR_VERSION}" STREQUAL "")
    message(FATAL_ERROR "Couldn't read our major version number.")
endif()

execute_process(COMMAND cat minor
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE RKB_CMAKE_MINOR_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${RKB_CMAKE_MINOR_VERSION}" STREQUAL "")
    message(FATAL_ERROR "Couldn't read our minor version number.")
endif()

execute_process(COMMAND cat revision
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE RKB_CMAKE_REVISION_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${RKB_CMAKE_REVISION_VERSION}" STREQUAL "")
    message(FATAL_ERROR "Couldn't read our revision version number.")
endif()

execute_process(COMMAND cat build
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE RKB_CMAKE_BUILD_NUMBER
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${RKB_CMAKE_BUILD_NUMBER}" STREQUAL "")
    message(FATAL_ERROR "Couldn't read our build version number.")
endif()

# -- Set up our project version number string. This needs to be used everywhere possible for consistency.
if("${RKB_CMAKE_REVISION_VERSION}" STREQUAL "0")
    set(RKB_CMAKE_VERSION_NUMBER "${RKB_CMAKE_MAJOR_VERSION}.${RKB_CMAKE_MINOR_VERSION}")
else()
    set(RKB_CMAKE_VERSION_NUMBER "${RKB_CMAKE_MAJOR_VERSION}.${RKB_CMAKE_MINOR_VERSION}.${RKB_CMAKE_REVISION_VERSION}")
endif()

set(RKB_CMAKE_BUNDLE_VERSION "${RKB_CMAKE_BUILD_NUMBER}")
set(RKB_CMAKE_VERSION_STRING "${RKB_CMAKE_VERSION_NUMBER}(${RKB_CMAKE_BUILD_NUMBER})")
set(RKB_CMAKE_VERSION_CRASHLOG_STRING "${RKB_CMAKE_VERSION_NUMBER}(${RKB_CMAKE_BUILD_NUMBER})")
set(RKB_CMAKE_SYMBOLS_VERSION_STRING "${RKB_CMAKE_MAJOR_VERSION}.${RKB_CMAKE_MINOR_VERSION}.${RKB_CMAKE_REVISION_VERSION}.${RKB_CMAKE_BUILD_NUMBER}")

# -- Package version must be major.minor[.revision] without any beta indicators or "v" prefix.
set(RKB_CMAKE_PACKAGE_VERSION "${RKB_CMAKE_VERSION_NUMBER}")
