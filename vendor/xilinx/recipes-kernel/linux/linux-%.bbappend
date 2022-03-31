FILESEXTRAPATHS:prepend := "${THISDIR}:"

KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/remoteproc-zynqmp.scc', '', d)}"

# ZynqMP and Versal require use of SPARSEVMEMMAP kernel config
KERNEL_FEATURES:versal:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
KERNEL_FEATURES:zynqmp:append = "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', ' cfg/sparsevmemmap.scc', '', d)}"
