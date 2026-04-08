#include "linux/dma-direction.h"
#include "linux/dma-mapping.h"
#include "linux/dmaengine.h"
#include "linux/err.h"
#include "linux/of.h"
#include <linux/dma/xilinx_dma.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_dma.h>
#include <linux/io.h>

static unsigned int frm_cnt;

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
};

struct vdmaoverlay_chan {
	struct list_head node;
	struct dma_chan *chan;
	struct list_head threads;
};

static struct dma_interleaved_template xt;

static int vdmaOverlay_configure(struct vdmaOverlay_info *vinfo){
  set_user_nice(current, 10); 
  
  struct dma_device *tx_dev = vinfo->tx->device;
  struct dma_async_tx_descriptor *txd = NULL; 
  struct xilinx_vdma_config *cfg = &vinfo->cfg;
/**
 * struct xilinx_vdma_config - VDMA Configuration structure
 * @frm_dly: Frame delay
 * @gen_lock: Whether in gen-lock mode
 * @master: Master that it syncs to
 * @frm_cnt_en: Enable frame count enable
 * @park: Whether wants to park
 * @park_frm: Frame to park on
 * @coalesc: Interrupt coalescing threshold
 * @delay: Delay counter
 * @reset: Reset Channel
 * @ext_fsync: External Frame Sync source
 * @vflip_en:  Vertical Flip enable
 */
/*
  struct xilinx_vdma_config {
	int frm_dly;
	int gen_lock;
	int master;
	int frm_cnt_en;
	int park;
	int park_frm;
	int coalesc;
	int delay;
	int reset;
	int ext_fsync;
	bool vflip_en;
};*/
  cfg->gen_lock = 0;    
  cfg->master = 0;      
  cfg->frm_cnt_en = 0;
  cfg->park = 0;
  cfg->coalesc = 0;
  cfg->delay = 0;
  cfg->reset = 1;  
  xilinx_vdma_channel_set_config(vinfo->tx, cfg);
   
  for(int i=0;i<frm_cnt;i++){
    pr_info("running setup loop it:%d\n",i);
    vinfo->src[i] = dma_map_resource(tx_dev->dev, vinfo->src_addr, (1920*1080*4), DMA_TO_DEVICE,0);
    if(dma_mapping_error(tx_dev->dev, vinfo->src[i])){
      pr_info("failed to map dma\n");
      continue;
    }
    xt.src_start = vinfo->src[i];
    xt.dir = DMA_MEM_TO_DEV;
    xt.numf = 1080;
    xt.sgl[0].size = (1920*4);
    xt.sgl[0].icg = 0;
    xt.frame_size = 1;
    txd = tx_dev->device_prep_interleaved_dma(vinfo->tx, &xt, vinfo->flags);
    //txd = tx_dev->device_prep_dma_cyclic(vinfo->tx, vinfo->src[i], (7680*1080), (7680*1080),DMA_MEM_TO_DEV,vinfo->flags); 
    if(IS_ERR(txd)){
      dma_release_channel(vinfo->tx);
      printk("Error: %ld, in device_prep_dma_cyclic", PTR_ERR(txd));
      return PTR_ERR(txd);
    } 

    //dmaengine_submit() will not start the DMA operation, it merely adds it to the pending queue.
    //vinfo->tx_cookie = txd->tx_submit(txd);
    vinfo->tx_cookie = txd->tx_submit(txd);
  }
 
  if(!txd){
    for(int i=0;i<frm_cnt;i++){
      dma_unmap_resource(tx_dev->dev, vinfo->src[i], 1920*1080*4, DMA_TO_DEVICE, 0);
    }
    pr_warn("error during prep?\n");
  } 

  dma_async_issue_pending(vinfo->tx);
  pr_info("oj_vdma_overlay configured\n");
  return 0;
}

static int vdmaOverlay_probe(struct platform_device *pdev){
  dev_info(&pdev->dev, "OJ -  VDMA overlay Driver Probed!!\n");

  int err;
  struct vdmaOverlay_info *vinfo = kmalloc(sizeof(struct vdmaOverlay_info), GFP_KERNEL);
  if (!vinfo)
    return -ENOMEM;
  memset(&vinfo->cfg, 0, sizeof(struct xilinx_vdma_config));
  vinfo->pdev = pdev;
  vinfo->src_addr = (phys_addr_t)0x10000000;
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
  
  //here will go code configuring everything
  
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
    if (!vinfo)
        return 0;
  
  //must be called before so termination can be done succesfully
  dmaengine_synchronize(vinfo->tx);
  dmaengine_terminate_async(vinfo->tx);
  for(int i=0;i<frm_cnt;i++)
    dma_unmap_resource(vinfo->tx->device->dev, vinfo->src[i], 1920*1080*4, DMA_TO_DEVICE, 0);
  //release exclusive access to dma channel
  dma_release_channel(vinfo->tx);
  kfree(vinfo);

  return 0;
}

static const struct of_device_id oj_vdmaoverlay_of_ids[] = {
  { .compatible = "xlnx,vdma-overlay-1.0",},
  {}
};

static struct platform_driver oj_vdmaoverlay_driver = {
  .driver = {
    .name = "oj_vdma_overlay",
    .owner = THIS_MODULE,
    .of_match_table = oj_vdmaoverlay_of_ids,
  },
  .probe =vdmaOverlay_probe, 
  .remove =vdmaOverlay_remove, 
};

module_platform_driver(oj_vdmaoverlay_driver)

MODULE_AUTHOR("Oliwier Jaworski");
MODULE_DESCRIPTION("simple vdma-framebuffer overlay");
MODULE_VERSION("0.01");
MODULE_LICENSE("GPL v2");


