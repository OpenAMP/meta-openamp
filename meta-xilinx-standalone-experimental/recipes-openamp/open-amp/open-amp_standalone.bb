LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"
SRC_URI_armrm_xilinx-standalone = "git://github.com/Xilinx/open-amp.git;protocol=https;branch=experimental_dt"

SRCREV = "0720f88f065f11d2223cde4c790a7f35bbcc098a"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"
OECMAKE_SOURCEPATH = "${S}/"
PROVIDES = "openamp"
DEPENDS_armrm_xilinx-standalone += " libmetal xilstandalone python3-pyyaml-native lopper-native python3-dtc-native  nativesdk-xilinx-lops "
FILESEXTRAPATHS_append := ":${THISDIR}/overlays"
#SRC_URI_versal_append = "    file://openamp-overlay-kernel-versal.yaml  "
#SRC_URI_zynqmp_append = "    file://openamp-overlay-kernel-zynqmp.yaml  "
SRC_URI_append = " file://openamp-overlay-kernel-${SOC_FAMILY}.yaml "

inherit cmake deploy
# We need the deployed output
do_configure_armrm_xilinx-standalone[depends] += "device-tree-lops:do_deploy lopper-native:do_install"
do_compile_armrm_xilinx-standalone[depends] += "device-tree-lops:do_deploy"
do_install_armrm_xilinx-standalone[depends] += "device-tree-lops:do_deploy"

BB_STRICT_CHECKSUM = "0"


EXTRA_OECMAKE = " \
       -DLIB_INSTALL_DIR=${libdir} \
       -DSOC_FAMILY="${SOC_FAMILY}" \
       "

COMPATIBLE_HOST = ".*-elf"
COMPATIBLE_HOST_arm = "[^-]*-[^-]*-eabi"

OPENAMP_CMAKE_MACHINE_versal = "Versal"
OPENAMP_CMAKE_MACHINE_zynqmp = "Zynqmp"

def get_cross_prefix(oe_cmake_c_compiler):
  if oe_cmake_c_compiler == 'arm-xilinx-eabi-gcc':
    return 'arm-xilinx-eabi-'

OPENAMP_CROSS_PREFIX_armrm_xilinx-standalone = "${@get_cross_prefix(d.getVar('OECMAKE_C_COMPILER'))}"

def get_openamp_machine(soc_family):
  if soc_family in ['versal', 'zynqmp']:
    return 'zynqmp_r5'
  return ''


OPENAMP_MACHINE = "${@get_openamp_machine(d.getVar('SOC_FAMILY'))}"


ALLOW_EMPTY_${PN}-demos = "1"
PACKAGES_append += "${PN}-demos"
EXTRA_OECMAKE_append = "-DWITH_APPS=ON "

REQUIRED_DISTRO_FEATURES_armrm_xilinx-standalone = "${DISTRO_FEATURES}"
PACKAGECONFIG_armrm_xilinx-standalone ?= "${DISTRO_FEATURES} ${MACHINE_FEATURES}"

FILES_${PN}-demos_armrm_xilinx-standalone = " \
    ${base_libdir}/firmware/*\.out \
"

LOPS_DIR="${RECIPE_SYSROOT_NATIVE}/usr/share/lopper/lops/"
OVERLAY_zynqmp ?= "${S}/../openamp-overlay-kernel-zynqmp.yaml"
OVERLAY_versal ?= "${S}/../openamp-overlay-kernel-versal.yaml"

CHANNEL_INFO_FILE = "openamp-channel-info.txt"

PACKAGE_DEBUG_SPLIT_STYLE_armrm_xilinx-standalone='debug-file-directory'
INHIBIT_PACKAGE_STRIP_armrm_xilinx-standalone = '1'
INHIBIT_PACKAGE_DEBUG_SPLIT_armrm_xilinx-standalone = '1'
PACKAGE_MINIDEBUGINFO_armrm_xilinx-standalone = '1'

