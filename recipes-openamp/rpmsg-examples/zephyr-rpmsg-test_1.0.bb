SUMMARY = "Zephyr RPMsg OpenAMP Test Script"
DESCRIPTION = "Automated test script for Zephyr-based OpenAMP RPMsg TTY demo"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://zephyr_rpmsg_test"

S = "${WORKDIR}"

inherit allarch
RDEPENDS:${PN} = "bash"
RRECOMMENDS:${PN} = "kernel-module-rpmsg-tty kernel-module-virtio-rpmsg-bus"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/zephyr_rpmsg_test ${D}${bindir}/zephyr_rpmsg_test
}

FILES:${PN} = "${bindir}/zephyr_rpmsg_test"
