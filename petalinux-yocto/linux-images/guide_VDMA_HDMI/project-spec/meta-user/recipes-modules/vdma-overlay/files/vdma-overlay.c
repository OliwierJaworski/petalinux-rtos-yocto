/* vdma-overlay.c - VDMA Framebuffer Overlay Driver
 * Copyright (C) 2025 Oliwier Jaworski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/dma/xilinx_dma.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include "asm-generic/errno-base.h"
#include "linux/dma-direction.h"
#include <linux/platform_device.h>
#include <linux/of_dma.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliwier Jaworski");
MODULE_DESCRIPTION("VDMA framebuffer overlay driver");
MODULE_VERSION("0.01");

#define DRIVER_NAME "vdma-overlay"

#define FB_ADDR(vinfo) vinfo->fb_info.addr

#define VSIZE 1080
#define HSIZE 1920 
#define RGBA 4
#define STRIDE (HSIZE*RGBA)
#define BUFFER_SIZE (VSIZE* HSIZE* 4)	
static unsigned int frm_cnt;

//reserved memory/framebuffer info
struct rmem_fb_info{
  struct device_node *fb,*rmem;
  const __be32 *reg; //big endian 32 register
  u32 addr, size;
  int err;
};

struct vdmaOverlay_info{
  struct platform_device *pdev;
  struct dma_chan *tx;
  phys_addr_t src_addr;
  enum dma_transaction_type type; 
  enum dma_status status;
  struct xilinx_vdma_config cfg;
  dma_cookie_t tx_cookie;
  enum dma_ctrl_flags flags;
  dma_addr_t src[3];
  struct rmem_fb_info fb_info;
};


static struct dma_interleaved_template xt;

static int vdmaOverlay_configure(struct vdmaOverlay_info *vinfo){
  
  struct dma_device *tx_dev = vinfo->tx->device;
  struct dma_async_tx_descriptor *txd = NULL; 
  struct xilinx_vdma_config *cfg = &vinfo->cfg;

  // -> linux-xlnx/include/linux/dma
  cfg->gen_lock = 1;    
  cfg->master = 0;      
  cfg->frm_cnt_en = 0;
  cfg->park = 0;
  cfg->coalesc = 0;
  cfg->delay = 0;
  cfg->reset = 1;  
  xilinx_vdma_channel_set_config(vinfo->tx, cfg);
   
  for(int i=0;i<frm_cnt;i++){
    vinfo->src[i] = dma_map_resource(tx_dev->dev, vinfo->src_addr, (1920*1080*4), DMA_TO_DEVICE,0);
    if(dma_mapping_error(tx_dev->dev, vinfo->src[i])){
      pr_info("failed to map dma\n");
      continue;
    }
    xt.src_start = vinfo->src[i];
    xt.dir = DMA_MEM_TO_DEV;
    xt.numf = VSIZE;
    xt.sgl[0].size = STRIDE;
    xt.sgl[0].icg = 0;
    xt.frame_size = 1;
    txd = tx_dev->device_prep_interleaved_dma(vinfo->tx, &xt, vinfo->flags);
    if(IS_ERR(txd)){
      printk("Error: %ld, in device_prep_dma_cyclic", PTR_ERR(txd));
      return PTR_ERR(txd);
    } 

    //dmaengine_submit() will not start the DMA operation, it merely adds it to the pending queue.
    vinfo->tx_cookie = txd->tx_submit(txd);
  }
 
  if(!txd){
    for(int i=0;i<frm_cnt;i++){
      dma_unmap_resource(tx_dev->dev, vinfo->src[i], BUFFER_SIZE, DMA_TO_DEVICE, 0);
    }
    if(IS_ERR(txd)){
      printk("Error: %ld, in device_prep_dma_cyclic", PTR_ERR(txd));
      return PTR_ERR(txd);
    }
  } 

  dma_async_issue_pending(vinfo->tx);
  pr_info("oj_vdma_overlay configured\n");
  return 0;
}

//checks whether framebuffer is present in device tree and has been set up properly
static int rmem_fb_check(struct platform_device *pdev, struct rmem_fb_info * fbi){
  //check if reserved memory has been added to dev tree
  fbi->rmem = of_find_node_by_path("/reserved-memory");
  if(!fbi->rmem){
    dev_err(&pdev->dev, "Cannot find /reserved-memory node\n");
    return -ENODEV;
  }
   
  //find framebuffer node inside reserved memory
  fbi->fb = of_find_node_by_name(fbi->rmem, "framebuffer");
  of_node_put(fbi->fb);//release hold on parent -> prevent memleak
  if(!fbi->fb){
    dev_err(&pdev->dev, "Cannot find framebuffer node in /reserved-memory\n");
    of_node_put(fbi->rmem);
    return -ENODEV;
  } 
  
  fbi->reg = of_get_property(fbi->fb,"reg", NULL); 
  if(!fbi->reg){
    dev_err(&pdev->dev, "Cannot find framebuffer node in /reserved-memory\n");
    of_node_put(fbi->fb);
    of_node_put(fbi->rmem);
    return -EINVAL;
  }

  //reg holds 2 32bit values -> read 32, move 1 cell(32bits) and read 32
  fbi->addr = of_read_number(fbi->reg, 1);
  fbi->size = of_read_number(fbi->reg + 1, 1);

  //release hold
  of_node_put(fbi->fb);
  of_node_put(fbi->rmem);

  dev_info(&pdev->dev, "Found framebuffer at 0x%x, size = 0x%x\n", fbi->addr, fbi->size);
  return 0;
}

static int vdmaOverlay_probe(struct platform_device *pdev){
  dev_info(&pdev->dev, "OJ -  VDMA overlay Driver Probed!!\n");

  int err,r;

  struct vdmaOverlay_info *vinfo = kmalloc(sizeof(struct vdmaOverlay_info), GFP_KERNEL);
  if (!vinfo)
    return -ENOMEM;

  //default all settings to 0
  memset(&vinfo->cfg, 0, sizeof(struct xilinx_vdma_config));

  //function checks whether framebuffer is present
  r = rmem_fb_check(pdev, &vinfo->fb_info);
  if(r != 0){
    kfree(vinfo);
    return r;
  }

  // enough space for 3 frames?
  if(vinfo->fb_info.size < (FB_ADDR(vinfo)*frm_cnt)){

    pr_err("oj-vdmaoverlay: not enough space on framebuffer node for %d vdma frames\n",frm_cnt); 
    kfree(vinfo);
    return -ENOSPC;
  } 

  vinfo->pdev = pdev;
  vinfo->src_addr = (phys_addr_t) FB_ADDR(vinfo);
  vinfo->flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;

  // search whether it has given property
  err = of_property_read_u32(pdev->dev.of_node, "xlnx,num-fstores", &frm_cnt);

  if (err < 0){
    pr_err("oj-vdmaoverlay: missing xlnx,num-fstores property\n"); 
    kfree(vinfo);
    return err;
  }
  dev_info(&pdev->dev, "oj-vdmaoverlay: xlnx,num-fstores:%d\n",frm_cnt);
  
  vinfo->tx= dma_request_chan(&pdev->dev, "vdma0");
  if(IS_ERR(vinfo->tx)){
    err = PTR_ERR(vinfo->tx);
    if(err != -EPROBE_DEFER){
      kfree(vinfo);
      pr_err("oj-vdmaoverlay: No Tx channel\n");
    }
    return err;
  }
  
  err = vdmaOverlay_configure(vinfo);
  if(err){// free dma resource
    kfree(vinfo);
    pr_err("oj-vdmaoverlay: failed to add channels\n");
    goto free_tx;
  }
  platform_set_drvdata(pdev, vinfo);
  return 0;

  free_tx:
    dma_release_channel(vinfo->tx);
  
  return err;
}

static int vdmaOverlay_remove(struct platform_device *pdev){
  dev_info(&pdev->dev, "OJ -  VDMA overlay Driver Removed!!\n");
  
  //struct dma_chan *tx;
  struct vdmaOverlay_info *vinfo = platform_get_drvdata(pdev);
  if (!vinfo)//rip in this situation will have to restart board to get access to dma channel again
      return 0;
  
  //must be called before so termination can be done succesfully
  dmaengine_synchronize(vinfo->tx);
  dmaengine_terminate_async(vinfo->tx);

  for(int i=0;i<frm_cnt;i++)
    dma_unmap_resource(vinfo->tx->device->dev, vinfo->src[i], BUFFER_SIZE , DMA_TO_DEVICE, 0);
  //release exclusive access to dma channel
  dma_release_channel(vinfo->tx);
  kfree(vinfo);

  return 0;
}

static const struct of_device_id vdma_overlay_of_match[] = {
    { .compatible = "xlnx,vdma-overlay-1.0", },
    { /* end of list */ },
};
MODULE_DEVICE_TABLE(of, vdma_overlay_of_match);

static struct platform_driver vdma_overlay_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = vdma_overlay_of_match,
    },
    .probe = vdmaOverlay_probe,
    .remove = vdmaOverlay_remove,
};

static int __init vdma_overlay_init(void)
{
    return platform_driver_register(&vdma_overlay_driver);
}

static void __exit vdma_overlay_exit(void)
{
    platform_driver_unregister(&vdma_overlay_driver);
}

module_init(vdma_overlay_init);
module_exit(vdma_overlay_exit);

