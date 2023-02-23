LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"

include ${LAYER_PATH_openamp-layer}/recipes-openamp/open-amp/open-amp.inc
require ${LAYER_PATH_openamp-layer}/vendor/xilinx/recipes-openamp/open-amp/open-amp-xlnx.inc

SRCREV = "7513776e35225ce604773b6d42316a93ef544c60"
BRANCH = "main"
SRCBRANCH = "${BRANCH}"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=ab88daf995c0bd0071c2e1e55f3d3505"

#S = "${WORKDIR}/git"
#B = "${WORKDIR}/build"
#OECMAKE_SOURCEPATH = "${S}/"
DEPENDS:append:armv7r:xilinx-standalone = " libmetal xilstandalone python3-pyyaml-native lopper-native python3-dtc-native  "
DTS_FILE = "/scratch/decoupling/lopper/lopper-sdt.dtb"
FILESEXTRAPATHS:prepend := "${THISDIR}/overlays:"
SRC_URI:append:zynqmp = "  file://openamp-overlay-zynqmp.yaml "
SRC_URI:append:versal = "  file://openamp-overlay-versal.yaml "
OPENAMP_OVERLAY:zynqmp ?= "${S}/../openamp-overlay-zynqmp.yaml"
OPENAMP_OVERLAY:versal ?= "${S}/../openamp-overlay-versal.yaml"

inherit cmake deploy python3-dir features_check

BB_STRICT_CHECKSUM = "0"
EXTRA_OECMAKE:append = " -DSOC_FAMILY=\"${SOC_FAMILY}\" "
COMPATIBLE_HOST = ".*-elf"
COMPATIBLE_HOST:arm = "[^-]*-[^-]*-eabi"
OPENAMP_CMAKE_MACHINE:versal = "Versal"
OPENAMP_CMAKE_MACHINE:zynqmp = "Zynqmp"
OPENAMP_MACHINE = "zynqmp_r5"

def get_cross_prefix(oe_cmake_c_compiler):
  if oe_cmake_c_compiler == 'arm-xilinx-eabi-gcc':
    return 'arm-xilinx-eabi-'

OPENAMP_CROSS_PREFIX:armv7r:xilinx-standalone = "${@get_cross_prefix(d.getVar('OECMAKE_C_COMPILER'))}"

REQUIRED_DISTRO_FEATURES:armv7r:xilinx-standalone = "${DISTRO_FEATURES}"

FILES:${PN}-demos:armv7r:xilinx-standalone = " \
    ${base_libdir}/firmware/*\.out \
    ${bindir}/*out \
"

LOPS_DIR="${RECIPE_SYSROOT_NATIVE}/${PYTHON_SITEPACKAGES_DIR}/lopper/lops/"
CHANNEL_INFO_FILE = "openamp-channel-info.txt"
PACKAGE_DEBUG_SPLIT_STYLE:armv7r:xilinx-standalone='debug-file-directory'
INHIBIT_PACKAGE_STRIP:armv7r:xilinx-standalone = '1'
INHIBIT_PACKAGE_DEBUG_SPLIT:armv7r:xilinx-standalone = '1'
PACKAGE_MINIDEBUGINFO:armv7r:xilinx-standalone = '1'

LOPPER_OPENAMP_OUT_DTB = "${WORKDIR}/openamp-lopper-output.dtb"
OPENAMP_DTFILE = "${SYSTEM_DTFILE}"

do_run_lopper() {
    cd ${WORKDIR}

    lopper -f -v --enhanced  --permissive \
    -i ${OPENAMP_OVERLAY} \
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


SELECTED_OPTIMIZATION:armv7r:xilinx-standalone = " -g "

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

    HOSTBITMASK =   parse_channel_info(  "CHANNEL0TO_HOST-BITMASK", d)
    HOSTIPI =       parse_channel_info(  "CHANNEL0TO_HOST", d)
    REMOTEBITMASK = parse_channel_info(  "CHANNEL0TO_REMOTE-BITMASK", d)
    REMOTEIPI =         parse_channel_info("CHANNEL0TO_REMOTE", d)
    HOST_IRQVECTID    = parse_channel_info("CHANNEL0TO_HOST-IPIIRQVECTID", d)
    REMOTE_IRQVECTID  = parse_channel_info("CHANNEL0TO_REMOTE-IPIIRQVECTID", d)

    IPI_DEV_NAME = get_ipi_str( HOSTIPI )

    ELFLOADBASE =     parse_channel_info( "CHANNEL0ELFBASE", d)
    ELFLOADSIZE =     parse_channel_info( "CHANNEL0ELFSIZE", d)
    VDEV0BUFFERBASE = parse_channel_info( "CHANNEL0VDEV0BUFFERBASE", d)
    VDEV0BUFFERSIZE = parse_channel_info( "CHANNEL0VDEV0BUFFERSIZE", d)
    VDEV0VRING0BASE = parse_channel_info( "CHANNEL0VRING0BASE", d)
    VDEV0VRING0SIZE = parse_channel_info( "CHANNEL0VRING0SIZE" , d)
    VDEV0VRING1BASE = parse_channel_info( "CHANNEL0VRING1BASE", d)
    VDEV0VRING1SIZE = parse_channel_info( "CHANNEL0VRING1SIZE", d)
    TX =              parse_channel_info( "CHANNEL0VDEV0BUFFERTX", d)
    RX =              parse_channel_info( "CHANNEL0VDEV0BUFFERRX", d)

    VRING_MEM_SIZE = hex( int(VDEV0VRING0SIZE,16) + int(VDEV0VRING1SIZE, 16) )

    RSC_MEM_PA     = get_rsc_mem_pa( ELFLOADBASE )
    SHM_DEV_NAME   = get_rsc_mem_pa_str( RSC_MEM_PA )

    d.setVar("RING_RX",            RX)
    d.setVar("RING_TX",            TX)
    d.setVar("SHARED_MEM_PA",      VDEV0VRING0BASE)
    d.setVar("SHARED_BUF_OFFSET",  VRING_MEM_SIZE)
    d.setVar("HOSTBITMASK",        REMOTEBITMASK)
    d.setVar("POLL_BASE_ADDR",     REMOTEIPI)
    d.setVar("IPI_CHN_BITMASK",    HOSTBITMASK)
    d.setVar("IPI_IRQ_VECT_ID",    REMOTE_IRQVECTID)
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
      "set( CMAKE_C_ARCHIVE_CREATE \"<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>\")",
      "set( CMAKE_SYSTEM_NAME \"Generic\")",
      "set( MACHINE \"zynqmp_r5\")",

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
    toolchain_file.write("\n")
    toolchain_file.write("add_definitions( -DMACHINE=\"zynqmp_r5\" -DCMAKE_MACHINE=\"zynqmp_r5\"  )")
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
