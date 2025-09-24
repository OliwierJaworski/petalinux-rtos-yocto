// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2010-2014 Xilinx, Inc. All rights reserved.
 * Copyright (C) 2025 Oliwier Jaworski. Modifications under GPL-2.0-or-later
 */


#include <linux/bitops.h>
#include <linux/dmapool.h>
#include <linux/dma/xilinx_dma.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_dma.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/io-64-nonatomic-lo-hi.h>

#include "../dmaengine.h"
#include "linux/dmaengine.h"
#include "linux/printk.h"

static u32 v_frm_b; // how many framebuffers vdma has
struct xilinx_vdmatest_chan {
	struct list_head node;
	struct dma_chan *chan;
	struct list_head threads;
};                    
static int vdma_alloc_tx_channels(struct dma_chan *tx_chan)
{
  unsigned int thread_count =0;
  return 0;
}

static int vdma_overlay_probe(struct platform_device *pdev)
{  
  int err;
  struct dma_chan *tx_chan;
  err = of_property_read_u32(pdev->dev.of_node,"xlnx,num-fstores",&v_frm_b); 
  if( err < 0 ){
    pr_err("vdma_overlay: missing xlnx,num-fstores property\n");
    return err;
  }
  pr_info("vdma_overlay: initialized driver succesfully\n");

  tx_chan = dma_request_chan(&pdev->dev, "vdma0");
  if(IS_ERR(tx_chan)){
        err = PTR_ERR(tx_chan);
	    if (err != -EPROBE_DEFER)
			pr_err("vdma_overlay: No Tx channel\n");
		return err;
  }
  vdma_alloc_tx_channels(tx_chan);

  return 0;
}

static int vdma_overlay_remove(struct platform_device *pdev)
{  
  pr_info("vdma_overlay: remove cb called\n");
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