do_run_lopper() {
    cd ${WORKDIR}

    ${LOPS_DIR}/../lopper.py -f -v --enhanced  --permissive \
    -i ${OVERLAY} \
    -i ${LOPS_DIR}/lop-xlate-yaml.dts \
    -i ${LOPS_DIR}/lop-load.dts \
    -i ${LOPS_DIR}/lop-openamp-versal.dts \
    ${SYSTEM_DTFILE} \
    ${WORKDIR}/openamp-lopper-output.dts

    # old SDT /scratch/decoupling/lopper/lopper-sdt.dts

    cd -
}

addtask run_lopper before do_generate_toolchain_file
addtask run_lopper after do_prepare_recipe_sysroot


SELECTED_OPTIMIZATION_armrm_xilinx-standalone = " -g "

python do_set_openamp_cmake_vars() {
    def parse_channel_info( val, d ):
        filename = d.getVar('WORKDIR') + '/' + d.getVar('CHANNEL_INFO_FILE')
        with open(filename, "r") as f:
            lines = f.readlines()
            for l in lines:
                if val in l:
                    ret = l.replace(val+'=','').replace('\n','').replace('"','').upper().replace('resource','').replace('0X','0x')
                    if 'TO_GROUP' in val:
                        ret = ret.split('-')[2].replace('@','_').upper()
                    return ret

        return ""


    def get_vring_mem_size(vring0, vring1):
        return hex( int(vring0,16) + int(vring1, 16) )

    def get_rsc_mem_pa(base):
        return hex( int(base,16) + 0x20000 )

    def get_rsc_mem_pa_str(val):
        return val.replace('0x','') + '.shm'

    def get_ipi_str(val):
        return val.replace('0x','') +'.ipi'

    CHANNEL0GROUP = parse_channel_info('CHANNEL0_TO_GROUP', d)

    HOSTBITMASK =   parse_channel_info( CHANNEL0GROUP + "-HOST-BITMASK", d)
    HOSTIPI =       parse_channel_info( CHANNEL0GROUP + "-HOST-IPI", d)
    REMOTEBITMASK = parse_channel_info( CHANNEL0GROUP + "-HOST-BITMASK", d)
    REMOTEIPI =         parse_channel_info( CHANNEL0GROUP + "-REMOTE-IPI", d)
    HOST_IRQVECTID    = parse_channel_info( CHANNEL0GROUP + "-HOST-IPI-IRQ-VECT-ID", d)
    REMOTE_IRQVECTID  = parse_channel_info( CHANNEL0GROUP + "-REMOTE-IPI-IRQ-VECT-ID", d)

    IPI_DEV_NAME = get_ipi_str( HOSTIPI )

    ELFLOADBASE =     parse_channel_info( CHANNEL0GROUP+"ELFLOAD_BASE", d)
    ELFLOADSIZE =     parse_channel_info( CHANNEL0GROUP+"ELFLOAD_SIZE", d)
    VDEV0BUFFERBASE = parse_channel_info( CHANNEL0GROUP+"VDEV0BUFFER_BASE", d)
    VDEV0BUFFERSIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0BUFFER_SIZE", d)
    VDEV0VRING0BASE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING0_BASE", d)
    VDEV0VRING0SIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING0_SIZE", d)
    VDEV0VRING1BASE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING1_BASE", d)
    VDEV0VRING1SIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING1_SIZE", d)
    TX =              parse_channel_info( CHANNEL0GROUP+"-TX", d)
    RX =              parse_channel_info( CHANNEL0GROUP+"-RX", d)

    VRING_MEM_SIZE = get_vring_mem_size( VDEV0VRING0SIZE, VDEV0VRING1SIZE )
    RSC_MEM_PA     = get_rsc_mem_pa( ELFLOADBASE )
    SHM_DEV_NAME   = get_rsc_mem_pa_str( RSC_MEM_PA )

    d.setVar("RING_RX",            RX)
    d.setVar("RING_TX",            TX)
    d.setVar("SHARED_MEM_PA",      VDEV0VRING0BASE)
    d.setVar("SHARED_MEM_SIZE",    VDEV0BUFFERSIZE)
    d.setVar("SHARED_BUF_OFFSET",  VRING_MEM_SIZE)
    d.setVar("HOSTBITMASK",        REMOTEBITMASK)
    d.setVar("POLL_BASE_ADDR",     REMOTEIPI)
    d.setVar("IPI_CHN_BITMASK",    HOSTBITMASK)
    d.setVar("IPI_IRQ_VECT_ID",    REMOTE_IRQVECTID)
    d.setVar("SHARED_MEM_PA",      VDEV0VRING0BASE)
    d.setVar("SHARED_MEM_SIZE",    VDEV0BUFFERSIZE)
    d.setVar("SHARED_BUF_OFFSET",  VRING_MEM_SIZE)

}
# run lopper before parsing lopper-generated file
do_set_openamp_cmake_vars[prefuncs]  += "do_run_lopper"

