
HOMEPAGE = "https://github.com/OpenAMP/openamp-system-reference"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/LICENSE.md;md5=0b96a4c07d631aa5141bd3f058ba43b0"

REPO = "git://github.com/OpenAMP/openamp-system-reference;protocol=https"
BRANCH = "main"

SRC_URI = "${REPO};branch=${BRANCH};"
SRCREV = "${AUTOREV}"
SRC_URI[sha256sum] = "96169ab95166de8b8a8c18e4084948af66b03c5e22305fa736cfbd2620b5fd7e"
