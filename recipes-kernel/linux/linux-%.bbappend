FILESEXTRAPATHS_prepend := "${THISDIR}:"
SRC_URI_append = " file://openamp-kmeta;type=kmeta;name=openamp-kmeta;destsuffix=openamp-kmeta"

KERNEL_FEATURES_append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/openamp.scc', '', d)}"
KERNEL_FEATURES_append = "${@bb.utils.contains('DISTRO_FEATURES', \
	'openamp-experiment-virtio-rpmsg-char', \
	' cfg/openamp-experiment-virtio-rpmsg-char.scc', '', d)}"
