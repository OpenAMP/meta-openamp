SRCBRANCH ?= "main"
SRCREV = "${@oe.utils.conditional("PREFERRED_PROVIDER_open-amp", "open-amp-dev", "${AUTOREV}", "568d507be81a27230ba3f0260485f2ee699f5aa0", d)}"
BRANCH = "main"
PV = "${SRCBRANCH}+git${SRCPV}"
DEFAULT_PREFERENCE = "-1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=ab88daf995c0bd0071c2e1e55f3d3505"

include open-amp.inc
