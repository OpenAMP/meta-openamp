SRCBRANCH ?= "rel-v2020.2"
SRCREV ?= "29eb98c02326199ad4245a484611a3b199d55bab"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
