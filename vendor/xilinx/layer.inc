# Define dynamic layers
BBFILES_DYNAMIC += " \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bb \
    xilinx:${LAYERDIR}/vendor/xilinx/recipes*/*/*.bbappend \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bb \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bbappend \
    xilinx-tools:${LAYERDIR}/vendor/xilinx/meta-xilinx-tools/recipes-*/*/*.bb \
    xilinx-tools:${LAYERDIR}/vendor/xilinx/meta-xilinx-tools/recipes-*/*/*.bbappend \
"

# Set to 1 to add the openamp dts nodes to device-tree recipe
ENABLE_OPENAMP_DTSI ?= "0"
