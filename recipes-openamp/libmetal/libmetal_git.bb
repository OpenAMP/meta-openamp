SRCBRANCH ?= "2022"
SRCREV = "bee059dfedcfd98d1b113d8d6cce1c8aa916ff54"
BRANCH = "2022"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
