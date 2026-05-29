# Define dynamic layers
BBFILES_DYNAMIC += " \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bb \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bbappend \
    xilinx-standalone-sdt:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-sdt/recipes-*/*/*.bb \
    xilinx-standalone-sdt:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-sdt/recipes-*/*/*.bbappend \
"
