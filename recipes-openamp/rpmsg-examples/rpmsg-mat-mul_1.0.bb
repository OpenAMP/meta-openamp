SUMMARY = "RPMsg examples: Matrix Multiplication demo"

include rpmsg-example.inc

S = "${WORKDIR}/git/examples/linux/rpmsg-mat-mul"

RRECOMMENDS:${PN} = "kernel-module-rpmsg-char"

FILES:${PN} = "\
	/usr/bin/mat_mul_demo\
"

do_install () {
	install -d ${D}/usr/bin
	install -m 0755 mat_mul_demo ${D}/usr/bin/mat_mul_demo
}
