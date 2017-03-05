FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

RPU_MODE_zynqmp ?= "split"
OPENAMP_DTSI_zynqmp ?= "openamp-overlay-${RPU_MODE}.dtsi"

SRC_URI_append_zynqmp ="\
    file://zynqmp/openamp-overlay-${RPU_MODE}.dtsi \
"
DTS_DIR="${WORKDIR}/${PN}"

DTS_OPENAMP_EXAMPLE ?= "false"

do_compile_prepend_zynqmp () {
    if  [ "${DTS_OPENAMP_EXAMPLE}" = "true" ]; then
        if [ "${SOC_FAMILY}" == "zynqmp" ]; then
            install -m 0644 ${WORKDIR}/${SOC_FAMILY}/${OPENAMP_DTSI} ${DTS_DIR}/
            sed -i "/${OPENAMP_DTSI}/d" ${DTS_DIR}/system-top.dts
            echo -e "\n/include/ \"${OPENAMP_DTSI}\"\n" >> ${DTS_DIR}/system-top.dts
        fi
    fi
}
