SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

# As of 2022/04/27, the virtio layer is BSD 2 Clause and the rest BSD 3 Clause
LICENSE = "BSD-3-Clause & BSD-2-Clause"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"

REPO ?= "git://github.com/OpenAMP/open-amp.git;protocol=https"
BRANCH ?= "main"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

# Skip processing of this recipe if it is not explicitly specified as the
# PREFERRED_PROVIDER for open-amp. This avoids network access required by
# the use of AUTOREV SRCREVs, which may be the default for some recipes.
python () {
    if (not d.getVar("PREFERRED_PROVIDER_open-amp") and "open-amp" != d.getVar("PN")) or \
       (d.getVar("PREFERRED_PROVIDER_open-amp") and d.getVar("PREFERRED_PROVIDER_open-amp") != d.getVar("PN")):
        d.delVar("BB_DONT_CACHE")
        raise bb.parse.SkipRecipe("Set PREFERRED_PROVIDER_open-amp to %s to enable it" % (d.getVar("PN")))
}

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp open-amp"

inherit pkgconfig cmake yocto-cmake-translation

OPENAMP_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"
EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${OPENAMP_MACHINE} \
	"

PACKAGE_ARCH = "${MACHINE_ARCH}"

# by default enable proxy apps
OPENAMP_PROXY ?= "1"

# by default use IPI for polling
OPENAMP_SHMEM_POLL ?= "0"

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
PACKAGES:append = " ${PN}-demos"

RPROVIDES:${PN} += "open-amp"
FILES:${PN} = " \
    ${libdir}/*.so* \
"

RPROVIDES:${PN}-demos += "open-amp-demos"
FILES:${PN}-demos = " \
    ${bindir}/*-shared \
"
