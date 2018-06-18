SRCBRANCH ?= "master"
SRCREV ?= "9c34a7484dd4bfa557c2b7273f3674b30b3b0cb2"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=395307789d21fd8945fc1c933cad18b5"

include libmetal.inc

SRC_URI_append = " \
	file://0001-linux-uio-dev-open-dev-file-only-if-it-exists.patch;patch=1 \
	"
