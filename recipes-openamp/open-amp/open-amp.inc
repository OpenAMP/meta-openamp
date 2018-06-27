SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"

REPO ?= "git://github.com/OpenAMP/open-amp.git;protocol=https"
BRANCH ?= "master"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp"

inherit pkgconfig cmake

EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${SOC_FAMILY} \
	-DWITH_PROXY=off \
	-DWITH_PROXY_APPS=on \
	"

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE_append_zynqmp = "-DWITH_APPS=ON"

PACKAGES_append_zynqmp += "${PN}-demos"

FILES_${PN} = " \
    ${libdir}/*.so* \
"

FILES_${PN}-demos = " \
    ${bindir}/*-shared \
"
do_install_append_zynqmp () {
	# Only install echo test client, matrix multiplication client,
	# and proxy app server for ZynqMP
	rm -rf ${D}/${bindir}/*-static
}
