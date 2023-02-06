
HOMEPAGE = "https://github.com/OpenAMP/openamp-system-reference"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/LICENSE.md;md5=0b96a4c07d631aa5141bd3f058ba43b0"

REPO = "git://github.com/OpenAMP/openamp-system-reference;protocol=https"
BRANCH = "main"

# this sets the default SRCREV for all rpmsg examples
# you can set it in local.conf to "${AUTOREV}" to get the tip of main
# This hash is tip of main as of 2023/2/6
OPENAMP_SYS_REF_SRCREV ?= "7f1fb3b84edc1b3eab62a7ade7f88e99f7e78b93"

SRC_URI = "${REPO};branch=${BRANCH};"
SRCREV  = "${OPENAMP_SYS_REF_SRCREV}"
