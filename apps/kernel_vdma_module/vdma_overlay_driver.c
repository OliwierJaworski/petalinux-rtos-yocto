// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2010-2014 Xilinx, Inc. All rights reserved.
 * Copyright (C) 2025 Oliwier Jaworski. Modifications under GPL-2.0-or-later
 */

#include "asm-generic/errno.h"
#include "linux/device.h"
#include "linux/gfp_types.h"
#include <linux/dma/xilinx_dma.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/of_dma.h>
#include <linux/platform_device.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/sched/task.h>
#include <linux/wait.h>

#include "linux/types.h"
#include "vdma_overlay.h"

#define TEST_SIZE 			1024

/* set on insmod to 1 for testing */
static unsigned internal_test = 0;
module_param(internal_test, int, S_IRUGO);

struct bd_channel{
    struct completion cmp;
    dma_cookie_t tx_cookie;
    dma_addr_t dma_handle;
    struct scatterlist sglist;
};

struct vdma_channel{
    struct dma_chan *tx;
    u32 frm_cnt;
    u32 direction;
    dma_addr_t bfr_phys_addr;

    struct xilinx_vdma_config config;
    enum dma_transaction_type type;
    u8 **srcs;
    u8 **dsts;
    struct channel_buffer *btable_p;
    struct bd_channel bdtable[BUFFER_COUNT];
    u32 bdindex;
};

struct vdma_overlay{
    struct vdma_channel *chan;
};

/* Prepare a DMA buffer to be used in a DMA transaction, submit it to the DMA engine
 * to be queued and return a cookie that can be used to track that status of the
 * transaction
 */
static void sync_callback(void *completion)
{
	/* Indicate the DMA transaction completed to allow the other
	 * thread of control to finish processing
	 */
	complete(completion);
}

static void 
start_transfer(struct vdma_channel* vc){
    enum dma_ctrl_flags flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	struct dma_async_tx_descriptor *chan_desc;
	struct dma_device *dma_device = vc->tx->device;
	int bdindex = vc->bdindex;

    sg_init_one(
        &vc->bdtable[bdindex].sglist,
        (void*)vc->bdtable[bdindex].dma_handle,
        vc->btable_p[bdindex].length
        );

	chan_desc = dma_device->device_prep_slave_sg(vc->tx, &vc->bdtable[bdindex].sglist, 1, 
						vc->direction, flags, NULL);

    if (!chan_desc) {
		printk(KERN_ERR "dmaengine_prep*() error\n");
	} else {
        chan_desc->callback = sync_callback;
		chan_desc->callback_param = &vc->bdtable[bdindex].cmp;

        init_completion(&vc->bdtable[bdindex].cmp);

        vc->bdtable[bdindex].tx_cookie = dmaengine_submit(chan_desc);
        if (dma_submit_error(vc->bdtable[bdindex].tx_cookie)) {
            printk(KERN_ERR "Submit error\n");
            return;
        }

        dma_async_issue_pending(vc->tx);
    }
}

static void 
wait_for_transfer(struct vdma_channel *vc){
    unsigned long timeout = msecs_to_jiffies(3000);
	enum dma_status status;
	int bdindex = vc->bdindex;

    vc->btable_p[bdindex].status = PROXY_BUSY;

    timeout = wait_for_completion_timeout(&vc->bdtable[bdindex].cmp, timeout);
	status = dma_async_is_tx_complete(vc->tx, vc->bdtable[bdindex].tx_cookie, NULL, NULL);

    if (timeout == 0)  {
        vc->btable_p[bdindex].status  = PROXY_TIMEOUT;
        printk(KERN_ERR "DMA timed out\n");
    } else if (status != DMA_COMPLETE) {
        vc->btable_p[bdindex].status = PROXY_ERROR;
        printk(KERN_ERR "DMA returned completion callback status of: %s\n",
                status == DMA_ERROR ? "error" : "in progress");
    } else
        vc->btable_p[bdindex].status = PROXY_NO_ERROR;
}

static int
test( struct platform_device *pdev ){
    struct vdma_overlay *vo;

    vo = (struct vdma_overlay*)dev_get_drvdata(&pdev->dev);
    if(IS_ERR_OR_NULL(vo)){
        dev_err(&pdev->dev, "Could not allocate for < struct vdma_overlay >");
        if(!vo){
            return -ENODATA;
        } else { 
            return PTR_ERR(vo);
        }
    }
    struct vdma_channel *vc= vo->chan;

    vc->btable_p[0].length = TEST_SIZE;
    vc->bdindex = 0;

    /* fill in buffer 0 for testing with white screen */
    for(int i=0; i< BUFFER_SIZE/sizeof(unsigned int); i++){
        vc->btable_p[0].buffer[i] = 0xFF;
    }

    start_transfer(vc);
    wait_for_transfer(vc);
    return 0;
}

