SRCREV = "0cb7d293a7f25394a06847a28d0f0ace9862936e"
BRANCH = "v2023.10"
SRCBRANCH ?= "${BRANCH}"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=f4d5df0f12dcea1b1a0124219c0dbab4"

include libmetal.inc
