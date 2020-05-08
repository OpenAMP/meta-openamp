SRCBRANCH ?= "rel-v2020.1"
SRCREV ?= "9ee43dbed82c088fdb91a1dbb8ba6ae4a2d18050"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
