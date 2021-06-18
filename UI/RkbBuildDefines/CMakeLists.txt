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

# -- This project allows us to output some defines that get updated when needed by triggering a re-config from cmake.

nxa_project(RkbBuildDefines)

# -- Crash Symbols and Receipt Activation version strings below must be compatible with
# -- being added to a URL, not all characters are supported.
file(WRITE ${CMAKE_BINARY_DIR}/rkb_build_defines.h.in
     "\#define RKB_GIT_COMMIT_HASH \"@RKB_CMAKE_GIT_COMMIT_HASH@\"\n"
     "\#define RKB_VERSION_STRING \"@RKB_CMAKE_VERSION_STRING@\"\n"
     "\#define RKB_CRASH_SYMBOLS_VERSION_STRING \"@RKB_CMAKE_SYMBOLS_VERSION_STRING@\"\n"
     "\#define RKB_MAJOR_VERSION \"@RKB_CMAKE_MAJOR_VERSION@\"\n"
     "\#define RKB_MINOR_VERSION \"@RKB_CMAKE_MINOR_VERSION@\"\n")

file(WRITE ${CMAKE_BINARY_DIR}/rkb_build_defines.cmake
     "list(APPEND CMAKE_MODULE_PATH \"${CMAKE_BINARY_DIR}/../../Versionning\")
      set(NXA_CMAKE_BETA_BUILD ${NXA_CMAKE_BETA_BUILD})
      set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
      include(getversions)
      configure_file(\${SRC} \${DST} @ONLY)")

add_custom_target(RkbBuildDefines ${CMAKE_COMMAND}
                  -D SRC=${CMAKE_BINARY_DIR}/rkb_build_defines.h.in
                  -D DST=${CMAKE_BINARY_DIR}/rkb_build_defines.h
                  -P ${CMAKE_BINARY_DIR}/rkb_build_defines.cmake)

nxa_project_end("NO_EXTRA_COMPILE_DEFINITIONS")
