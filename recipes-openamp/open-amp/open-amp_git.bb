SRCBRANCH ?= "master"
SRCREV ?= "7f2d8ca88d643a9cec993add93d1630b2c7bd41e"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
