#!/bin/sh
# updates the bundle version from GIT revision
GIT_REVISION=$(git rev-list --count 1af8c74f53110e349d8f0d19b14599281913f71f..)

echo "const char SVN_revision[]=\"$GIT_REVISION\";" > $PROJECT_DIR/../../src/version.c

#if [ "${CONFIGURATION}" = "Release" ]; then
#    /usr/libexec/PlistBuddy -c "Set :CFBundleVersion $GIT_REVISION" "${TARGET_BUILD_DIR}/${INFOPLIST_PATH}"
#fi
