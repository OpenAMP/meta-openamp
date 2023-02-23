SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

include ${LAYER_PATH_openamp-layer}/recipes-openamp/open-amp/open-amp.inc
require ${LAYER_PATH_openamp-layer}/vendor/xilinx/recipes-openamp/open-amp/open-amp-xlnx.inc

SRCREV = "7513776e35225ce604773b6d42316a93ef544c60"
BRANCH = "main"
SRCBRANCH = "${BRANCH}"
PV = "${SRCBRANCH}+git${SRCPV}"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=ab88daf995c0bd0071c2e1e55f3d3505"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"
COMPATIBLE_HOST = ".*"
SOC_FAMILY_ARCH ??= "${TUNE_PKGARCH}"
PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

DEPENDS:append = " python3-pyyaml-native python3-dtc-native lopper-native "
FILESEXTRAPATHS:prepend := "${THISDIR}/overlays:"

OPENAMP_YAML = "openamp-overlay-${SOC_FAMILY}.yaml"
SRC_URI:append = "  file://${OPENAMP_YAML} "
OPENAMP_OVERLAY ?= "${S}/../${OPENAMP_YAML}"

# We need the deployed output
do_configure[depends] += " lopper-native:do_install"

inherit pkgconfig cmake yocto-cmake-translation python3-dir

LOPS_DIR="${RECIPE_SYSROOT_NATIVE}/${PYTHON_SITEPACKAGES_DIR}/lopper/lops/"
CHANNEL_INFO_FILE = "openamp-channel-info.txt"
LOPPER_OPENAMP_OUT_DTB = "${WORKDIR}/openamp-lopper-output.dtb"
OPENAMP_DTFILE = "${SYSTEM_DTFILE}"

LINUX_CORE:versal = "a72"
LINUX_CORE:zynqmp = "a53"

# The order of lops is as follows:
# 1. lop-load - This must be run first to ensure enabled lops and plugins
#    can be used.
# 2. lop-xlate-yaml - Ensure any following lops can be YAML
# 3. imux - This is used to make sure the imux node has correct
#    interrupt parent and interrupt-multiplex node is trimmed. This is
#    present for all Xilinx Lopper runs, with or without OpenAMP. This
#    is done first for all lop runs as the imux node may be referenced by
#    later plugins.
# 4. domain - domain processing should be done before OpenAMP as there
#    can be nodes that are stripped out or modified based on the domain.
# 5. OpenAMP - This lopper processing is done on top of the domain as
#    noted due to domain reasons above.
# 6. domain-prune - Only prune files AFTER all other processing is complete
#    so that a lopper plugin or lop does not inadvertently process a
#    non-existent node.
OPENAMP_LOPPER_INPUTS:zynqmp:linux = "            \
    -i ${LOPS_DIR}/lop-a53-imux.dts               \
    -i ${LOPS_DIR}/lop-domain-linux-a53.dts       \
    -i ${LOPS_DIR}/lop-openamp-versal.dts         \
    -i ${LOPS_DIR}/lop-domain-linux-a53-prune.dts "

OPENAMP_LOPPER_INPUTS:versal:linux = "      \
    -i ${LOPS_DIR}/lop-a72-imux.dts         \
    -i ${LOPS_DIR}/lop-domain-a72.dts      \
    -i ${LOPS_DIR}/lop-openamp-versal.dts  \
    -i ${LOPS_DIR}/lop-domain-a72-prune.dts "

do_run_lopper() {
    cd ${WORKDIR}

    lopper -f -v --enhanced  --permissive \
    -i ${OPENAMP_OVERLAY}		  \
    -i ${LOPS_DIR}/lop-load.dts           \
    -i ${LOPS_DIR}/lop-xlate-yaml.dts     \
    ${OPENAMP_LOPPER_INPUTS} \
    ${OPENAMP_DTFILE} \
    ${LOPPER_OPENAMP_OUT_DTB}

    cd -
}

do_run_lopper[depends] += " lopper-native:do_install"
addtask run_lopper before do_generate_toolchain_file
addtask run_lopper after do_prepare_recipe_sysroot

