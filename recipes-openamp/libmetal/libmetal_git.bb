SRCBRANCH ?= "master"
SRCREV ?= "baaead3bb35921f9d5cc76c4cdbd619c2ef4152b"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
