SRCBRANCH ?= "master"
SRCREV ?= "7f2d8ca88d643a9cec993add93d1630b2c7bd41e"
LIC_FILES_CHKSUM ?= "file://LICENSE;md5=b30cbe0b980e98bfd9759b1e6ba3d107"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
