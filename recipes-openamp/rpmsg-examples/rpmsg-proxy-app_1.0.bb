SUMMARY = "RPMsg examples: Matrix Multiplication demo"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b30cbe0b980e98bfd9759b1e6ba3d107"

SRC_URI = "\
	file://LICENSE \
	file://Makefile \
	file://proxy_app.c \
	file://proxy_app.h \
	"

S = "${WORKDIR}"

RRECOMMENDS_${PN} = "kernel-module-rpmsg-char"

FILES_${PN} = "\
	/usr/bin/proxy_app \
"

do_install () {
	install -d ${D}/usr/bin
	install -m 0755 proxy_app ${D}/usr/bin/proxy_app
}
