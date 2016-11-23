SRCBRANCH ?= "master"

SRCREV_DEFAULT ?= "9a117913e2950b7e37820ebc6178dc1452267f48"
SRCREV ?= "${@oe.utils.conditional("PREFERRED_PROVIDER_virtual/openamp", "open-amp-dev", "${AUTOREV}", "${SRCREV_DEFAULT}", d)}"

PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
