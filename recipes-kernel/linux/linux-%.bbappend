FILESEXTRAPATHS:prepend := "${THISDIR}:"
SRC_URI:append = " file://openamp-kmeta;type=kmeta;name=openamp-kmeta;destsuffix=openamp-kmeta"

KERNEL_FEATURES:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/openamp.scc', '', d)}"

# genericarm64 viaqemuarm64 w/ openamp distro feature
KERNEL_FEATURES:qemuarm64:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-genericarm64.scc', '', d)}"
