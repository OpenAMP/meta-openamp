SRCBRANCH ?= "master"
SRCREV ?= "93c396de7ddcf446a0d6b247004bba8a01a335ee"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
