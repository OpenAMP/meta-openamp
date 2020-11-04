SRCBRANCH ?= "rel-v2020.2"
SRCREV ?= "595d97e827c2cd3974e26a2be70e7fe48b7fe67e"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
