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

#PNBLACKLIST[open-amp] = "${@''  if d.getVar('PREFERRED_VERSION_open-amp') \
#    and (d.getVar('PREFERRED_VERSION_open-amp') != d.getVar('PV')) else \
#    ('This is not the preferred version ', d.getVar('PV'))}"


PNBLACKLIST[open-amp] = "${@ ('This is not the preferred version (%s)' % d.getVar('PV')) \
    if d.getVar('PREFERRED_VERSION_%s' % d.getVar('PN')) and \
    d.getVar('PREFERRED_VERSION_%s' % d.getVar('PN')) != d.getVar('PV') else ''}"