static int 
vdma_channels_setup( struct platform_device *pdev ){
    struct vdma_overlay *vo;

    vo = (struct vdma_overlay*)dev_get_drvdata(&pdev->dev);
    if(IS_ERR_OR_NULL(vo)){
        dev_err(&pdev->dev, "Could not allocate for < struct vdma_overlay >");
        if(!vo){
            return -ENODATA;
        } else { 
            return PTR_ERR(vo);
        }
    }
    struct vdma_channel *vc= vo->chan;

    vc->btable_p = (struct channel_buffer *) 
        dmam_alloc_coherent(&pdev->dev, sizeof(struct channel_buffer) * vc->frm_cnt,
                &vc->bfr_phys_addr, GFP_KERNEL | GFP_DMA);
    if (IS_ERR_OR_NULL(vc->btable_p)) {
        dev_err(&pdev->dev, "DMA allocation error\n");
        if(!vc->btable_p) {
            return -ENOMEM;
        } else {
            return PTR_ERR(vc->btable_p);
        }
    }
    printk(KERN_INFO "Allocating memory, virtual address: %px physical address: %px\n",
            vc->btable_p, (void *)vc->bfr_phys_addr);

    for(int bd =0; bd < BUFFER_COUNT; bd++){
        vc->bdtable[bd].dma_handle = (dma_addr_t)(vc->bfr_phys_addr +
                (sizeof(struct channel_buffer)*bd) + offsetof(struct channel_buffer, buffer));
        printk(KERN_INFO "\t%d: %px\n",bd,(void*)vc->bdtable[bd].dma_handle);
    } 

    /*init so no garbage*/
    vc->bdindex = 0;
    vc->direction = DMA_MEM_TO_DEV;

    return 0;
};

    static int 
vdma_overlay_probe( struct platform_device *pdev )
{
    struct vdma_overlay *vo;
    struct vdma_channel *vchan;
    int rc;

    /* allocate struct vdma_overlay*/
    vo = (struct vdma_overlay *) devm_kmalloc(&pdev->dev, sizeof(struct vdma_overlay), GFP_KERNEL);
    if(IS_ERR_OR_NULL(vo)){
        dev_err(&pdev->dev, "Could not allocate for < struct vdma_overlay >");
        if(!vo){
            return -ENOMEM;
        } else { 
            return PTR_ERR(vo);
        }
    }

    /* allocate allocate vdma_channel*/
    vchan = (struct vdma_channel *) devm_kmalloc(&pdev->dev, sizeof(struct vdma_channel ), GFP_KERNEL);
    if(IS_ERR_OR_NULL(vchan)){
        dev_err(&pdev->dev, "Could not allocate for < struct vdma_channel >");
        if(!vchan){
            return -ENOMEM;
        } else { 
            return PTR_ERR(vchan);
        }
    }

    vo->chan = vchan; 

    /*instead of globals*/ 
    dev_set_drvdata(&pdev->dev,vo);

    rc = of_property_read_u32(pdev->dev.of_node,
            "xlnx,num-fstores", &vchan->frm_cnt);
    if (rc < 0) {
        printk(KERN_ERR "vdma_overlay: missing xlnx,num-fstores property\n");
        return rc;
    }

    vchan->tx = dma_request_chan(&pdev->dev, "vdma0");
    if (IS_ERR(vchan->tx)) {
        rc = PTR_ERR(vchan->tx);
        if (rc != -EPROBE_DEFER)
            pr_err("vdma_overlay: No Tx channel\n");
        return rc;
    }

    if (rc) {
        pr_err("vdma_overlay: Unable to add channels\n");
        dma_release_channel(vchan->tx);
        return rc;
    } 
    vdma_channels_setup(pdev);

    if (internal_test)
        test(pdev);
    return 0;
}

    static int 
vdma_overlay_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct vdma_overlay *lp = dev_get_drvdata(dev);

    printk(KERN_INFO "dma_proxy module exited\n");

    /* Take care of the DMA channels and any buffers allocated
     * for the DMA transfers. The DMA buffers are using managed
     * memory such that it's automatically done.
     */
    if (lp->chan->tx) {
        lp->chan->tx->device->device_terminate_all(lp->chan->tx);
        dma_release_channel(lp->chan->tx);
    }
    return 0;
}

static const struct of_device_id vdma_overlay_of_ids[]={
    {.compatible = "xlnx,vdma-overlay-1.0",},
    {}
};

static struct platform_driver vdma_connect_driver = {
    .probe      = vdma_overlay_probe,
    .remove     = vdma_overlay_remove,
    .driver     = {
        .name     = "vdma_overlay",
        .owner    = THIS_MODULE,
        .of_match_table = vdma_overlay_of_ids,
    },
};

// for driver that dont do anything special in module init/exit | eliminates alot of boilerplate
module_platform_driver(vdma_connect_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliwier Jaworski");
MODULE_DESCRIPTION("Simple GPIO driver to interface ZYNQ PS GPIO");
MODULE_VERSION("0.1");
