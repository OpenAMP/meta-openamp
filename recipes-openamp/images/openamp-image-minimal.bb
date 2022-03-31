DESCRIPTION = "A minimal OpenAMP image"

INITRD_IMAGE = "core-image-minimal-initramfs"
LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL:append = " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    libmetal \
    open-amp \
    "

# for the generic arm64 machine (right now qemuarm64)
# install all the modules we built
IMAGE_INSTALL:qemuarm64:append = " kernel-modules"

IMAGE_LINGUAS=""
