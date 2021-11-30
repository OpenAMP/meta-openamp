SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"

REPO ?= "git://github.com/Xilinx/open-amp.git;protocol=https"
BRANCH ?= "rel-v2020.2"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp"

inherit pkgconfig cmake yocto-cmake-translation

OPENAMP_MACHINE:versal = "zynqmp"
OPENAMP_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"
EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${OPENAMP_MACHINE} \
	"

SOC_FAMILY_ARCH ??= "${TUNE_PKGARCH}"
PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

CFLAGS:versal += " -Dversal "


# by default enable proxy apps
OPENAMP_PROXY ?= "1"

# by default use IPI for polling
OPENAMP_SHMEM_POLL ?= "0"

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = "-DWITH_APPS=ON "
EXTRA_OECMAKE:append:versal = "-DWITH_APPS=ON "

EXTRA_OECMAKE:append = " ${@'-DWITH_PROXY=on -DWITH_PROXY_APPS=on' if d.getVar('OPENAMP_PROXY') == '1' else ''}"

#set rpmsg no ipi if applicable
python rpmsg_no_ipi_setup() {
    if d.getVar("OPENAMP_SHMEM_POLL") == "1":
        toolchain_file_path = d.getVar("WORKDIR") + "/toolchain.cmake"
        toolchain_file = open(toolchain_file_path, "a") # a for append
        toolchain_file.write(" add_definitions(\" -DRPMSG_NO_IPI=ON \") ")
}


# only add definition after toolchain file has been generated
do_generate_toolchain_file[postfuncs] += "rpmsg_no_ipi_setup"

ALLOW_EMPTY:${PN}-demos = "1"
PACKAGES:append += "${PN}-demos"

FILES:${PN} = " \
    ${libdir}/*.so* \
"

FILES:${PN}-demos = " \
    ${bindir}/*-shared \
"
do_install:append () {
	# Only install echo test client, matrix multiplication client,
	# and proxy app server for ZynqMP
	rm -rf ${D}/${bindir}/*-static
}
