#! /usr/bin/python3
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

# -- This script fixes the dependencies/runtime paths to all the Qt
# -- libraries found in an app bundle.

import os
import re
from subprocess import Popen, PIPE


def is_library(file):
    if os.path.isdir(file) or os.path.islink(file):
        return False

    is_at_root_of_frameworks_dir = re.search("Frameworks/[^/]+$", file) is not None

    # mode is 755
    return  is_at_root_of_frameworks_dir or os.stat(file).st_mode == 33261


def list_rpaths(program):
    proc = Popen(["otool", "-l", program], stderr=PIPE, stdout=PIPE)
    out, err = proc.communicate()

    if err:
        raise RuntimeError("otool error: " + err)

    outArr = filter(
        lambda s: s.startswith("         name "),
        out.decode("utf-8").split("\n")
    )
    return list(set(map(
        lambda s: s[1:].split('name ')[1].split(" ")[0],
        outArr
    )))

def fix_qt_paths(app):
    tested = []
    fixed = []


    def is_problematic(rpath):
        tested.append({})
        return "@rpath" in rpath and "@rpath/Qt" not in rpath


    def fix_qt_rpath(program, rpath):
        fixed_rpath = rpath.replace("@rpath", "@executable_path/../Frameworks")
        fixed.append(program)

        proc = Popen(["install_name_tool", "-change", rpath, fixed_rpath, program], stderr=PIPE, stdout=PIPE)
        result = proc.communicate()
        if result != (b"", b""):
            raise RuntimeError("install_name_tool error: " + result[1].decode("utf-8"))


    allfiles = [os.path.join(dp, f) for dp, dn, filenames in os.walk(app) for f in filenames]

    print("Testing {} files".format(len(allfiles)))

    for file in allfiles:
        if is_library(file):
            if "Frameworks" not in file:
                for rpath in list_rpaths(file):
                    if (is_problematic(rpath)):
                        print("{} is problematic: {}".format(file, rpath))
                    elif "@rpath" in rpath:
                        fix_qt_rpath(file, rpath)

    print("Tested {} rpaths".format(len(tested)))
    print("Fixed {} rpaths".format(len(fixed)))


if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python qt_fix_deps.py Example.app")
    else:
        fix_qt_paths(sys.argv[1])
