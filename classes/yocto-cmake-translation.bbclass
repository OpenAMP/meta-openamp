#
# this class handles conversion of TARGET_OS and combination of
# SOC_FAMILY and processor information for openamp and libmetal recipes.
#

def get_cmake_machine(target_os, tune_arch, soc_fam, d):
 if (target_os == 'linux'):
  return soc_fam
 elif (target_os == 'elf'):
  if ('microblaze' in tune_arch):
   if ('pmu' in d.getVar('MACHINE')):
    return soc_fam + '_pmu'
 else:
  return soc_fam
