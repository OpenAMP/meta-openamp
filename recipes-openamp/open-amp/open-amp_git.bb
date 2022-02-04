SRCBRANCH ?= "2022.1"
SRCREV = "a6555a3d7b98741737c7d03b47567044cc5eb91e"
BRANCH = "xlnx_rel_v2022.1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
