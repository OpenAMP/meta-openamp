LIBMETAL_MACHINE:versal = "zynqmp"

EXTRA_OECMAKE:append:zynqmp = " -DWITH_VFIO=on"
EXTRA_OECMAKE:append:versal = " -DWITH_VFIO=on"

