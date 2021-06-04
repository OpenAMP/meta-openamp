SRCBRANCH ?= "xlnx_rel_v2021.1"
SRCREV = "3c848513f2dd1227fb54010a3f989ddc3c3dbea2"
BRANCH = "xlnx_rel_v2021.1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
