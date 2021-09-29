FILESEXTRAPATHS:prepend := "${THISDIR}:"
SRC_URI:append = " file://openamp-kmeta;type=kmeta;name=openamp-kmeta;destsuffix=openamp-kmeta"

KERNEL_FEATURES:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/openamp.scc', '', d)}"
KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc.scc', '', d)}"

# ZynqMP and Versal require use of SPARSEVMEMMAP kernel config
KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
KERNEL_FEATURES:zynqmp:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
