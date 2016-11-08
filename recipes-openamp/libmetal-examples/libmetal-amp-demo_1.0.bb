SUMMARY = "Libmetal examples: Applications to demonstrate how to use libmetal library"

HOMEPAGE = "https://github.com/OpenAMP/libmetal/"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://git/LICENSE.md;md5=395307789d21fd8945fc1c933cad18b5"

SRCREV ?= "9c34a7484dd4bfa557c2b7273f3674b30b3b0cb2"
SRC_URI = " \
	git://github.com/OpenAMP/libmetal.git;protocol=https;branch=master \
	file://Makefile \
	"

DEPENDS = "libmetal"

S = "${WORKDIR}"

FILES_${PN} = "/usr/bin/libmetal-amp-demo"

do_compile_prepend() {
	src="git/examples/system/linux/${SOC_FAMILY}/${SOC_FAMILY}_amp_demo"
	cp "${src}/init_linux.c" ./
	cp "${src}/libmetal_amp_demo.c" ./
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 libmetal-amp-demo ${D}/usr/bin/libmetal-amp-demo
}
