SRCBRANCH ?= "main"
SRCREV = "b8cd61b5191a3d961d99d9114cb781b3b5b66d7c"
BRANCH = "main"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
