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

# -- This script codesign an app bundle, including ant frameworks or
# -- executables found inside it.

# -- We invoked with -e, but also -e is set here for documentation.
#    It means we will terminate as soon as any unhandled error happens.
set -e

function ok {
    echo "   ✅  $1" >&2
}

function require {
    which $1 > /dev/null || die "$1 required; not found in \$PATH/"
    ok $1
}

initial_folder=$PWD
identity=$2
bundle_id=$3

cd $initial_folder

if [ -z ${TERM+x} ]; then
    export TERM=dumb
fi

require codesign

function usage {
    echo "usage: codesign_deep.sh [-h] <path> <certificate> <app_bundle_id>" >&2
    echo "     -h This help message." >&2
}

function codesign_in {
    do_codesign=0

    if [ -d "$1" ]; then
        if [ "${1: -8}" == "/Headers" ]; then
            rm -rf "$1"
            ok "Headers at '$1' removed."
        elif [ "${1: -15}" == "/PrivateHeaders" ]; then
            rm -rf "$1"
            ok "Headers at '$1' removed."
        elif [ ! -L "$1" ]; then
            if [ "${1: -4}" == ".app" ]; then
                for entry in "$1/"*; do
                    parent_app=$(basename "$1")
                    parent_app=${parent_app%.app}
                    codesign_in "$entry" "$(basename "$1")" "$parent_app"
                done
            else
                for entry in "$1/"*; do
                    codesign_in "$entry" "$(basename "$1")" "$3"
                done
            fi

            do_codesign=0

            if [ "${1: -10}" == ".framework" ]; then
                do_codesign=1
            elif [ "${1: -4}" == ".app" ]; then
                do_codesign=1
            fi
        fi
    elif [ "${1: -6}" == ".dylib" ]; then
        do_codesign=1
    elif [ "$2" == "MacOS" ]; then
        do_codesign=0
        if [ ! "$(basename "$1")" == "$3" ]; then
            do_codesign=1
        fi
    fi

    if [ $do_codesign == 1 ]; then
        codesign --verbose=4 --timestamp --options=runtime --force -s "$identity" -i $bundle_id "$1"
        ok "$(basename "$1") code signed."
        codesign -vv --deep --strict "$1"
        ok "$(basename "$1") code signing verified."
    fi
}

# -- Process any arguments
while getopts ":h" option; do
    case $option in
        h) usage; exit 1;;
    esac
done

case $1 in
    /*) absolute=$1;;
    *) absolute=$PWD/$1;;
esac

codesign_in "$absolute"
