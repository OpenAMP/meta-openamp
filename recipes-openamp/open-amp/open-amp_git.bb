SRCBRANCH ?= "xlnx_rel_v2021.1"
SRCREV = "84041fa84d9bc524357b030ebe9a5174b01377bd"
BRANCH = "xlnx_rel_v2021.1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
