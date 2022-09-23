SUMMARY = "RPMsg examples: proxy file system demo"

include rpmsg-example.inc

S = "${WORKDIR}/git/examples/linux/rpmsg-proxy-app"

RRECOMMENDS:${PN} = "kernel-module-rpmsg-char"

FILES:${PN} = "\
	/usr/bin/proxy_app \
"

do_install () {
	install -d ${D}/usr/bin
	install -m 0755 proxy_app ${D}/usr/bin/proxy_app
}
