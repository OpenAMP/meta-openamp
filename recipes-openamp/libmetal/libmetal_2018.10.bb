FILESEXTRAPATHS_prepend := "${THISDIR}/2018.10:"
SRCBRANCH ?= "master"
SRCREV ?= "89276b531e0d982ede3e3c59a3832d0888b6d239"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"

SRC_URI_append = " \
  file://0001-linux-device-Allow-multi-driver-probe.patch \
  file://0002-linux-device-Initialize-the-irq-info.patch \
  "

include libmetal.inc

