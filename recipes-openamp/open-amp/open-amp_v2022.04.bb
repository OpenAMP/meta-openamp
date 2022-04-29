SRCBRANCH ?= "main"
SRCREV = "10dcceca63e5c699312ebc44eac87b63aeb58bf7"
BRANCH = "v2022.04"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
