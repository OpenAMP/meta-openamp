SRCBRANCH ?= "master"
SRCREV ?= "a4f763094cb26cd8f7abdff251f57a6a802c039d"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
