require ${LAYER_PATH_openamp-layer}/recipes-openamp/libmetal/libmetal.inc

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

SRCREV = "6244b2e0c4feb46b1d7f615eb51afa32058542c9"
BRANCH = "main"
SRCBRANCH = "${BRANCH}"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=f4d5df0f12dcea1b1a0124219c0dbab4"
OECMAKE_SOURCEPATH = "${S}/"
PROVIDES:armv7r:xilinx-standalone = "libmetal "
DEPENDS:append:armv7r:xilinx-standalone = " libxil scugic doxygen-native xilstandalone nativesdk-xilinx-lops "
DEPENDS:remove = "sysfsutils eudev"
inherit cmake

FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"
SRC_URI += " file://0001-lib-atomic-add-defines-for-build.patch "

TUNE_CCARGS:append =  "EXTRA-mfloat-abi=soft -mcpu=cortex-r5 -Wall -Werror -Wextra -flto -Os"
COMPATIBLE_HOST = ".*-elf"
COMPATIBLE_HOST:arm = "[^-]*-[^-]*-eabi"

LIBMETAL_CMAKE_MACHINE:versal = "Versal"
LIBMETAL_CMAKE_MACHINE:zynqmp = "Zynqmp"
LIBMETAL_CROSS_PREFIX:armv7r:xilinx-standalone = "arm-xilinx-eabi-"
LIBMETAL_MACHINE = "zynqmp_r5"

EXTRA_OECMAKE:armv7r:xilinx-standalone = " -DWITH_DOCS=OFF "

FILES:${PN}:append = " ${libdir}/*.a "

cmake_do_generate_toolchain_file:armv7r:xilinx-standalone:append() {
    cat >> ${WORKDIR}/toolchain.cmake <<EOF
    set (CMAKE_SYSTEM_PROCESSOR "${TRANSLATED_TARGET_ARCH}" )
    set (MACHINE "${LIBMETAL_MACHINE}" )
    set (CMAKE_MACHINE "${LIBMETAL_CMAKE_MACHINE}" )
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
    set (CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    set (CMAKE_SYSTEM_NAME "Generic")
    set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING "")

    include (CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER("${OECMAKE_C_COMPILER}" GNU)

    set (CROSS_PREFIX           "${LIBMETAL_CROSS_PREFIX}" CACHE STRING "")
    set (CMAKE_LIBRARY_PATH "${PKG_CONFIG_SYSROOT_DIR}/usr/lib" CACHE STRING "")
    set (CMAKE_C_ARCHIVE_FINISH   true)
    set (CMAKE_INCLUDE_PATH "${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")

    include (cross-generic-gcc)
    set (CMAKE_C_FLAGS          "-mfloat-abi=soft -mcpu=cortex-r5 " CACHE STRING "")
    add_definitions( -DWITH_EXAMPLES=OFF -DXPAR_SCUGIC_0_DIST_BASEADDR=XPAR_SCUGIC_DIST_BASEADDR -DSOC_FAMILY=${SOC_FAMILY})

EOF
}
