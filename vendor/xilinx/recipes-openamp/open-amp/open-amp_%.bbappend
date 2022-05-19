OPENAMP_MACHINE:versal = "zynqmp"

SOC_FAMILY_ARCH ??= "${TUNE_PKGARCH}"
PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

CFLAGS:append:versal = " -Dversal "

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = " -DWITH_APPS=ON"
EXTRA_OECMAKE:append:versal = " -DWITH_APPS=ON"

do_install:append () {
        # Only install echo test client, matrix multiplication client,
        # and proxy app server for ZynqMP
        rm -rf ${D}/${bindir}/*-static
}

