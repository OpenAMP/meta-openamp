FILESEXTRAPATHS:prepend := "${THISDIR}:"
SRC_URI:append = " file://openamp-xilinx-kmeta;type=kmeta;name=openamp-xilinx-kmeta;destsuffix=openamp-xilinx-kmeta"

KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-zynqmp.scc', '', d)}"
KERNEL_FEATURES:versal-net:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-zynqmp.scc', '', d)}"

# ZynqMP and Versal require use of SPARSEVMEMMAP kernel config
KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
KERNEL_FEATURES:versal-net:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
KERNEL_FEATURES:zynqmp:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
