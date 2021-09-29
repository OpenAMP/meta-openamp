SUMMARY = "RPMsg examples: echo test demo"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b30cbe0b980e98bfd9759b1e6ba3d107"

SRC_URI = "\
	file://LICENSE \
	file://Makefile \
	file://echo_test.c \
	"

S = "${WORKDIR}"

RRECOMMENDS_${PN} = "kernel-module-rpmsg-char"

FILES_${PN} = "\
	/usr/bin/echo_test\
"

do_install () {
	install -d ${D}/usr/bin
	install -m 0755 echo_test ${D}/usr/bin/echo_test
}
