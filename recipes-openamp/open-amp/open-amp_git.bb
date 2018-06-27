SRCBRANCH ?= "master"
SRCREV ?= "33a899f23a4f140e5a99c9f8742741504ca8c6ab"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
