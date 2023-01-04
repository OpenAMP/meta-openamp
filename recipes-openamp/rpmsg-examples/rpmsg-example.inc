
HOMEPAGE = "https://github.com/OpenAMP/openamp-system-reference"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/LICENSE.md;md5=0b96a4c07d631aa5141bd3f058ba43b0"

REPO = "git://github.com/OpenAMP/openamp-system-reference;protocol=https"
BRANCH = "main"

# this sets the default SRCREV for all rpmsg examples
# you can set it in local.conf to "${AUTOREV}" to get the tip of main
# This hash is tip of main as of 2022/11/16 (still tip on 11/20)
OPENAMP_SYS_REF_SRCREV ?= "15550e1f1e1437dde5f8c22e01b488de641ec19e"

SRC_URI = "${REPO};branch=${BRANCH};"
SRCREV  = "${OPENAMP_SYS_REF_SRCREV}"
