FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://bsp.cfg"
KERNEL_FEATURES:append = " bsp.cfg"
SRC_URI += "file://user_2025-09-18-15-23-00.cfg \
            file://user_2025-09-22-17-54-00.cfg \
            file://user_2025-09-24-12-26-00.cfg \
            file://user_2025-10-08-15-58-00.cfg \
            file://user_2025-10-08-16-59-00.cfg \
            file://user_2025-10-08-18-36-00.cfg \
            file://user_2025-10-08-19-20-00.cfg \
            file://user_2025-10-08-19-25-00.cfg \
            file://user_2025-10-08-20-09-00.cfg \
            file://user_2025-10-10-08-13-00.cfg \
            file://user_2025-10-13-17-15-00.cfg \
            file://user_2025-10-13-18-55-00.cfg \
            file://user_2025-10-13-20-34-00.cfg \
            "

