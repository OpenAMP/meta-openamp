SRCBRANCH ?= "master"
SRCREV ?= "a1fb0099e81f439734696c984690ef8d9812c6c9"
PV = "${SRCBRANCH}+git${SRCPV}"

include open-amp.inc
