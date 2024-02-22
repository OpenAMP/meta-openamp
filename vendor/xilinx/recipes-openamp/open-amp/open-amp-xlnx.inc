OPENAMP_MACHINE:versal = "zynqmp"

CFLAGS:append:versal = " -Dversal "
CFLAGS:append:versal-net = " -DVERSAL_NET "

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = " -DWITH_APPS=ON"
EXTRA_OECMAKE:append:versal = " -DWITH_APPS=ON"

OPENAMP_SHM_DEV_NAME ?= ""
EXTRA_OECMAKE:append = " ${@'-DSHM_DEV_NAME=\"${OPENAMP_SHM_DEV_NAME}\"' if d.getVar('OPENAMP_SHM_DEV_NAME') else ''}"

OPENAMP_IPI_DEV_NAME ?= ""
EXTRA_OECMAKE:append = " ${@'-DIPI_DEV_NAME=\"${OPENAMP_IPI_DEV_NAME}\"' if d.getVar('OPENAMP_IPI_DEV_NAME') else ''}"

OPENAMP_IPI_CHN_BITMASK ?= ""
EXTRA_OECMAKE:append = " ${@'-DIPI_CHN_BITMASK=${OPENAMP_IPI_CHN_BITMASK}' if d.getVar('OPENAMP_IPI_CHN_BITMASK') else ''}"

OPENAMP_RSC_MEM_PA ?= ""
EXTRA_OECMAKE:append = " ${@'-DRSC_MEM_PA=${OPENAMP_RSC_MEM_PA}' if d.getVar('OPENAMP_RSC_MEM_PA') else ''}"

OPENAMP_VRING_MEM_PA ?= ""
EXTRA_OECMAKE:append = " ${@'-DVRING_MEM_PA=${OPENAMP_VRING_MEM_PA}' if d.getVar('OPENAMP_VRING_MEM_PA') else ''}"

OPENAMP_SHARED_BUF_PA ?= ""
EXTRA_OECMAKE:append = " ${@'-DSHARED_BUF_PA=${OPENAMP_SHARED_BUF_PA}' if d.getVar('OPENAMP_SHARED_BUF_PA') else ''}"

do_install:append () {
        # Only install echo test client, matrix multiplication client,
        # and proxy app server for ZynqMP
        rm -rf ${D}/${bindir}/*-static
}

