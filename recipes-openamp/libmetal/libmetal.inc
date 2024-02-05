SUMMARY = "Libmetal : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/libmetal/"

SECTION = "libs"

# NOTE: some scripts used in patch compliance are Apache-2.0 but these are not
# used on target nor in normal builds
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=fe0b8a4beea8f0813b606d15a3df3d3c"

REPO ?= "git://github.com/OpenAMP/libmetal.git;protocol=https"
BRANCH ?= "main"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

# Skip processing of this recipe if it is not explicitly specified as the
# PREFERRED_PROVIDER for libmetal. This avoids network access required by
# the use of AUTOREV SRCREVs, which may be the default for some recipes.
python () {
    if (not d.getVar("PREFERRED_PROVIDER_libmetal") and "libmetal" != d.getVar("PN")) or \
       (d.getVar("PREFERRED_PROVIDER_libmetal") and d.getVar("PREFERRED_PROVIDER_libmetal") != d.getVar("PN")):
        d.delVar("BB_DONT_CACHE")
        raise bb.parse.SkipRecipe("Set PREFERRED_PROVIDER_libmetal to %s to enable it" % (d.getVar("PN")))
}

S = "${WORKDIR}/git"

# if user want to enable hugetlbfs support, please
# add "DEPENDS:append:pn-libmetal = " libhugetlbfs "" to your local.conf
DEPENDS = "sysfsutils ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'udev', 'eudev', d)}"
RDEPENDS:${PN}:append:aarch64:linux = " libudev"

PROVIDES = "libmetal"

inherit pkgconfig cmake yocto-cmake-translation

LIBMETAL_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"

PACKAGE_ARCH = "${MACHINE_ARCH}"

EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${LIBMETAL_MACHINE} \
	-DWITH_EXAMPLES=ON \
	"

ALLOW_EMPTY:${PN}-demos = "1"

PACKAGES:append = " ${PN}-demos"

RPROVIDES:${PN} += "libmetal"
FILES:${PN} = " \
    ${libdir}/*.so* \
    ${bindir}/test-* \
"

RPROVIDES:${PN}-demos += "libmetal-demos"
FILES:${PN}-demos = " \
    ${bindir}/libmetal_* \
    ${bindir}/*demo \
"

do_install:append () {
	# Only install the shared lib binaries
	rm -rf ${D}/${bindir}/*-static

	for f in `find ${D}/${bindir}/ -type f -regex ".*-shared*"`; do
		newf="`echo ${f} | sed 's/-shared*$//g'`"
		mv "${f}" "${newf}"
	done
}

