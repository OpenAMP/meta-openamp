SRCBRANCH ?= "master"
SRCREV ?= "6bbe28d41f3b97d82053c22bf1cf4a4d98d692e2"

include libmetal.inc

SRC_URI_append = " \
	file://0001-linux-uio-dev-open-dev-file-only-if-it-exists.patch;patch=1 \
	"
