FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://zynqmp-openamp.dtsi \
    file://versal-openamp.dtsi \
"

do_configure:prepend:zynqmp() {
	if [ ${ENABLE_OPENAMP_DTSI} = "1" ]; then
		ln -sf ${WORKDIR}/zynqmp-openamp.dtsi ${WORKDIR}/openamp.dtsi
	else
		echo "ENABLE_OPENAMP_DTSI is not SET (${ENABLE_OPENAMP_DTSI})"
	fi
}

do_configure:prepend:versal() {
	if [ ${ENABLE_OPENAMP_DTSI} = "1" ]; then
		ln -sf ${WORKDIR}/versal-openamp.dtsi ${WORKDIR}/openamp.dtsi
        else
		echo "ENABLE_OPENAMP_DTSI is not SET (${ENABLE_OPENAMP_DTSI})"
	fi
}

# openamp.dtsi is in the WORKDIR
DT_INCLUDE:append = " ${WORKDIR}"

# Change SoC to generic openamp.dtsi naming via symlink
do_configure:prepend() {
	if [ -e ${WORKDIR}/openamp.dtsi ]; then
		if [ -e "${DT_FILES_PATH}/system-top.dts" ]; then
			sed -i '/openamp\.dtsi/d' ${DT_FILES_PATH}/system-top.dts
			echo '/include/ "openamp.dtsi"' >> ${DT_FILES_PATH}/system-top.dts
		else
			bbwarn "system-top.dts not found in this configuration, cannot automatically add OpenAmp device tree nodes (openamp.dtsi)"
		fi
	fi
}
