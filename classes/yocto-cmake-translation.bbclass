#
# this class handles conversion of TARGET_OS and combination of
# SOC_FAMILY and processor information for openamp and libmetal recipes.
#

# Linux:
#  'zynq' isn't supported
#  'zynqmp' and 'versal' should both return 'zynqmp'
# Baremetal:
#  'microblaze_generic'
#  'zynqmp_r5' (both zynqmp and versal RPU)
#  'zynqmp_a53' (both zynqmp and versal APU)
def get_cmake_machine(target_os, tune_arch, soc_fam, d):
 if target_os.startswith('linux'):
  if (soc_fam == 'zynqmp' or soc_fam == 'versal'):
    return 'zynqmp'
 elif (target_os == 'elf' or target_os == 'eabi'):
  if ('microblaze' in tune_arch):
   return 'microblaze_generic'
  elif ('cortexr5' in tune_arch):
   return 'zynqmp_r5'
  elif (soc_fam == 'zynqmp' or soc_fam == 'versal'):
   return 'zynqmp_a53'

  return ''
