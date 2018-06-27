SRCBRANCH ?= "master"
SRCREV ?= "d279a6a14941e150dad6746a5a5dd3fff102a5ae"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
