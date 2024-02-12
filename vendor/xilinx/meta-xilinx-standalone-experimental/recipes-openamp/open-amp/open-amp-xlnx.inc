PROVIDES:armv7r:xilinx-standalone = " open-amp open-amp-xlnx "
RPROVIDES:${PN} += "open-amp"
RPROVIDES:${PN}-demos += "open-amp-demos"

inherit ccmake cmake python3-dir deploy plnx-deploy

LOPS_DIR="${RECIPE_SYSROOT_NATIVE}/${PYTHON_SITEPACKAGES_DIR}/lopper/lops/"
OPENAMP_DTFILE ?= "${SYSTEM_DTFILE}"

EXTRA_OECMAKE:append = " -DYOCTO=ON "
S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

OECMAKE_SOURCEPATH = "${S}/"

DEPENDS:append = " libmetal python3-pyyaml-native lopper-native python3-dtc-native "
DEPENDS:xilinx-standalone:append = " xiltimer libxil scugic xilstandalone xiltimer  nativesdk-xilinx-lops "

OPENAMP_WITH_PROXY ?= "OFF"

FILESEXTRAPATHS:prepend := "${THISDIR}/overlays:"
OPENAMP_OVERLAY_DEFAULT = "unknown"
OPENAMP_OVERLAY_DEFAULT:zynqmp ??= "openamp-overlay-zynqmp.yaml"
OPENAMP_OVERLAY_DEFAULT:versal ??= "openamp-overlay-versal.yaml"
OPENAMP_OVERLAY_DEFAULT:versal-net ??= "openamp-overlay-versal-net.yaml"
OPENAMP_OVERLAY ?= "${OPENAMP_OVERLAY_DEFAULT}"
SRC_URI:append = "  file://${OPENAMP_OVERLAY} "

OECMAKE_C_LINK_FLAGS:xilinx-standalone:append = " --sysroot=${STAGING_DIR_HOST} "
CFLAGS:append = " -DSDT -D_AMD_GENERATED_ "
CFLAGS:xilinx-standalone:append = " -O3  -DXLNX_PLATFORM \
 -specs=${PKG_CONFIG_SYSROOT_DIR}/usr/include/Xilinx.spec \
 "

DEMO_LINK_FLAGS ?= ""
DEMO_LINK_FLAGS:armv7r = " -lxilstandalone -lscugic -lxiltimer"

cmake_do_generate_toolchain_file:append:armv7r() {
    cat >> ${WORKDIR}/toolchain.cmake <<EOF
	include (CMakeForceCompiler)
	CMAKE_FORCE_C_COMPILER("${OECMAKE_C_COMPILER}" GNU)
	set (CMAKE_SYSTEM_PROCESSOR "${TRANSLATED_TARGET_ARCH}" )
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
	set (CMAKE_SYSTEM_NAME "Generic")
	set (MACHINE               "zynqmp_r5" )
	set (CMAKE_MACHINE         "${OPENAMP_CMAKE_MACHINE}" )
	set (CMAKE_LIBRARY_PATH    "${PKG_CONFIG_SYSROOT_DIR}/usr/lib" CACHE STRING "")
	set (CMAKE_INCLUDE_PATH    "${CMAKE_INCLUDE_PATH} ${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")
	set (CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH} ${STAGING_LIBDIR} " CACHE STRING "")
	set (LIBMETAL_INCLUDE_DIR  " ${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")
	set (LIBMETAL_LIB_DIR      " ${PKG_CONFIG_SYSROOT_DIR}/usr/lib" CACHE STRING "")
	set (XIL_INCLUDE_DIR       " ${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")
	set (CMAKE_C_FLAGS         " ${CMAKE_C_FLAGS}  ${PKG_CONFIG_SYSROOT_DIR}/usr/include/" CACHE STRING "")
	set (DEMO_LINK_FLAGS       "${DEMO_LINK_FLAGS}" CACHE STRING "")
	set (WITH_PROXY            "${OPENAMP_WITH_PROXY}" CACHE STRING "")

EOF
}