OPENAMP_HOST:standalone = "0"
OPENAMP_HOST:linux = "1"

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


    def get_sum(a, b):
        return hex( int(a,16) + int(b, 16) )

    def get_rsc_mem_pa(base):
        return hex( int(base,16) + 0x20000 )

    def get_rsc_mem_pa_str(val):
        return "\"" + val.replace('0x','') + '.shm' + "\""

    def get_ipi_str(val):
        return "\""+val.replace('0x','').lower() +'.ipi'+"\""

    if d.getVar('OPENAMP_HOST') == "1":
        IPI_DEV_NAME =       parse_channel_info( "CHANNEL0TO_HOST", d)
        IPI_DEV_NAME = get_ipi_str( IPI_DEV_NAME )

        IPI_CHN_BITMASK  = parse_channel_info( "CHANNEL0TO_REMOTE", d)

        ELFLOADBASE =     parse_channel_info( "CHANNEL0ELFBASE", d)
        RSC_MEM_PA  = ELFLOADBASE
        SHM_DEV_NAME   = get_rsc_mem_pa_str( RSC_MEM_PA )

        RSC_MEM_SIZE = "0x2000UL"

        VRING_MEM_PA = parse_channel_info( "CHANNEL0VRING0BASE", d)
        VDEV0VRING0SIZE = parse_channel_info( "CHANNEL0VRING0SIZE", d)
        VDEV0VRING1SIZE = parse_channel_info( "CHANNEL0VRING1SIZE", d)
        VRING_MEM_SIZE = hex( int(VDEV0VRING0SIZE,16) + int(VDEV0VRING1SIZE, 16) )

        VDEV0BUFFERBASE = parse_channel_info( "CHANNEL0VDEV0BUFFERBASE", d)
        SHARED_BUF_PA = VDEV0BUFFERBASE

        VDEV0BUFFERSIZE = parse_channel_info( "CHANNEL0VDEV0BUFFERSIZE", d)
        SHARED_BUF_SIZE = VDEV0BUFFERSIZE

        d.setVar("IPI_DEV_NAME", IPI_DEV_NAME)
        d.setVar("IPI_CHN_BITMASK", IPI_CHN_BITMASK)
        d.setVar("RSC_MEM_PA", RSC_MEM_PA)
        d.setVar("SHM_DEV_NAME", SHM_DEV_NAME)
        d.setVar("RSC_MEM_SIZE", RSC_MEM_SIZE)
        d.setVar("VRING_MEM_PA", VRING_MEM_PA)
        d.setVar("VRING_MEM_SIZE", VRING_MEM_SIZE)
        d.setVar("SHARED_BUF_PA", SHARED_BUF_PA)
        d.setVar("SHARED_BUF_SIZE", SHARED_BUF_SIZE)

}
# run lopper before parsing lopper-generated file
do_set_openamp_cmake_vars[prefuncs]  += "do_run_lopper"

# set openamp vars before using them in toolchain file
do_generate_toolchain_file[prefuncs] += "do_set_openamp_cmake_vars"

python openamp_toolchain_file_setup() {
    toolchain_file_path = d.getVar("WORKDIR") + "/toolchain.cmake"
    toolchain_file = open(toolchain_file_path, "a") # a for append

    # openamp app specific info
    config_vars = []
    if d.getVar('OPENAMP_HOST') == "1":
        config_vars = [ "IPI_CHN_BITMASK", "IPI_DEV_NAME", "SHM_DEV_NAME",
                        "RSC_MEM_PA", "RSC_MEM_SIZE", "VRING_MEM_PA",
                        "VRING_MEM_SIZE", "SHARED_BUF_PA", "SHARED_BUF_SIZE" ]

    defs = " "
    for cv in config_vars:
        defs += " -D" + cv + "=" + d.getVar(cv)

    toolchain_file.write("add_definitions( " + defs + " )")
}

do_generate_toolchain_file[postfuncs] += "openamp_toolchain_file_setup"

do_deploy() {
	install -Dm 0644 ${LOPPER_OPENAMP_OUT_DTB} ${DEPLOY_DIR_IMAGE}/
}

addtask deploy before do_build after do_install
