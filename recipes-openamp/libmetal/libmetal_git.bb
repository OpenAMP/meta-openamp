SRCBRANCH ?= "master"
SRCREV ?= "51e08c67556474762257a825fc92cf8dbd302388"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
