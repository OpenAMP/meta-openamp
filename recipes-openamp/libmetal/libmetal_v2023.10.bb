SRCBRANCH ?= "main"
SRCREV = "0cb7d293a7f25394a06847a28d0f0ace9862936e"
BRANCH = "v2023.10"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
