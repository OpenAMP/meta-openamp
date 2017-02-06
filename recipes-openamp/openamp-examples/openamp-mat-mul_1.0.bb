SUMMARY = "OpenAMP examples: matrix multiplication demo"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://git/LICENSE;md5=b30cbe0b980e98bfd9759b1e6ba3d107"

SRCREV ?= "ea35683b9550f000b702abad9be45b5357c744dd"
SRC_URI = " \
	git://github.com/OpenAMP/open-amp.git;protocol=https;branch=master \
	file://Makefile \
	"

DEPENDS = "open-amp libmetal"

S = "${WORKDIR}"

FILES_${PN} = "/usr/bin/mat_mul-openamp"

do_compile_prepend() {
	cp git/apps/matrix_multiply/matrix_multiply.c ./
	cp git/apps/machine/${SOC_FAMILY}/platform_info.c ./
	cp git/apps/machine/${SOC_FAMILY}/rsc_table.c ./
	cp git/apps/machine/${SOC_FAMILY}/rsc_table.h ./
	cp git/apps/system/linux/machine/${SOC_FAMILY}/helper.c ./
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 mat_mul-openamp ${D}/usr/bin/mat_mul-openamp
}
