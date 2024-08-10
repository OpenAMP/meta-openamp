SRCBRANCH ?= "main"
SRCREV = "${@oe.utils.conditional("PREFERRED_PROVIDER_open-amp", "open-amp-dev", "${AUTOREV}", "568d507be81a27230ba3f0260485f2ee699f5aa0", d)}"
BRANCH = "main"
PV = "${SRCBRANCH}+git${SRCPV}"
DEFAULT_PREFERENCE = "-1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=dfc0adf4d04cc738ba65b7d3f587dca5"

include open-amp.inc
