SUMMARY = "RPMsg examples: echo test demo"

include rpmsg-example.inc

S = "${WORKDIR}/git/examples/linux/rpmsg-echo-test"

RRECOMMENDS:${PN} = "kernel-module-rpmsg-char"

FILES:${PN} = "\
	/usr/bin/echo_test \
"

do_install () {
	install -d ${D}/usr/bin
	install -m 0755 echo_test ${D}/usr/bin/echo_test
}
