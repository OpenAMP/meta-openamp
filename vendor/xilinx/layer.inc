# Define dynamic layers
BBFILES_DYNAMIC += " \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bb \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bbappend \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bb \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bbappend \
"
