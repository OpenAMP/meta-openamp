FILESEXTRAPATHS_prepend := "${THISDIR}/config:"

SRC_URI += "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' file://openamp.scc', '', d)}"
