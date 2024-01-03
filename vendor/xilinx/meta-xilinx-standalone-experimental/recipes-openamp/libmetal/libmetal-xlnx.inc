EXTRA_OECMAKE += "-DYOCTO=ON"

inherit cmake ccmake

S:xilinx-standalone = "${WORKDIR}/git"
B:xilinx-standalone = "${WORKDIR}/build"

OECMAKE_SOURCEPATH:xilinx-standalone = "${S}/"
PROVIDES:xilinx-standalone = "libmetal"

DEPENDS:xilinx-standalone:append = " libxil doxygen-native xilstandalone nativesdk-xilinx-lops "
DEPENDS:xilinx-standalone:arm:append = " scugic xiltimer "
DEPENDS:xilinx-standalone:append:xilinx-freertos = " freertos10-xilinx "
DEPENDS:xilinx-standalone:remove = " sysfsutils eudev "

COMPATIBLE_HOST:xilinx-standalone = ".*-elf"
COMPATIBLE_HOST:xilinx-standalone:arm = "[^-]*-[^-]*-eabi"

LIBMETAL_CROSS_PREFIX:xilinx-standalone = "${TARGET_PREFIX}"

LIBMETAL_MACHINE:armv7r = "zynqmp_r5"
LIBMETAL_MACHINE:xilinx-standalone:cortexa53 = "zynqmp_a53"
LIBMETAL_MACHINE:xilinx-standalone:cortexa72 = "zynqmp_a72"
LIBMETAL_MACHINE:xilinx-standalone:cortexa78 = "zynqmp_a78"
LIBMETAL_MACHINE:microblaze = "microblaze_generic"

LIBMETAL_GCC_INCLUDE:xilinx-standalone = "generic"
LIBMETAL_GCC_INCLUDE:xilinx-freertos = "freertos"

LIBMETAL_SYSTEM_NAME:xilinx-standalone = "Generic"
LIBMETAL_SYSTEM_NAME:xilinx-freertos = "FreeRTOS"

LIBMETAL_CMAKE_SYSTEM_PROCESSOR:xilinx-standalone = "${TRANSLATED_TARGET_ARCH}"
LIBMETAL_CMAKE_SYSTEM_PROCESSOR:xilinx-standalone:microblaze = "microblaze"

LIBMETAL_DEMO ?= "OFF"

EXTRA_OECMAKE:armv7r = "\
    -DWITH_EXAMPLES=${LIBMETAL_DEMO} \
    -DSOC_FAMILY=${SOC_FAMILY} \
    "
OECMAKE_C_LINK_FLAGS:append:xilinx-standalone = " -lxilstandalone -lscugic -lxiltimer -lgcc"

FILES:${PN}:xilinx-standalone:append = " ${libdir}/*.a ${bindir}/*elf "

DEMO_LINK_FLAGS:armv7r:append = " -lxilstandalone -lscugic -lxiltimer"

cmake_do_generate_toolchain_file:append:xilinx-standalone() {
    cat >> ${WORKDIR}/toolchain.cmake <<EOF
    set (CMAKE_SYSTEM_PROCESSOR "${LIBMETAL_CMAKE_SYSTEM_PROCESSOR}" )
    set (CROSS_PREFIX           "${LIBMETAL_CROSS_PREFIX}" CACHE STRING "")
    set (MACHINE "${LIBMETAL_MACHINE}" )
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
    set (CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    set (CMAKE_SYSTEM_NAME "${LIBMETAL_SYSTEM_NAME}")
    set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING "")
    set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING "")
    include (CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER("${OECMAKE_C_COMPILER}" GNU)
    set (CMAKE_LIBRARY_PATH "${PKG_CONFIG_SYSROOT_DIR}/usr/lib" CACHE STRING "")
    set (CMAKE_C_ARCHIVE_FINISH   true)
    set (CMAKE_INCLUDE_PATH "${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PKG_CONFIG_SYSROOT_DIR}/usr/include/   " CACHE STRING "")
    include (cross-${LIBMETAL_GCC_INCLUDE}-gcc)
    set (WITH_DOC OFF)
    set (DEMO_LINK_FLAGS "${DEMO_LINK_FLAGS}" CACHE STRING "")
    add_definitions(-DSDT -DXLNX_PLATFORM)
EOF
}
