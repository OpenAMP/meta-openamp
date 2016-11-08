SRCBRANCH ?= "master"

SRCREV ?= "${AUTOREV}"

PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
