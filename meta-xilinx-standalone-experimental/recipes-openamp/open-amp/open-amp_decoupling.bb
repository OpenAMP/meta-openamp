SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=0e6d7bfe689fe5b0d0a89b2ccbe053fa"

SRC_URI = "git://gitenterprise.xilinx.com/OpenAMP/open-amp.git;protocol=https;branch=xlnx_decoupling"
SRCREV = "0720f88f065f11d2223cde4c790a7f35bbcc098a"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp"

inherit python3native pkgconfig cmake yocto-cmake-translation

OPENAMP_MACHINE:versal = "zynqmp"
OPENAMP_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"
EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${OPENAMP_MACHINE} \
	"

SOC_FAMILY_ARCH ??= "${TUNE_PKGARCH}"
PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

CFLAGS:versal += " -Dversal -O1 "
# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = "-DWITH_APPS=ON -DWITH_PROXY=on -DWITH_PROXY_APPS=on "
EXTRA_OECMAKE:append:versal = "-DWITH_APPS=ON -DWITH_PROXY=on -DWITH_PROXY_APPS=on "

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




DEPENDS:append = " lopper-native  "
FILESEXTRAPATHS:append := ":${THISDIR}/overlays"
SRC_URI:append = " \
     file://openamp-overlay-kernel.yaml \
          "

# We need the deployed output
do_configure[depends] += " lopper-native:do_install"

PROVIDES = "openamp"

inherit pkgconfig cmake yocto-cmake-translation

LOPS_DIR="${PYTHON_SITEPACKAGES_DIR}/lopper/lops/"
OVERLAY ?= "${S}/../openamp-overlay-kernel.yaml"
CHANNEL_INFO_FILE = "openamp-channel-info.txt"
LOPPER_OPENAMP_OUT_DTB = "${WORKDIR}/openamp-lopper-output.dtb"

LINUX_CORE:versal = "a72"
LINUX_CORE:zynqmp = "a53"

OPENAMP_LOPPER_INPUTS:linux = " \
    -i ${LOPS_DIR}/lop-${LINUX_CORE}-imux.dts \
    -i ${OVERLAY} \
    -i ${LOPS_DIR}/lop-xlate-yaml.dts \
    -i ${LOPS_DIR}/lop-load.dts \
    -i ${LOPS_DIR}/lop-openamp-versal.dts \
    -i ${LOPS_DIR}/lop-domain-${LINUX_CORE}.dts "

do_run_lopper() {
    cd ${WORKDIR}

    lopper -f -v --enhanced  --permissive \
    ${OPENAMP_LOPPER_INPUTS} \
    ${SYSTEM_DTFILE} \
    ${LOPPER_OPENAMP_OUT_DTB}

    cd -
}

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

    CHANNEL0GROUP = parse_channel_info('CHANNEL0_TO_GROUP', d)

    if d.getVar('OPENAMP_HOST') == "1":
        IPI_DEV_NAME =       parse_channel_info( CHANNEL0GROUP + "-HOST-IPI", d)
        IPI_DEV_NAME = get_ipi_str( IPI_DEV_NAME )

        IPI_CHN_BITMASK  = parse_channel_info( CHANNEL0GROUP + "-REMOTE-IPI-IRQ-VECT-ID", d)

        ELFLOADBASE =     parse_channel_info( CHANNEL0GROUP+"ELFLOAD_BASE", d)
        RSC_MEM_PA     = get_rsc_mem_pa( ELFLOADBASE )
        SHM_DEV_NAME   = get_rsc_mem_pa_str( RSC_MEM_PA )

        RSC_MEM_SIZE = "0x2000UL"

        VRING_MEM_PA = parse_channel_info( CHANNEL0GROUP+"-RX", d)
        VDEV0VRING0SIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING0_SIZE", d)
        VDEV0VRING1SIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0VRING1_SIZE", d)
        VRING_MEM_SIZE = get_sum( VDEV0VRING0SIZE, VDEV0VRING1SIZE )

        VDEV0BUFFERBASE = parse_channel_info( CHANNEL0GROUP+"VDEV0BUFFER_BASE", d)
        SHARED_BUF_PA = VDEV0BUFFERBASE

        VDEV0BUFFERSIZE = parse_channel_info( CHANNEL0GROUP+"VDEV0BUFFER_SIZE", d)
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