# set openamp vars before using them in toolchain file
do_generate_toolchain_file[prefuncs] += "do_set_openamp_cmake_vars"

python openamp_toolchain_file_setup() {
    toolchain_file_path = d.getVar("WORKDIR") + "/toolchain.cmake"
    toolchain_file = open(toolchain_file_path, "a") # a for append

    # generate boilerplate for toolchain file
    lines = [
      "set( CMAKE_SYSTEM_PROCESSOR \"" + d.getVar("TRANSLATED_TARGET_ARCH") + "\" )",
      "set( MACHINE \""                + d.getVar("OPENAMP_MACHINE")        + "\" )",
      "set( CMAKE_MACHINE \""          + d.getVar("OPENAMP_CMAKE_MACHINE")  + "\" )",
      "set( CMAKE_C_ARCHIVE_CREATE \"<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>\")",
      "set( CMAKE_SYSTEM_NAME \"Generic\")",
      "set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING \"\")",
      "set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING \"\")",
      "set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING \"\")",
      "include (CMakeForceCompiler)",
      "CMAKE_FORCE_C_COMPILER( \""        + d.getVar("OECMAKE_C_COMPILER") + "\" GNU)",
      "set (CROSS_PREFIX \""           + d.getVar("OPENAMP_CROSS_PREFIX") + "\" )",
      "set (CMAKE_C_ARCHIVE_FINISH true)",
      "set (CMAKE_INCLUDE_PATH \""     + d.getVar("S") + "/../recipe-sysroot/usr/include/\" CACHE STRING \"\")",
      "include (cross_generic_gcc)",
    ]
    for line in lines:
        toolchain_file.write(line + "\n")

    # openamp app specific info
    config_vars = [ "RING_RX", "RING_TX", "SHARED_MEM_PA", "SHARED_MEM_SIZE", "SHARED_BUF_OFFSET", "POLL_BASE_ADDR", "IPI_CHN_BITMASK", "IPI_IRQ_VECT_ID"]

    defs = " "
    for cv in config_vars:
        defs += " -D" + cv + "=" + d.getVar(cv)

    toolchain_file.write("add_definitions( " + defs + " )")
}

do_generate_toolchain_file[postfuncs] += "openamp_toolchain_file_setup"

# deploy for other recipes
DEPLOY_MACHINE = "${@ d.getVar('MACHINE_ARCH').replace('_','-') }"
SHOULD_DEPLOY = "${@'true' if ( 'Standalone' in  d.getVar('DISTRO_NAME') ) else 'false'}"
do_deploy() {
    echo "get the following: ";
    if ${SHOULD_DEPLOY}; then
        install -Dm 0644 ${D}/usr/bin/*.out ${DEPLOY_DIR}/images/${DEPLOY_MACHINE}/
    fi
}
addtask deploy before do_build after do_install
