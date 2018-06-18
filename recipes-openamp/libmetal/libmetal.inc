SUMMARY = "Libmetal : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/libmetal/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"

REPO ?= "git://github.com/OpenAMP/libmetal.git;protocol=https"
BRANCH ?= "master"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

# if user want to enable hugetlbfs support, please
# add "DEPENDS_append_pn-libmetal = " libhugetlbfs "" to your local.conf
DEPENDS = "sysfsutils"

PROVIDES = "libmetal"

inherit pkgconfig cmake yocto-cmake-translation

EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE="${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}" \
	"

EXTRA_OECMAKE_append_zynqmp = "-DWITH_EXAMPLES=ON"

PACKAGES_append_zynqmp += "${PN}-demos"

FILES_${PN} = " \
    ${libdir}/*.so* \
    ${bindir}/test-* \
"
FILES_${PN}-demos = " \
    ${bindir}/libmetal_* \
"

do_install_append_zynqmp () {
	# Only install the shared lib binaries
	rm -rf ${D}/${bindir}/*-static

	for f in `find ${D}/${bindir}/ -type f -regex ".*-shared*"`; do
		newf="`echo ${f} | sed 's/-shared*$//g'`"
		mv "${f}" "${newf}"
	done
}
