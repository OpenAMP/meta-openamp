SRCBRANCH ?= "main"
SRCREV = "1904dee18da85400e72b8f55c5c99e872a486573"
BRANCH = "v2023.10"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
