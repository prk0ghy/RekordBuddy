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

# -- This script builds the current version of Rekord Buddy.

import os
import sys
import getopt
import subprocess
import atexit
import fcntl
import time
import shutil
import platform


class LockDirectory(object):
    """Singleton that cna be used to lock access to a directory."""
    def __init__(self, directory):
        assert os.path.exists(directory)
        self.directory = directory

    def __enter__(self):
        self.dir_fd = os.open(self.directory, os.O_RDONLY)
        try:
            fcntl.flock(self.dir_fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except IOError as ex:
            die(f'Somebody else is locking {self.directory} - quitting.')
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        os.close(self.dir_fd)


class Context:
    """Context in which the script is used."""
    project_directory = os.path.dirname(os.path.abspath(os.path.split(sys.argv[0])[0]))

    debug_build = False
    run_unit_tests = False
    run_in_dev_mode = False
    build_app = False
    package_build = False

    local_repo_is_dirty = False

    beta_build = False
    app_name_postfix = ''

    build_version_number = ''
    symbols_version_number = ''
    git_tag_for_build = ''

    cmake_build_arguments = ''

    bugsnag_path = ''
    bugsnag_app_id = ''
    taglib_path = ''
    qt_path = ''
    codesign_identity = ''
    provider_short_name = ''
    appleid_email = ''

    bundle_id = 'org.rekordbuddy.RekordBuddy'


def die(message):
    """Error message when someting went wrong."""
    print('   ⛔️  ' + message, file=sys.stderr)
    sys.exit(-1)


def ok(message):
    """User message to provide information."""
    print('   ✅  ' + message)


def warn(message):
    """Message to warn the user about something."""
    print('   ⚠️  ' + message, file=sys.stderr)


def run_shell(command, capture_output=True):
    """Run a shell command."""
    try:
        result = subprocess.run(command,
                                shell=True,
                                check=True,
                                capture_output=capture_output)

        if not capture_output:
            if result.returncode == 0:
                return True, ''
            else:
                return False, ''

        newline = '\n'

        if result.returncode == 0:
            string_result = result.stdout.decode('UTF-8')
            if string_result.endswith(newline):
                string_result = string_result[:-len(newline)]

            return True, string_result
        else:
            string_result = (result.stdout + result.stderr).decode('UTF-8')
            if string_result.endswith(newline):
                string_result = string_result[:-len(newline)]

            return False, string_result
    except subprocess.CalledProcessError as error:
        if capture_output:
            return False, (error.stdout + error.stderr).decode('UTF-8')
        else:
            return False, ''

    return False, ''


def run_shell_script(command, context, capture_output=True):
    """Run a shell script located in bin/macOS."""
    return run_shell(os.path.join(context.project_directory,
                                  'bin',
                                  'macOS',
                                  command), capture_output)

def require(command):
    """Make sure a required command line tool is available."""
    if not run_shell('which ' + command)[0]:
        die(f'Can\'t find ' + command + '.')


def require_shell_script(command, context):
    """Make sure a required shell script is available."""
    if not os.path.isfile(os.path.join(context.project_directory,
                                       'bin',
                                       'macOS',
                                       command)):
        die(f'Can\'t find shell script ' + command + '.')


def get_first_line_of_file(input_file_path):
    """Return the first line of a text file."""
    with open(input_file_path) as input_file:
        for line in input_file:
            return line.replace('\n', '')


def get_version_number_from_file_in_directory(version_name, directory):
    """Get a version number from a text file."""
    version_file = os.path.join(directory, version_name)
    if os.path.isfile(version_file):
        version_number = get_first_line_of_file(version_file)
        if len(version_number) != 0:
            return version_number

    die(f'Could not find a {version_name} version number for this build.')


def check_no_existing_git_tag(context):
    """Make sure we don't already have a build tagged like this."""
    result = run_shell(f'git show "{context.git_tag_for_build}" 2>&1 > /dev/null | grep "fatal: ambiguous argument"')
    if not result[0]:
        die(f'Can\'t check for existing git tag ({result[1]}).')

    if result[1] == '':
        die(f'Found another commit tagged \'{context.git_tag_for_build}\'')


def check_local_dir_is_git_repo():
    """Check that there is a git repo in the local folder."""
    require('git')

    result = run_shell('git rev-parse --git-dir')
    if not result[0] or result[1] != '.git':
        die('Working directory is not a git repo.')


def check_local_git_clean(context):
    """Check that there's no changes on local working tree or index."""
    require('git')

    result = run_shell('git diff')
    if not result[0]:
        die('Can\'t run git diff.')

    if result[1] != '':
        result = run_shell('git ls-files --modified')
        if not result[0]:
            die('Can\'t run git ls-files.')

        if not context.run_in_dev_mode:
            die(f'Some files are checked out.\n{result[1]}')
        else:
            return False

    result = run_shell('git ls-files --others --exclude-standard')
    if not result[0]:
        die('Can\'t run git ls-files.')

    if result[1] != '':
        if not context.run_in_dev_mode:
            die(f'Some files are untracked.\n{result[1]}')
        else:
            return False

    result = run_shell('git update-index -q --ignore-submodules --refresh > /dev/null')
    if not result[0]:
        die('Can\'t run git update-index.')

    if result[1] != '':
        if not context.run_in_dev_mode:
            die(f'Unstaged changes in working tree.\n{result[1]}')
        else:
            return False

    result = run_shell('git diff-files -q --ignore-submodules -- > /dev/null')
    if not result[0]:
        die('Can\'t run git diff-files.')

    if result[1] != '':
        if not context.run_in_dev_mode:
            die(f'Uncommitted changes in index.\n{result[1]}')
        else:
            return False

    result = run_shell('git diff-index --quiet HEAD -- > /dev/null')
    if not result[0]:
        die('Can\'t run git diff-index.')

    if result[1] != '':
        if not context.run_in_dev_mode:
            die('Uncommitted changes in index.')
        else:
            return False

    ok('Working tree is clean.')
    return True


def setup_context(context):
    """Setup the script context and check everything looks good."""
    require('git')

    check_local_dir_is_git_repo()
    if not check_local_git_clean(context):
        context.local_repo_is_dirty = True

    ok(f'Building project in {context.project_directory}')

    ok(f'Beta build is {context.beta_build}')
    ok(f'Debug build is {context.debug_build}')

    version_files_directory = os.path.join(context.project_directory, 'Versionning')

    major_version_number = get_version_number_from_file_in_directory('major', version_files_directory)
    ok(f'Major version number is {major_version_number}')

    minor_version_number = get_version_number_from_file_in_directory('minor', version_files_directory)
    ok(f'Minor version number is {minor_version_number}')

    revision_version_number = get_version_number_from_file_in_directory('revision', version_files_directory)
    ok(f'Revision version number is {revision_version_number}')

    result = run_shell('git log -1 --format=%h')
    if not result[0]:
        die('Can\'t get commit hash.')

    commit_hash = commit_hash_for_symbols = result[1][:8]

    if context.local_repo_is_dirty:
        commit_hash = commit_hash + '-dirty'

    ok(f'Commit hash is {commit_hash}')

    if context.beta_build:
        context.app_name_postfix = ' Beta'

    if context.debug_build:
        if context.package_build:
            die('Can only package release builds.')

        context.build_type = 'Debug'
    else:
        if context.run_unit_tests:
            die('Can only run unit tests in debug builds.')

        context.build_type = 'RelWithDebInfo'

    build_version_number = f'{major_version_number}.{minor_version_number}'
    if revision_version_number != '0':
        build_version_number = f'{build_version_number}.{revision_version_number}'

    build_number = get_version_number_from_file_in_directory('build', version_files_directory)
    symbols_version_number = f'{major_version_number}.{minor_version_number}.{revision_version_number}.{build_number}.{commit_hash_for_symbols}'
    build_version_number = f'{build_version_number}({build_number})'
    git_tag_for_build = f'v{build_version_number}'

    if context.codesign_identity != '':
        if context.provider_short_name == '' or context.appleid_email == '':
            die('Code signing requires also the ASC provider short name and an Apple ID username/email.')

    ok(f'Build version number is {build_version_number}')
    ok(f'Symbols version number is {symbols_version_number}')
    ok(f'Git tag for build is {git_tag_for_build}')

    context.build_version_number = build_version_number
    context.symbols_version_number = symbols_version_number
    context.git_tag_for_build = git_tag_for_build


def check_requirements(context):
    """Make sure we have everything we need."""
    require('grep')
    require('clang')
    require('clang++')

    # Check versions of important things like clang and cmake
    result = run_shell('clang --version | grep "\(clang-12\)"')
    if not result[0] or result[1] == '':
        die('XCode 12 required.')
    else:
        ok(f'XCode 12 installed ({result[1]}).')

    cmake_version_is_correct = False

    require('cmake')

    result = run_shell('cmake --version | grep "cmake version [3456789]."')
    if result[0] and not result[1] == '':
        version_numbers = result[1][14:].split('.')
        number_of_version_numbers = len(version_numbers)
        if number_of_version_numbers > 0:
            major_cmake_version = int(version_numbers[0])
            if major_cmake_version > 3:
                cmake_version_is_correct = True
            elif (major_cmake_version == 3) and \
                 (number_of_version_numbers > 1) and \
                 (int(version_numbers[1]) >= 16):
                cmake_version_is_correct = True

    if cmake_version_is_correct:
        ok(f'CMake 3+ installed ({result[1]}).')
    else:
        die('CMake 3.16 or greater required required.')

    if context.package_build:
        require('codesign')
        require('strip')
        require('ditto')
        require('dsymutil')
        require('dwarfdump')
        require('dmgcanvas')
        require('spctl')

        require_shell_script('codesign-deep.sh', context)
        require_shell_script('notarize-app.sh', context)
        require_shell_script('bump.sh', context)
        require_shell_script('upload-bugsnag-symbols.sh', context)

        check_no_existing_git_tag(context)

        ok(f'Packaging build {context.git_tag_for_build}')

        context.cmake_build_arguments += ' -DNXA_CMAKE_THIS_IS_A_PRODUCTION_BUILD=1'


def get_build_path(context):
    """Get the path used to build the app given the context."""
    if context.debug_build:
        build_type = 'debug'
    else:
        build_type = 'relwithdebinfo'

    if context.beta_build:
        beta_build_string = '-beta'
    else:
        beta_build_string = ''

    context.build_path = os.path.join(context.project_directory, 'builds', 'cmake-buildscript-' + build_type + beta_build_string)
    ok(f'Build path {context.build_path}')


def get_cmake_build_arguments(context):
    """Get the arguments passed to CMake given the context."""
    context.cmake_build_arguments = ''

    if context.package_build:
        context.cmake_build_arguments += ' -DNXA_CMAKE_THIS_IS_A_PRODUCTION_BUILD=1'

    if context.beta_build:
        context.cmake_build_arguments += ' -DNXA_CMAKE_BETA_BUILD=1'
    else:
        context.cmake_build_arguments += ' -DNXA_CMAKE_BETA_BUILD=0'

    if context.run_unit_tests:
        context.cmake_build_arguments += ' -DNXA_CMAKE_BUILD_FOR_TESTING=1'
    else:
        context.cmake_build_arguments += ' -DNXA_CMAKE_BUILD_FOR_TESTING=0'

    if context.debug_build:
        context.cmake_build_arguments += ' -DNXA_CMAKE_ENABLE_EXTENDED_ERROR_DESCRIPTIONS=1'
        context.cmake_build_arguments += ' -DNXA_CMAKE_DISABLE_LINK_TIME_OPTIMIZATIONS=1'
        context.cmake_build_arguments += ' -DNXA_CMAKE_ENABLE_ADDRESS_SANITIZATION=1'
    else:
        context.cmake_build_arguments += ' -DNXA_CMAKE_THIS_IS_A_PRODUCTION_BUILD=1'
        context.cmake_build_arguments += ' -DNXA_CMAKE_ENABLE_EXTENDED_ERROR_DESCRIPTIONS=0'
        context.cmake_build_arguments += ' -DNXA_CMAKE_DISABLE_LINK_TIME_OPTIMIZATIONS=0'
        context.cmake_build_arguments += ' -DNXA_CMAKE_ENABLE_ADDRESS_SANITIZATION=0'

    if context.bugsnag_app_id != '':
        if context.bugsnag_path == '':
            die('You need to also provide a the path to the framework if you want to use the bugsnag framework.')

        context.cmake_build_arguments += ' -DNXA_BUGSNAG_DIR="' + context.bugsnag_path + '"'
        context.cmake_build_arguments += ' -DNXA_BUGSNAG_APP_ID=' + context.bugsnag_app_id

    if context.qt_path != '':
        context.cmake_build_arguments += ' -DNXA_QT_DIR="' + context.qt_path + '"'
    else:
        die('You need provide a location for the qt SDK.')

    if context.taglib_path != '':
        context.cmake_build_arguments += ' -DNXA_TAGLIB_DIR="' + context.taglib_path + '"'
    else:
        die('You need provide a location for the taglib framework.')

    ok(f'CMake arguments:{context.cmake_build_arguments}')


def package_build(context):
    """Package the build."""
    ok(f'Packaging a {context.build_type} build for version {context.build_version_number} in {context.build_path}.')

    app_name = 'Rekord Buddy.app'
    source_app_path = os.path.join(context.build_path, 'UI', app_name)
    packaged_app_folder = os.path.join(context.build_path, 'Packaged')
    build_app_path = os.path.join(packaged_app_folder, app_name)
    build_binary_path = os.path.join(build_app_path, 'Contents', 'MacOS', 'Rekord Buddy')

    if not os.path.exists(source_app_path):
        die('Can\'t find built binary in \'' + source_app_path + '\'.')

    if os.path.exists(packaged_app_folder):
        shutil.rmtree(packaged_app_folder)

    os.mkdir(packaged_app_folder)
    shutil.copytree(source_app_path, build_app_path, symlinks=True)

    # -- Before code signing, get symbols for uploading; and remove debug symbols from the binary.
    build_dsym_path = os.path.join(packaged_app_folder, context.symbols_version_number + '.dSYM')
    build_zipped_dsym_path = os.path.join(packaged_app_folder, context.symbols_version_number + ' Symbols.zip')

    result = run_shell(f'dsymutil "{build_binary_path}" -o "{build_dsym_path}"')
    if not result[0]:
        die(f'Error exporting symbols to dSYM file.\n{result[1]}')

    result = run_shell(f'ditto -c -k --sequesterRsrc --keepParent "{build_dsym_path}" "{build_zipped_dsym_path}"')
    if not result[0]:
        die(f'Error zipping up dSYM file.\n{result[1]}')

    ok('Symbols exported to dSYM file.')

    uuid_text_file = os.path.join(packaged_app_folder, context.build_version_number + ' UUID.txt')
    result = run_shell(f'dwarfdump -u "{build_dsym_path}" > "{uuid_text_file}"')
    if not result[0]:
        die(f'Error retrieving build UUID.\n{result[1]}')

    shutil.rmtree(build_dsym_path)

    run_shell(f'strip -S -X -T -x "{build_binary_path}"')
    if not result[0]:
        die(f'Error stripping app of non-exported symbols.\n{result[1]}')

    ok('App stripped of non-exported symbols.')

    ok('After symbols stripped:')
    run_shell(f'dsymutil "{build_binary_path}" --no-output', False)
    if not result[0]:
        die(f'Error checking that the app now has no symbols.\n{result[1]}')

    # -- Codesign and notarize the .app
    if context.codesign_identity != '':
        result = run_shell_script(f'codesign-deep.sh "{build_app_path}" "{context.codesign_identity}" "{context.bundle_id}"',
                                  context)
        if not result[0]:
            die(f'Error code signing app.\n{result[1]}')

        ok('App codesigned.')

        # -- Prepare the zip package used to notarize the signed .app
        notarize_package_path = os.path.join(packaged_app_folder, app_name + context.app_name_postfix + '.zip')
        result = run_shell(f'ditto -c -k --sequesterRsrc --keepParent "{build_app_path}" "{notarize_package_path}"',
                           context)
        if not result[0]:
            die(f'Error zipping up package for notarization.\n{result[1]}')

        # -- Notarize the app with Apple (this needs an application specific password added to the keychain, see notarize-app.sh)
        result = run_shell_script(f'notarize-app.sh {context.provider_short_name} \'{context.appleid_email}\' \'@keychain:notarize-app\' \'{context.bundle_id}\' "{notarize_package_path}" "{build_app_path}"',
                                  context, False)
        if not result[0]:
            die(f'Error notarizing app.\n{result[1]}')

        ok('App notarized.')

        # -- Remove the zip package used to notarize the signed .app
        os.remove(notarize_package_path)

        result = run_shell(f'spctl -a -t exec -vv "{build_app_path}"', False)
        if not result[0]:
            die(f'Error verifying app by Gatekeeper.\n{result[1]}')

        ok('App verified by Gatekeeper.')

    # -- Prepare the Sparkle package using the signed .app
    result = run_shell(f'ditto -c -k --sequesterRsrc --keepParent "{build_app_path}" "{notarize_package_path}"')
    if not result[0]:
        die(f'Error zipping up package for updater.\n{result[1]}')

    sparkle_zip_size = os.path.getsize(notarize_package_path)

    ok('Sparkle package created.')

    # -- Create a DMG installer (this requires the 'dmgcanvas' command line tool to be installed by DMGCanvas)
    dmg_package_path = os.path.join(packaged_app_folder, app_name + context.app_name_postfix + '.dmg')
    result = run_shell(f'dmgcanvas "{context.project_directory}/Installer/macOS/Rekord Buddy.dmgCanvas" "{dmg_package_path}" -setFilePath "{app_name}" "{build_app_path}"',
                       False)
    if not result[0]:
        die('Error creating installer.')

    # -- Upload the symbols to bugsnag
    ok(f'upload-bugsnag-symbols.sh -v --api-key {context.bugsnag_app_id} "{build_zipped_dsym_path}"')

    if not context.run_in_dev_mode and context.bugsnag_app_id != '':
        result = run_shell_script(f'upload-bugsnag-symbols.sh -v --api-key {context.bugsnag_app_id} "{build_zipped_dsym_path}"', context)
        if not result[0]:
            die(f'Error uploading symboles to bugsnag.\n{result[1]}')

        ok("Symbols uploaded to Bugsnag.")

    final_output_folder = os.path.join(os.path.expanduser('~'), 'Desktop', context.build_version_number)
    if os.path.exists(final_output_folder):
        shutil.rmtree(final_output_folder)

    shutil.move(packaged_app_folder, final_output_folder)

    ok(f'SUCCESS: Succesfully packaged Rekord Buddy {context.build_version_number} (Sparkle Package Size: {sparkle_zip_size}).')


def lock_build():
    """Lock so only one instance of script can be in flight at once. why? NXA_VERSION_TO_BUILD could be incremented wrong otherwise."""
    return LockDirectory(os.getcwd())


def show_usage():
    """Show how the script can be used."""
    print('build.py[-h][-d][-a][-s][-t][-p][-dev]')
    print(' -h/--help Show this help message.')
    print(' -d/--debug Build a debug build.')
    print(' -b/--beta Build a beta build.')
    print(' -a/--app Build the app.')
    print(' -t/--tests Build and run the unit tests.')
    print(' -p/--package Package the app after building.')
    print(' --dev Run the script in development mode.')
    print(' --qtpath=<PATH> Path the the Qt libraries.')
    print(' --taglibpath=<PATH> Path to the taglib framework.')
    print(' --bugsnagpath=<PATH> Path to the bugsnag framework (optional).')
    print(' --bsappid=<ID> ID to use with the bugsnag API (optional but required to use bugsnag).')
    print(' --codesign=<ID> Identity used to codesign the packaged app (optional).')
    print(' --providershorname=<name> ASC shortname used to notatize the app (optional but required when codesigning).')
    print(' --appleidemail=<email> Apple ID username email used to notatize the app (optional but required when codesigning).')

def parse_arguments(argv, context):
    try:
        opts, args = getopt.getopt(argv, 'hdbtap', ['help', 'debug', 'beta', 'tests', 'app', 'package', 'dev',
                                         'bugsnagpath=', 'bsappid=', 'taglibpath=', 'qtpath=', 'codesign=',
                                         'providershortname=', 'appleidemail='])
    except getopt.GetoptError as e:
        die(f'Error parsing arguments: {e}.')

    for opt, arg in opts:
        if opt in ('-h', '--help'):
            show_usage()
            sys.exit()
        elif opt in ('-d', '--debug'):
            context.debug_build = True
        elif opt in ('-b', '--beta'):
            context.beta_build = True
        elif opt in ('-t', '--tests'):
            context.run_unit_tests = True
        elif opt in ('-a', '--app'):
            context.build_app = True
        elif opt in ('--dev'):
            context.run_in_dev_mode = True
        elif opt in ('-p', '--package'):
            context.package_build = True
        elif opt in ('--bugsnagpath'):
            context.bugsnag_path = arg
        elif opt in ('--bsappid'):
            context.bugsnag_app_id = arg
        elif opt in ('--taglibpath'):
            context.taglib_path = arg
        elif opt in ('--qtpath'):
            context.qt_path = arg
        elif opt in ('--codesign'):
            context.codesign_identity = arg
        elif opt in ('--providershortname'):
            context.provider_short_name = arg
        elif opt in ('--appleidemail'):
            context.appleid_email = arg

def main(argv):
    """Main function of the script."""
    if platform.system() != 'Darwin':
        die('This build script is only for macOS at the moment.')

    context = Context()

    parse_arguments(argv, context)
    setup_context(context)

    check_requirements(context)

    get_build_path(context)
    get_cmake_build_arguments(context)

    # -- Let's build.
    with lock_build() as lock:
        ok(f'Starting a {context.build_type} build for version {context.build_version_number} in {context.build_path}.')

        result = run_shell('mkdir -p ' + context.build_path)
        if not result[0]:
            die('Can\'t create build directory.')

        result = run_shell(f'cd {context.build_path} && cmake ../.. {context.cmake_build_arguments} -DCMAKE_BUILD_TYPE={context.build_type}', False)
        if not result[0]:
            die(f'Error configuring CMake.')

        if context.run_unit_tests:
            ok('Running unit tests.')
            result = run_shell(f'cd {context.build_path} && cmake --build . --target RekordBuddyBuildAllTestsTarget -- -j20', False)
            if not result[0]:
                die(f'Error building unit tests.')

            result = run_shell(f'cd {context.build_path} && ctest --verbose', False)
            if not result[0]:
                die(f'Error running unit tests.')

        if context.build_app:
            result = run_shell(f'cd {context.build_path} && cmake --build . --target RekordBuddy -- -j20', False)
            if not result[0]:
                die(f'Error compiling code.')

            ok('Build completed successfully.')

            if context.package_build:
                package_build(context)


if __name__ == '__main__':
    main(sys.argv[1:])
