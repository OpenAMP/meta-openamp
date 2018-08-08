SRCBRANCH ?= "master"
SRCREV ?= "b4aaf58c860b14869590ddf2229872aa397c141a"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
