SRCBRANCH ?= "master"
SRCREV ?= "18048c4144f276cda793c125399057a6b5773edb"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
