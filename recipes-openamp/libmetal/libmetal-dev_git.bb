SRCBRANCH ?= "main"
SRCREV = "${@oe.utils.conditional("PREFERRED_PROVIDER_libmetal", "libmetal-dev", "${AUTOREV}", "2a59968271231b747bdf8fde0201cefe44aacc8c", d)}"
BRANCH = "main"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=f4d5df0f12dcea1b1a0124219c0dbab4"
PV = "${SRCBRANCH}+git${SRCPV}"
DEFAULT_PREFERENCE = "-1"

include libmetal.inc
