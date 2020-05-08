SRCBRANCH ?= "rel-v2020.2"
SRCREV ?= "7014401c4a720dcdc1472ccd530cce1eb046454e"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
