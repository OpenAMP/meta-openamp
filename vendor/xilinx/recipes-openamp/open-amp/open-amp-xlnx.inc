OPENAMP_MACHINE:versal = "zynqmp"

CFLAGS:append:versal = " -Dversal "
CFLAGS:append:versal-net = " -DVERSAL_NET "

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = " -DWITH_APPS=ON"
EXTRA_OECMAKE:append:versal = " -DWITH_APPS=ON"


OPENAMP_SHM_DEV_NAME ?= ""
OPENAMP_IPI_DEV_NAME ?= ""

# Do this so that C String Literals can propagate properly with '"' and '.'
do_configure:append () {
    if [ -n "${OPENAMP_IPI_DEV_NAME}" ]; then
        cat << EOF >> ${WORKDIR}/git/apps/machine/zynqmp/platform_info.h
#ifdef IPI_DEV_NAME
  #undef IPI_DEV_NAME
#endif
#define IPI_DEV_NAME "${OPENAMP_IPI_DEV_NAME}"
EOF
    fi
    if [ -n "${OPENAMP_SHM_DEV_NAME}" ]; then
        cat << EOF >> ${WORKDIR}/git/apps/machine/zynqmp/platform_info.h
#ifdef SHM_DEV_NAME
  #undef SHM_DEV_NAME
#endif
#define SHM_DEV_NAME "${OPENAMP_SHM_DEV_NAME}"
EOF
    fi

}

OPENAMP_IPI_CHN_BITMASK ?= ""
OECMAKE_C_FLAGS:append = " ${@'-DIPI_CHN_BITMASK=${OPENAMP_IPI_CHN_BITMASK}' if d.getVar('OPENAMP_IPI_CHN_BITMASK') else ''}"

OPENAMP_RSC_MEM_PA ?= ""
OECMAKE_C_FLAGS:append = " ${@'-DRSC_MEM_PA=${OPENAMP_RSC_MEM_PA}' if d.getVar('OPENAMP_RSC_MEM_PA') else ''}"

OPENAMP_VRING_MEM_PA ?= ""
OECMAKE_C_FLAGS:append = " ${@'-DVRING_MEM_PA=${OPENAMP_VRING_MEM_PA}' if d.getVar('OPENAMP_VRING_MEM_PA') else ''}"

OPENAMP_SHARED_BUF_PA ?= ""
OECMAKE_C_FLAGS:append = " ${@'-DSHARED_BUF_PA=${OPENAMP_SHARED_BUF_PA}' if d.getVar('OPENAMP_SHARED_BUF_PA') else ''}"

do_install:append () {
        # Only install echo test client, matrix multiplication client,
        # and proxy app server for ZynqMP
        rm -rf ${D}/${bindir}/*-static
}

