FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

RPU_MODE_zynqmp ?= "split"
OPENAMP_DTSI_zynqmp ?= "openamp-overlay-${RPU_MODE}.dtsi"

SRC_URI_append_zynqmp ="\
    file://openamp-overlay-${RPU_MODE}.dtsi \
"
DTS_DIR="${WORKDIR}/${PN}"

DTS_OPENAMP_OVERLAY_zynqmp := "${@bb.utils.contains('DISTRO_FEATURES', 'openamp', 'true', 'false', d)}"

do_compile_prepend_zynqmp () {
	if  [ "${DTS_OPENAMP_OVERLAY}" = "true" ]; then
		install -m 0644 ${WORKDIR}/openamp-overlay-${RPU_MODE}.dtsi ${DTS_DIR}/
		sed -i "/${OPENAMP_DTSI}/d" ${DTS_DIR}/system-top.dts
		echo -e "\n/include/ \"${OPENAMP_DTSI}\"\n" >> ${DTS_DIR}/system-top.dts
	fi
}
