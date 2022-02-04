DESCRIPTION = "A minimal OpenAMP image"

INITRD_IMAGE = "core-image-minimal-initramfs"
LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL += " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    libmetal \
    open-amp \
    "

# for the generic arm64 machine (right now qemuarm64)
# install all the modules we built
IMAGE_INSTALL:qemuarm64 += " \
    kernel-modules \
    "

# save for later
#    kernel-module-zynqmp-r5-remoteproc

# uio-pdrv-genirq is not available upstream
# for now assume MACHINE = something Xilinx means kernel is Xilinx kernel
IMAGE_INSTALL:zynqmp += " kernel-module-uio-pdrv-genirq"

# for these machines, these are builtin
#IMAGE_INSTALL:zynqmp += " kernel-module-zynqmp-r5-remoteproc"
#IMAGE_INSTALL:zynqmp += " kernel-module-virtio-rpmsg-bus"
#IMAGE_INSTALL:zynq   += " kernel-module-zynq-remoteproc"

IMAGE_LINGUAS=""