do_generate_toolchain_file[postfuncs] += "openamp_toolchain_file_setup"
LOPPER_GEN_FILE_OUTPUT:linux = "zynqmp"
LOPPER_GEN_FILE_OUTPUT:xilinx-standalone = "zynqmp_r5"
openamp_toolchain_file_setup() {
  cp ${WORKDIR}/amd_platform_info.h ${S}/apps/machine/${LOPPER_GEN_FILE_OUTPUT}
}

OPENAMP_LOPPER_INPUTS:xilinx-standalone = " -i ${LOPS_DIR}/lop-r5-imux.dts "
OPENAMP_LOPPER_INPUTS:linux:zynqmp = "            \
    -i ${LOPS_DIR}/lop-gen_domain_dts-invoke.dts  \
    -i ${LOPS_DIR}/lop-a53-imux.dts "

OPENAMP_LOPPER_INPUTS:linux:versal = "           \
    -i ${LOPS_DIR}/lop-gen_domain_dts-invoke.dts \
    -i ${LOPS_DIR}/lop-a72-imux.dts "

# FIXME TODO loper is missing a78 linux domain lop file
OPENAMP_LOPPER_INPUTS:linux:versal-net = "       \
    -i ${LOPS_DIR}/lop-gen_domain_dts-invoke.dts \
    -i ${LOPS_DIR}/lop-a78-imux.dts "

do_configure[depends] += " lopper-native:do_install"

# Here set Lopper targets as needed
addtask run_set_lopper_targets after do_unpack
addtask run_set_lopper_targets after do_prepare_recipe_sysroot

OPENAMP_LOPPER_HOST:zynqmp ?= "a53_0"
OPENAMP_LOPPER_HOST:versal ?= "a72_0"
OPENAMP_LOPPER_HOST:versal-net ?= "a78_0"
OPENAMP_LOPPER_HOST ?= ""

OPENAMP_LOPPER_REMOTE:zynqmp ?= "r5_0"
OPENAMP_LOPPER_REMOTE:versal ?= "r5_0"
OPENAMP_LOPPER_REMOTE:versal-net ?= "r52_0"

OPENAMP_LOPPER_OUTPUT[psu_cortexa53_0] = "a53_0"
OPENAMP_LOPPER_OUTPUT[psu_cortexa53_1] = "a53_1"
OPENAMP_LOPPER_OUTPUT[psu_cortexr5_0] = "r5_0"
OPENAMP_LOPPER_OUTPUT[psu_cortexr5_1] = "r5_1"
OPENAMP_LOPPER_OUTPUT[psv_cortexr5_0] = "r5_0"
OPENAMP_LOPPER_OUTPUT[psv_cortexr5_1] = "r5_1"
OPENAMP_LOPPER_OUTPUT[psv_cortexa72_0] = "a72_0"
OPENAMP_LOPPER_OUTPUT[psv_cortexa72_1] = "a72_1"
OPENAMP_LOPPER_OUTPUT[psx_cortexa78_0] = "a78_0"
OPENAMP_LOPPER_OUTPUT[psx_cortexa78_1] = "a78_1"
OPENAMP_LOPPER_OUTPUT[psx_cortexr52_0] = "r52_0"
OPENAMP_LOPPER_OUTPUT[psx_cortexr52_1] = "r52_1"
OPENAMP_LOPPER_OUTPUT[psx_cortexr52_2] = "r52_2"
OPENAMP_LOPPER_OUTPUT[psx_cortexr52_3] = "r52_3"

OPENAMP_LOPPER_TARGET = "${@d.getVarFlag('OPENAMP_LOPPER_OUTPUT', d.getVar('ESW_MACHINE') or '') or ''}"
OPENAMP_LOPPER_REMOTE:xilinx-standalone ?= "${OPENAMP_LOPPER_TARGET}"

OPENAMP_ROLE:xilinx-standalone = "remote"
OPENAMP_ROLE:linux = "host"

LOP_LINUX_HOST:zynqmp = "psu_cortexa53_0"
LOP_LINUX_HOST:versal = "psv_cortexa72_0"
LOP_LINUX_HOST:versal-net = "psx_cortexa78_0"

