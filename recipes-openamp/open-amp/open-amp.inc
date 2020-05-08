SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"

REPO ?= "git://github.com/Xilinx/open-amp.git;protocol=https"
BRANCH ?= "rel-v2020.1"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp"

inherit pkgconfig cmake yocto-cmake-translation

OPENAMP_MACHINE_versal = "zynqmp"
OPENAMP_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"
EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${OPENAMP_MACHINE} \
	"

PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

CFLAGS_versal += " -Dversal "
# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE_append_zynqmp = "-DWITH_APPS=ON -DWITH_PROXY=on -DWITH_PROXY_APPS=on "
EXTRA_OECMAKE_append_versal = "-DWITH_APPS=ON -DWITH_PROXY=on -DWITH_PROXY_APPS=on "

ALLOW_EMPTY_${PN}-demos = "1"
PACKAGES_append += "${PN}-demos"

FILES_${PN} = " \
    ${libdir}/*.so* \
"

FILES_${PN}-demos = " \
    ${bindir}/*-shared \
"
do_install_append () {
	# Only install echo test client, matrix multiplication client,
	# and proxy app server for ZynqMP
	rm -rf ${D}/${bindir}/*-static
}
