# save for later
#    kernel-module-zynqmp-r5-remoteproc

# uio-pdrv-genirq is not available upstream
# for now assume MACHINE = something Xilinx means kernel is Xilinx kernel
IMAGE_INSTALL:zynqmp:append = " kernel-module-uio-pdrv-genirq"

# for these machines, these are builtin
# zynqmp: kernel-module-zynqmp-r5-remoteproc kernel-module-virtio-rpmsg-bus
# zynq: kernel-module-zynq-remoteproc
