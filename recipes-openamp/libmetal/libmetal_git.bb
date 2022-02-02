SRCBRANCH ?= "main"
SRCREV = "f252f0e007fbfb8b3a52b1d5901250ddac96baad"
BRANCH = "main"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
