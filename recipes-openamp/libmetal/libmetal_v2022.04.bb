SRCBRANCH ?= "main"
SRCREV = "2371f615bb0f7968e7bd2483301a9ab9d957bec9"
BRANCH = "v2022.04"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
