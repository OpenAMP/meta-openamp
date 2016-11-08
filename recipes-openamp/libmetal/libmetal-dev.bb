SRCBRANCH ?= "master"

SRCREV ?= "${AUTOREV}"

PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
