FILESEXTRAPATHS_prepend := "${THISDIR}/config:"

SRC_URI += "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' file://openamp.scc', '', d)}"
SRC_URI += "${@bb.utils.contains('DISTRO_FEATURES', \
	'openamp-experiment-virtio-rpmsg-char', \
	' file://openamp-experiment-virtio-rpmsg-char.scc', '', d)}"
