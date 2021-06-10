#!/bin/bash -e
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

# -- This script bumps the version to build in the repo.

# -- We invoked with -e, but also -e is set here for documentation.
#    It means we will terminate as soon as any unhandled error happens.
set -e

BUMP=""
unset FORCE_TAG
unset NXA_RUN_SCRIPT_IN_DEBUG

function usage {
    echo "usage: bump_version.sh [-M] [-m] [-r] [-f] [-s]" >&2
    echo "     -M Bump major by 1 and reset minor/revision to 0" >&2
    echo "     -m Bump minor by 1 and reset revision to 0" >&2
    echo "     -r Bump revision by 1" >&2
    echo "     -b Bump build by 1" >&2
    echo "     -f Force tag name (useful for updating tag if it already exists, old commit will be untagged)" >&2
    echo "     -s to run the script in debug mode (does not commit or upload results anywhere)" >&2
}

function die {
    echo "   ⛔️  $1" >&2
    exit 1
}

function ok {
    echo "   ✅  $1" >&2
}

function warn {
    echo "   ⚠️  $1" >&2
    exit 1
}

function require {
    which $1 > /dev/null || die "$1 required; not found in \$PATH/"
    ok $1
}

function check_git_clean {
    require git

    # -- Check that there's no changes on local working tree or index
    if [ ! $NXA_RUN_SCRIPT_IN_DEBUG ]; then
        # -- Make sure everything is tracked and committed
        if [[ $(git diff) ]]; then
            git ls-files --modified
            die "Some files are checked out."
        fi

        if [[ $(git ls-files --others --exclude-standard) ]]; then
            git ls-files --others --exclude-standard
            die "Some files are untracked."
        fi

        git update-index -q --ignore-submodules --refresh > /dev/null
        git diff-files --quiet --ignore-submodules -- || die "Unstaged changes in working tree."
        git diff-index --quiet HEAD -- || die "Uncommitted changes in index."
        ok "Working tree is clean."
    fi
}

function require_git_working_directory {
    # -- before doing anything with git, we have to check if is installed
    require git

    git rev-parse --git-dir >/dev/null 2>&1 || die "Working directory is not a git repo."
}

function export_paths {
    export PATH=$NXA_SOURCE_PATH/bin/macOS:$NXA_SOURCE_PATH/Vendor/Qt/macOS/bin:$PATH
    ok "NXA_SOURCE_PATH is $NXA_SOURCE_PATH"
}

function export_versions {
  if [ ! -f "${NXA_SOURCE_PATH}/Versionning/major" ]; then
    die "Could not find a major version number for this build."
  else
    export NXA_MAJOR_VERSION=$(cat "${NXA_SOURCE_PATH}/Versionning/major")
  fi

  if [ ! -f "${NXA_SOURCE_PATH}/Versionning/minor" ]; then
    die "Could not find a minor version number for this build."
  else
    export NXA_MINOR_VERSION=$(cat "${NXA_SOURCE_PATH}/Versionning/minor")
  fi

  if [ ! -f "${NXA_SOURCE_PATH}/Versionning/revision" ]; then
    die "Could not find a revision version number for this build."
  else
    export NXA_REVISION_VERSION=$(cat "${NXA_SOURCE_PATH}/Versionning/revision")
  fi

  if [ ! -f "${NXA_SOURCE_PATH}/Versionning/build" ]; then
    die "Could not find a build version number for this build."
  else
    export NXA_BUILD_NUMBER=$(cat "${NXA_SOURCE_PATH}/Versionning/build")
  fi

  if [ $NXA_REVISION_VERSION == 0 ]; then
    export NXA_VERSION_TO_BUILD="$NXA_MAJOR_VERSION.$NXA_MINOR_VERSION($NXA_BUILD_NUMBER)"
  else
    export NXA_VERSION_TO_BUILD="$NXA_MAJOR_VERSION.$NXA_MINOR_VERSION.$NXA_REVISION_VERSION($NXA_BUILD_NUMBER)"
  fi
}

NO_COMMIT_NEEDED=0

# -- Process any arguments
while getopts ":Mmrbfs" option; do
  case $option in
    s) NXA_RUN_SCRIPT_IN_DEBUG="Debug";;
    M) increases=(major build); resets=(minor revision build);;
    m) increases=(minor build); resets=(revision build);;
    r) increases=(revision build); resets=();;
    b) increases=(build); resets=(); NO_COMMIT_NEEDED=1;;
    f) FORCE_TAG="ON";;
    \?) usage; exit 1 ;;
  esac
done

[ -n "$increases" ] || die "Requires something to bump."

export_paths

# -- We're going to do some git stuff next, in the source directory
cd "$NXA_SOURCE_PATH"

require_git_working_directory
check_git_clean
export_versions

OLD_VERSION=$NXA_VERSION_TO_BUILD

if [ $NO_COMMIT_NEEDED == 0 ]; then
  message="Release v${NXA_VERSION_TO_BUILD} (skip-ci-builds)"

  if [ ! $NXA_RUN_SCRIPT_IN_DEBUG ]; then
    if [ -z $FORCE_TAG ]; then
      git tag --annotate "v$NXA_VERSION_TO_BUILD" --message "$message"
    else
      git tag --force --annotate "v$NXA_VERSION_TO_BUILD" --message "$message"
    fi

    ok "Tagged $message"
  else
    ok "Would have tagged $message"
  fi
fi

for reset in ${resets[*]}; do
    rm "$NXA_SOURCE_PATH/Versionning/$reset"
    echo "0" > "$NXA_SOURCE_PATH/Versionning/$reset"
done

for increase in ${increases[*]}; do
    old_version=$(cat "${NXA_SOURCE_PATH}/Versionning/$increase")
    new_version=$(($old_version + 1))

    rm "$NXA_SOURCE_PATH/Versionning/$increase"
    echo $new_version > "$NXA_SOURCE_PATH/Versionning/$increase"
done

export_versions

message="Update version to ${NXA_VERSION_TO_BUILD} after build ${OLD_VERSION} (skip-ci-builds)"

if [ ! $NXA_RUN_SCRIPT_IN_DEBUG ]; then
    git commit -m "$message" >/dev/null 2>&1
fi

ok "in commit $(git rev-parse --short HEAD): $message"
