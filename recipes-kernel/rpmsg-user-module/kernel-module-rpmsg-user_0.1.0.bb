SUMMARY = "RPMsg user module example. It is for kernel v4.9 and above"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING.GPL;md5=fcb02dc552a041dee27e4b85c7396067"

inherit module

SRC_URI = "\
	file://COPYING.GPL \
	file://Makefile \
	file://rpmsg_user_dev_driver.c \
	"

S = "${WORKDIR}"

