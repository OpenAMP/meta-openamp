SRCBRANCH ?= "master"
SRCREV ?= "9c34a7484dd4bfa557c2b7273f3674b30b3b0cb2"

include libmetal.inc

SRC_URI_append = " \
	file://0001-linux-uio-dev-open-dev-file-only-if-it-exists.patch;patch=1 \
	"
