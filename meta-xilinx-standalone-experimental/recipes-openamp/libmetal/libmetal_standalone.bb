require ${LAYER_PATH_openamp-layer}/recipes-openamp/libmetal/libmetal.inc

SRCREV = "c014e1eacd0164a44336ff727fe2e91aa6c062b6"
S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

BRANCH_armrm_xilinx-standalone = "experimental_dt"

OECMAKE_SOURCEPATH = "${S}/"
PROVIDES_armrm_xilinx-standalone = "libmetal "
DEPENDS_armrm_xilinx-standalone += " libxil scugic doxygen-native xilstandalone"
RDEPENDS_${PN} = ""
inherit cmake
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=1ff609e96fc79b87da48a837cbe5db33"

do_configure_armrm_xilinx-standalone[depends] += "device-tree-lops:do_deploy"

EXTRA_OECMAKE_armrm_xilinx-standalone = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DSOC_FAMILY="${SOC_FAMILY}" \
	-DWITH_EXAMPLES=ON \
	-DWITH_DOCS=OFF \
"

ALLOW_EMPTY_${PN}-demos = "1"

FILES_${PN}-demos_armrm_xilinx-standalone = " \
    ${bindir}/libmetal_* \
    ${bindir}/*ocm_demo.elf \
"

COMPATIBLE_HOST = ".*-elf"
COMPATIBLE_HOST_arm = "[^-]*-[^-]*-eabi"

LIBMETAL_CMAKE_MACHINE_versal = "Versal"
LIBMETAL_CMAKE_MACHINE_zynqmp = "Zynqmp"

def get_cross_prefix(oe_cmake_c_compiler):
  if oe_cmake_c_compiler == 'arm-xilinx-eabi-gcc':
    return 'arm-xilinx-eabi-'

LIBMETAL_CROSS_PREFIX_armrm_xilinx-standalone = "${@get_cross_prefix(d.getVar('OECMAKE_C_COMPILER'))}"

def get_libmetal_machine(soc_family):
  if soc_family in ['versal', 'zynqmp']:
    return 'zynqmp_r5'
  return ''


LIBMETAL_MACHINE_armrm_xilinx-standalone = "${@get_libmetal_machine(d.getVar('SOC_FAMILY'))}"

cmake_do_generate_toolchain_file_armrm_xilinx-standalone_append() {
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
    add_definitions(-DWITH_DOC=OFF)
EOF
}

# deploy for other recipes
DEPLOY_MACHINE = "${@ d.getVar('MACHINE_ARCH').replace('_','-') }"
SHOULD_DEPLOY = "${@'true' if ( 'Standalone' in  d.getVar('DISTRO_NAME') ) else 'false'}"
do_deploy() {
    echo "get the following: ";
    if ${SHOULD_DEPLOY}; then
        install -Dm 0644 ${D}/usr/bin/*.elf ${DEPLOY_DIR}/images/${DEPLOY_MACHINE}/
    fi
}
addtask deploy before do_build after do_install