addtask run_set_lopper_targets before do_run_lopper
do_run_set_lopper_targets() {
	# Set OpenAMP Role, Host and Target in lop file
	sed -i -e 's/openamp_role=host/openamp_role=${OPENAMP_ROLE}/g' ${LOPS_DIR}/lop-openamp-invoke.dts
	sed -i -e 's/openamp_host=a53_0/openamp_host=${OPENAMP_LOPPER_HOST}/g' ${LOPS_DIR}/lop-openamp-invoke.dts
	sed -i -e 's/openamp_remote=r5_0/openamp_remote=${OPENAMP_LOPPER_REMOTE}/g' ${LOPS_DIR}/lop-openamp-invoke.dts

	# Set Linux Host
	sed -i -e 's/psv_cortexa72_0/${LOP_LINUX_HOST}/g' ${LOPS_DIR}/lop-gen_domain_dts-invoke.dts
}

LOPPER_OPENAMP_OUT_DTB ?= "${WORKDIR}/openamp-lopper-output.dtb"
addtask run_lopper after run_set_lopper_targets
addtask run_lopper after do_fetch
addtask run_lopper before do_generate_toolchain_file
do_run_lopper() {
    export LOPPER_DTC_FLAGS="-b 0 -@"
    cd ${WORKDIR}

    lopper -f -v --enhanced  --permissive \
    -i ${OPENAMP_OVERLAY}		  \
    -i ${LOPS_DIR}/lop-load.dts           \
    -i ${LOPS_DIR}/lop-xlate-yaml.dts     \
    -i ${LOPS_DIR}/lop-openamp-invoke.dts \
    ${OPENAMP_LOPPER_INPUTS} ${OPENAMP_DTFILE} \
    ${LOPPER_OPENAMP_OUT_DTB}

    cd -

    if [ ! -f ${WORKDIR}/amd_platform_info.h ]; then
        bberror "Unable to generate file amd_platform_info.h from OpenAMP Lopper run."
        exit 1
    fi
}

SOC_FAMILY_ARCH:linux ??= "${TUNE_PKGARCH}"
PACKAGE_ARCH:linux = "${SOC_FAMILY_ARCH}"

COMPATIBLE_HOST:linux = ".*"
COMPATIBLE_HOST:xilinx-standalone = ".*-elf"
COMPATIBLE_HOST:xilinx-standalone:arm = "[^-]*-[^-]*-eabi"

OPENAMP_MACHINE:xilinx-standalone = "zynqmp_r5"

DEPLOY_MACHINE = "${@ d.getVar('MACHINE_ARCH').replace('_','-') }"
STANDALONE_SHOULD_DEPLOY = "${@'true' if ( 'Standalone' in  d.getVar('DISTRO_NAME') ) else 'false'}" 
do_deploy() {
	# remove existing DTB if applicable
	if test -f ${DEPLOYDIR}${LOPPER_OPENAMP_OUT_DTB}; then
		rm ${DEPLOYDIR}${LOPPER_OPENAMP_OUT_DTB}
	fi

	install -Dm 0644 ${LOPPER_OPENAMP_OUT_DTB} ${DEPLOYDIR}

	# remove existing DTB if applicable
	if test -f ${PLNX_DEPLOY_DIR}${LOPPER_OPENAMP_OUT_DTB}; then
		rm ${PLNX_DEPLOY_DIR}${LOPPER_OPENAMP_OUT_DTB}
	fi

	install -Dm 0644 ${LOPPER_OPENAMP_OUT_DTB} ${PLNX_DEPLOY_DIR}

}

do_deploy:armv7r() {
	if ${STANDALONE_SHOULD_DEPLOY}; then
		install -Dm 0644 ${D}/usr/bin/*.out ${DEPLOY_DIR}/images/${DEPLOY_MACHINE}/
	fi
}

addtask deploy before do_build after do_install
FILES:${PN}:armv7r:append = " ${libdir}/*.a /usr/bin*  "
FILES:${PN}:append:linux = " ${DEPLOYDIR}${LOPPER_OPENAMP_OUT_DTB} "
