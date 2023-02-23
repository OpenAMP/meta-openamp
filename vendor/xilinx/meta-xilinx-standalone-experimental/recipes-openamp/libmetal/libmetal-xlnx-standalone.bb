require ${LAYER_PATH_openamp-layer}/recipes-openamp/libmetal/libmetal.inc

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

SRCREV = "964149ba2153eb6004ecd4bb8af461fbb227a76d"
SRCBRANCH = "xlnx_decoupling"
BRANCH = "xlnx_decoupling"
REPO = "git://gitenterprise.xilinx.com/OpenAMP/libmetal.git;protocol=https"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"
OECMAKE_SOURCEPATH = "${S}/"
PROVIDES:armv7r:xilinx-standalone = "libmetal "
DEPENDS:append:armv7r:xilinx-standalone = " libxil scugic doxygen-native xilstandalone nativesdk-xilinx-lops "
DEPENDS:remove = "sysfsutils eudev"
inherit cmake
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"

EXTRA_OECMAKE:armv7r:xilinx-standalone = " \
	-DSOC_FAMILY="${SOC_FAMILY}" \
	-DWITH_EXAMPLES=OFF \
	-DWITH_DOCS=OFF \
"
TUNE_CCARGS:append =  "EXTRA-mfloat-abi=soft -mcpu=cortex-r5 -Wall -Werror -Wextra -flto -Os"

ALLOW_EMPTY:${PN}-demos = "1"

COMPATIBLE_HOST = ".*-elf"
COMPATIBLE_HOST:arm = "[^-]*-[^-]*-eabi"

LIBMETAL_CMAKE_MACHINE:versal = "Versal"
LIBMETAL_CMAKE_MACHINE:zynqmp = "Zynqmp"
LIBMETAL_CROSS_PREFIX:armv7r:xilinx-standalone = "arm-xilinx-eabi-"
LIBMETAL_MACHINE = "zynqmp_r5"

cmake_do_generate_toolchain_file:armv7r:xilinx-standalone:append() {
    cat >> ${WORKDIR}/toolchain.cmake <<EOF
    set( CMAKE_SYSTEM_PROCESSOR "${TRANSLATED_TARGET_ARCH}" )
    set( MACHINE "${LIBMETAL_MACHINE}" )
    set( CMAKE_MACHINE "${LIBMETAL_CMAKE_MACHINE}" )
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
    SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    set( CMAKE_SYSTEM_NAME "Generic")
    set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING "")

    include (CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER("${OECMAKE_C_COMPILER}" GNU)
    set (CROSS_PREFIX           "${LIBMETAL_CROSS_PREFIX}" CACHE STRING "")
    set (CMAKE_LIBRARY_PATH "${S}/../recipe-sysroot/usr/lib" CACHE STRING "")
    SET(CMAKE_C_ARCHIVE_FINISH   true)
    set (CMAKE_INCLUDE_PATH "${S}/../recipe-sysroot/usr/include/" CACHE STRING "")
    include (cross-generic-gcc)
    add_definitions(-DWITH_DOC=OFF -DWITH_EXAMPLES=OFF)
    set (CMAKE_C_FLAGS          "-mfloat-abi=soft -mcpu=cortex-r5 " CACHE STRING "")
EOF
}
