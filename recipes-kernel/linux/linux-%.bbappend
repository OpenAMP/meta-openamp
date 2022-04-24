FILESEXTRAPATHS:prepend := "${THISDIR}:"
SRC_URI:append = " file://openamp-kmeta;type=kmeta;name=openamp-kmeta;destsuffix=openamp-kmeta"

KERNEL_FEATURES:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/openamp.scc', '', d)}"

# genericarm64 via qemuarm64 w/ openamp distro feature
KERNEL_FEATURES:qemuarm64:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-generic-arm64.scc', '', d)}"
KERNEL_FEATURES:generic-arm64:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-generic-arm64.scc', '', d)}"
KERNEL_FEATURES:qemuarm:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-generic-armv7a.scc', '', d)}"
KERNEL_FEATURES:generic-armv7a:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-generic-armv7a.scc', '', d)}"
