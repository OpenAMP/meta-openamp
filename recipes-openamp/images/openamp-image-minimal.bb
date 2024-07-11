DESCRIPTION = "A minimal OpenAMP image"

INITRD_IMAGE = "core-image-minimal-initramfs"
LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL:append = " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    libmetal \
    open-amp \
    rpmsg-echo-test rpmsg-mat-mul rpmsg-proxy-app rpmsg-utils \
    "

IMAGE_LINGUAS=""
