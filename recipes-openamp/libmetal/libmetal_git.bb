SRCBRANCH ?= "master-rel-2021.1"
SRCREV = "f190d588f5cf89ee04af7eeab30af512e3e69bfd"
BRANCH = "master-rel-2021.1"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
