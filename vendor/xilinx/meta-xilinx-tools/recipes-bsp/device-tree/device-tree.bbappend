FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://zynqmp-openamp.dtsi "

# openamp.dtsi is in the WORKDIR
DT_INCLUDE:append = " ${WORKDIR}"

# Change SoC to generic openamp.dtsi naming via symlink
do_configure:append:zynqmp() {
	ln -sf ${WORKDIR}/zynqmp-openamp.dtsi ${WORKDIR}/openamp.dtsi

	if [ -e "${DT_FILES_PATH}/system-top.dts" ]; then
		sed -i '/openamp\.dtsi/d' ${DT_FILES_PATH}/system-top.dts
		echo '/include/ "openamp.dtsi"' >> ${DT_FILES_PATH}/system-top.dts
	else
		bbwarn "system-top.dts not found in this configuration, cannot automatically add OpenAmp device tree nodes (openamp.dtsi)"
	fi
}
