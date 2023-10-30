SUMMARY = "RPMsg utilities: utilities for /dev/rpmsg*"

include rpmsg-example.inc

S = "${WORKDIR}/git/examples/linux/rpmsg-utils"

RRECOMMENDS:${PN} = "kernel-module-rpmsg-ctrl kernel-module-rpmsg-char"

FILES:${PN} = "\
	/usr/bin/rpmsg_destroy_ept \
	/usr/bin/rpmsg_export_dev \
	/usr/bin/rpmsg_export_ept \
	/usr/bin/rpmsg_ping \
"

EXTRA_OEMAKE = "DESTDIR=${D} prefix=/usr CC='${CC}' CFLAGS='${CFLAGS}' LDFLAGS='${LDFLAGS}'"

do_install() {
	install -d ${D}/usr/bin
	oe_runmake install
}
