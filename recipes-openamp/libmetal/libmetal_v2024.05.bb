# SRVREV == v2024.05.0
SRCREV = "3aee6be866b190d2e2b4997fedbd976a0c37c0c6"
BRANCH = "v2024.05"
SRCBRANCH ?= "${BRANCH}"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=f4d5df0f12dcea1b1a0124219c0dbab4"

include libmetal.inc
