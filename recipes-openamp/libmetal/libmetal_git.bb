SRCBRANCH ?= "master"
SRCREV ?= "f961176677491e5a3f85343306d99d75174a0e8e"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
