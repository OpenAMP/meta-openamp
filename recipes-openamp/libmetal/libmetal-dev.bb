SRCBRANCH ?= "master"

SRCREV_DEFAULT ?= "9c34a7484dd4bfa557c2b7273f3674b30b3b0cb2"
SRCREV ?= "${@oe.utils.conditional("PREFERRED_PROVIDER_virtual/libmetal", "libmetal-dev", "${AUTOREV}", "${SRCREV_DEFAULT}", d)}"

PV = "${SRCBRANCH}+git${SRCPV}"

include libmetal.inc
