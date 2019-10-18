SRCBRANCH ?= "rel-v2019.2"
SRCREV ?= "c7fe342fcabb638dc98f3356d7eea869f8363ec6"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
