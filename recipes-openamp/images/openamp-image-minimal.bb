DESCRIPTION = "A minimal OpenAMP image"

INITRD_IMAGE = "core-image-minimal-initramfs"
LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL += " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    kernel-module-uio-pdrv-genirq \
    kernel-module-remoteproc \
    kernel-module-virtio \
    kernel-module-virtio-ring \
    kernel-module-virtio-rpmsg-bus \
    libmetal \
    open-amp \
    "

#IMAGE_INSTALL_append_zynqmp += " kernel-module-zynqmp-r5-remoteproc"
#IMAGE_INSTALL_append_zynq += " kernel-module-zynq-remoteproc"

IMAGE_LINGUAS=""
