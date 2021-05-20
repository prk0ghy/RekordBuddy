#!/bin/sh -u
#
# Taken from https://github.com/rednoah/notarize-app
#
# Linear notarize and staple for build tools.
#
# Run altool --notarize-app
# Run altool --notarization-info periodically and sleep until notarization is complete
# Run stapler staple
#
# Usage
#   notarize-app <asc-provider-shortname> '<appleIDemail>' '@keychain:notarize-app' '<primary-bundle-id>' <zipfile> <appfile>
#
# List of provider shortnames can be obtained with `xcrun altool --list-providers -u "<appleIDemail>" -p "@keychain:notarize-app"`
#
# Needs an application specific password from https://appleid.apple.com/account/manage and then that needs to be added to
# the local keychain with: security add-generic-password -a "<your-apple-ID-email>" -w "<app-specific-password>" -s "notarize-app"
# (see https://stackoverflow.com/questions/56890749/macos-notarize-in-script).

ASC_PROVIDER="$1"
ASC_USERNAME="$2"
ASC_PASSWORD="$3"

BUNDLE_ID="$4"
BUNDLE_ZIP="$5"
BUNDLE_APP="$6"

# create temporary files
NOTARIZE_APP_LOG=$(mktemp -t notarize-app)
NOTARIZE_INFO_LOG=$(mktemp -t notarize-info)

# delete temporary files on exit
function finish {
	rm "$NOTARIZE_APP_LOG" "$NOTARIZE_INFO_LOG"
}

trap finish EXIT

# submit app for notarization
if xcrun altool --notarize-app --primary-bundle-id "$BUNDLE_ID" --asc-provider "$ASC_PROVIDER" --username "$ASC_USERNAME" --password "$ASC_PASSWORD" -f "$BUNDLE_ZIP" > "$NOTARIZE_APP_LOG" 2>&1; then
	cat "$NOTARIZE_APP_LOG"
	RequestUUID=$(awk -F ' = ' '/RequestUUID/ {print $2}' "$NOTARIZE_APP_LOG")

	# check status periodically
	while sleep 60 && date; do
		# check notarization status
		if xcrun altool --notarization-info "$RequestUUID" --asc-provider "$ASC_PROVIDER" --username "$ASC_USERNAME" --password "$ASC_PASSWORD" > "$NOTARIZE_INFO_LOG" 2>&1; then
			cat "$NOTARIZE_INFO_LOG"

			# once notarization is complete, run stapler and exit
			if ! grep -q "Status: in progress" "$NOTARIZE_INFO_LOG"; then
				xcrun stapler staple "$BUNDLE_APP"
				exit
			fi
		else
			cat "$NOTARIZE_INFO_LOG" 1>&2
			exit 1
		fi
	done
else
	cat "$NOTARIZE_APP_LOG" 1>&2
	exit 1
fi
