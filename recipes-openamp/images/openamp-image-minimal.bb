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

# for the generic arm64 machine (right now qemuarm64) everything is a module
# install all the rpoc/rpmg modules we built
IMAGE_INSTALL:qemuarm64 += " \
    kernel-module-imx-rproc \
    kernel-module-mtk-rpmsg \
    kernel-module-qcom-common \
    kernel-module-qcom-glink \
    kernel-module-qcom-glink-rpm \
    kernel-module-qcom-glink-smem \
    kernel-module-qcom-pil-info \
    kernel-module-qcom-q6v5 \
    kernel-module-qcom-q6v5-adsp \
    kernel-module-qcom-q6v5-mss \
    kernel-module-qcom-q6v5-pas \
    kernel-module-qcom-q6v5-wcss \
    kernel-module-qcom-scm \
    kernel-module-qcom-smd \
    kernel-module-qcom-sysmon \
    kernel-module-qcom-wcnss-pil \
    kernel-module-ti-k3-dsp-remoteproc \
    kernel-module-ti-k3-r5-remoteproc \
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
