SUMMARY = "OpenAMP examples: proxy application demo"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://git/LICENSE;md5=b30cbe0b980e98bfd9759b1e6ba3d107"

SRCREV ?= "9a117913e2950b7e37820ebc6178dc1452267f48"
SRC_URI = " \
	git://github.com/OpenAMP/open-amp.git;protocol=https;branch=master \
	file://Makefile \
	"

DEPENDS = "open-amp libmetal"

S = "${WORKDIR}"

FILES_${PN} = "/usr/bin/proxy_app-openamp"

do_compile_prepend() {
	cp git/apps/rpc_demo/rpc_demod.c ./
	cp git/apps/machine/${SOC_FAMILY}/platform_info.c ./
	cp git/apps/machine/${SOC_FAMILY}/rsc_table.c ./
	cp git/apps/machine/${SOC_FAMILY}/rsc_table.h ./
	cp git/apps/system/linux/machine/${SOC_FAMILY}/helper.c ./
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 proxy_app-openamp ${D}/usr/bin/proxy_app-openamp
}
