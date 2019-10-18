SRCBRANCH ?= "rel-v2019.2"
SRCREV ?= "7fd76b240fb009d25b774c098e73908b2ddb7d92"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
