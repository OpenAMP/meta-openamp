SRCBRANCH ?= "xlnx_rel_v2021.2"
SRCREV = "efe5547832934f99cf6b6004208e546e1cfb6ce0"
BRANCH = "xlnx_rel_v2021.2"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
