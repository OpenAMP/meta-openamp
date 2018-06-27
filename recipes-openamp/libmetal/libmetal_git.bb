SRCBRANCH ?= "master"
SRCREV ?= "18048c4144f276cda793c125399057a6b5773edb"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
