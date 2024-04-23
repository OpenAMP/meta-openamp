SRCBRANCH ?= "2023.1"
SRCREV = "be635252271de342014a146825870b64bd41d6eb"
BRANCH = "xlnx_rel_v2023.1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=f4d5df0f12dcea1b1a0124219c0dbab4"
PV = "${SRCBRANCH}+git${SRCPV}"

REPO = "git://github.com/Xilinx/libmetal.git;protocol=https"

include ${LAYER_PATH_openamp-layer}/recipes-openamp/libmetal/libmetal.inc

EXTRA_OECMAKE += " \
       -DSOC_FAMILY="${SOC_FAMILY}" \
"

RPROVIDES:${PN}-dbg += "libmetal-dbg"
RPROVIDES:${PN}-dev += "libmetal-dev"
RPROVIDES:${PN}-lic += "libmetal-lic"
RPROVIDES:${PN}-src += "libmetal-src"
RPROVIDES:${PN}-staticdev += "libmetal-staticdev"
