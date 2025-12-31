#pragma once

/* APP-SPECIFIC
    - debug type
    - thread prio
    - filepaths
    - ...
*/
#define LOG_VERBOSITY               3
#define GAME_TASK_PRIORITY          3

#define SD_CONFIG_FILE              "config.yaml"
#define SD_INDEX_HTML               "index.html"
#define SD_DEFAULT_VOLUME           "0:/"

#define TCP_QUERY_MSG_SIZE          10240
#define TCP_QUERY_MAX_QUEUE_ITEMS   100
#define TCP_THREAD_STACKSIZE        1024
#define TCP_HTTP_PORT               80
#define TCP_MAX_CLIENTS             10
#define TCP_SETUP_WAIT              500
/* VDMA-HDMI 
    - docs: https://docs.amd.com/r/en-US/pg020_axi_vdma/MM2S_VDMACR-MM2S-VDMA-Control-Register-Offset-00h
*/
#define HDMI_HSIZE              1920
#define HDMI_VSIZE              1080
#define HDMI_RGB                3

#define OFFST_R                 16
#define OFFST_G                 0
#define OFFST_B                 8
#define uCHAR_MAX               0xFF

#define FRAME_BUFFER_PTR1       0x10000000
#define FRAME_BUFFER_PTR2       0x118C0000
#define FRAME_BUFFER_PTR3       0x13180000

#define VDMA_CTRL_START         0x3
#define VDMA_CTRL_RESET         0x4
#define VDMA_CTRL_STOP          0x0
#define VDMA_STATUS_ERR         (1 << 4)
#define VDMA_STATUS_SLVERR      (1 << 5)
#define VDMA_STATUS_DECERR      (1 << 6)

#define VDMA_CTRL_REG           0x00
#define VDMA_STATUS_REG         0x04  
#define VDMA_FB1_REG            0x5C
#define VDMA_FB2_REG            0x60
#define VDMA_FB3_REG            0x64
#define VDMA_HSIZE_REG          0x54
#define VDMA_VSIZE_REG          0x50
#define VDMA_FRAME_STRIDE_REG   0x58
