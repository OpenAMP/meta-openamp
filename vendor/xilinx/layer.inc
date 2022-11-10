# Additional bb / bbappends
BBFILES += " \
    ${LAYERDIR}/vendor/xilinx/recipes*/*/*.bb \
    ${LAYERDIR}/vendor/xilinx/recipes*/*/*.bbappend \
"

# Define dynamic layers
BBFILES_DYNAMIC += " \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bb \
    xilinx-standalone-exp:${LAYERDIR}/vendor/xilinx/meta-xilinx-standalone-experimental/recipes-*/*/*.bbappend \
"

PREFERRED_VERSION_open-amp  ?= "${@'decoupling' if ( \
    ('xilinx-standalone-exp' in d.getVar('BBFILE_COLLECTIONS').split()) and \
    (d.getVar('OPENAMP_DECOUPLING') == "1") ) else ''}"
