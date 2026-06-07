# Allow recipes to be written for wrynose and later but still work for scrathgap

# this class ensures that UNPACKDIR is defined for scrathgap but does
# not interfere with the late definition of this variable in wrynose and later
# versions

# The recipe needs to set destsuffix=${BP} and S = ${UNPACKDIR}/${BP}
# (or something more specific)

# As of 2026 June, the only supported YP version that does not define UNPACKDIR
# is scarthgap.
# Although DISTRO_VERSION is 5.0 (scarthgap) or 6.0 (wrynose) in poky,
# custom distros may set these to anything.
# Therefore we key off the bitbake version for this.
# Scarthgap uses bitbake 2.8.*
python __anonymous() {
    bb_version = d.getVar('BB_VERSION') or ""

    if bb_version.startswith("2.8."):
        d.setVar('UNPACKDIR', '${WORKDIR}')
}
