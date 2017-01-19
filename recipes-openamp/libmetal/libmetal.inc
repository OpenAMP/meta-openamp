SUMMARY = "Libmetal : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/libmetal/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=395307789d21fd8945fc1c933cad18b5"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
SRCBRANCH ?= "master"
SRCBRANCHARG = "${@['nobranch=1', 'branch=${SRCBRANCH}'][d.getVar('SRCBRANCH', True) != '']}"
SRC_URI = "git://github.com/OpenAMP/libmetal.git;protocol=https;${SRCBRANCHARG}"

S = "${WORKDIR}/git"

DEPENDS = "libhugetlbfs sysfsutils"

PROVIDES = "libmetal"

inherit pkgconfig cmake

EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${SOC_FAMILY} \
	"

# Only builds the library but not the demo apps.
#EXTRA_OECMAKE_append_zynqmp = "-DWITH_EXAMPLES=ON"
