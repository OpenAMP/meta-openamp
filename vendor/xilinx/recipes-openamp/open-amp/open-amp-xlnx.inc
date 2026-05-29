OPENAMP_MACHINE:versal = "zynqmp"
OPENAMP_MACHINE:versal-net = "zynqmp"

CFLAGS:append:versal = " -Dversal "
CFLAGS:append:versal-net = " -DVERSAL_NET "
