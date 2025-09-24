FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://bsp.cfg"
KERNEL_FEATURES:append = " bsp.cfg"
SRC_URI += "file://user_2025-09-18-15-23-00.cfg \
            file://user_2025-09-22-17-54-00.cfg \
            file://user_2025-09-24-12-26-00.cfg \
            "

