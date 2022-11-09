SRCBRANCH ?= "main"
SRCREV = "${@oe.utils.conditional("PREFERRED_PROVIDER_libmetal", "libmeta-dev", "${AUTOREV}", "2a59968271231b747bdf8fde0201cefe44aacc8c", d)}"
BRANCH = "main"
PV = "${SRCBRANCH}+git${SRCPV}"
DEFAULT_PREFERENCE = "-1"

include libmetal.inc
