SRCBRANCH ?= "2022"
SRCREV = "af204f40297c1c5beda2a4ac5070ebf6e76e30b1"
BRANCH = "2022"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
