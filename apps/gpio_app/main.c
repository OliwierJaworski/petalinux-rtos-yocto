#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/types.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define XGPIO_DATA_OFFSET	0x0   /**< Data register for 1st channel */
#define XGPIO_TRI_OFFSET	0x4   /**< I/O direction reg for 1st channel */
#define XGPIO_DATA2_OFFSET	0x8   /**< Data register for 2nd channel */
#define XGPIO_TRI2_OFFSET	0xC   /**< I/O direction reg for 2nd channel */

#define dev_entry "/sys/class/uio/uio0/maps/map0/"
#define dev_parts(dev_info) \
    sizeof(dev_info) / sizeof(uint32_t)

const char *dev_attributes[] = {"offset","addr","size"};
struct dev_info_T{
  uint32_t offset;
  uint32_t addr;
  uint32_t size;
} __attribute__((packed, aligned(4))) gpio_device;


int
main(void) {
  int gpiofd;
  FILE *configfp; 
  int err;
  uint32_t *gpioMEM = NULL;

  gpiofd = open("/dev/uio0", O_RDWR);
  if(gpiofd < 0){
    perror("config open:");
    return errno;
  }
  printf("opened uio0 device\n"); 

  uint32_t *dev_attribute = (uint32_t*) &gpio_device ;
  for(int i=0; i < dev_parts(gpio_device); i++){
    char dev_info_path[sizeof(dev_entry)+7] = dev_entry; 
    strncpy(&dev_info_path[strlen(dev_entry)],dev_attributes[i],strlen(dev_attributes[i]));

    configfp = fopen(dev_info_path,"r");
    fscanf(configfp, "%x",&dev_attribute[i]);
    fclose(configfp);

    printf("attribute:%s:0x%x\n",dev_attributes[i],dev_attribute[i]);
  }

  gpioMEM = (uint32_t*) mmap(NULL,gpio_device.size,PROT_READ | PROT_WRITE, MAP_SHARED,gpiofd,0);
  if(gpioMEM == MAP_FAILED){
      perror("mmap failed:");
      return errno;
  }
  printf("mmap successful! GPIO memory mapped at %p\n", gpioMEM);
  
  uint16_t User_cmd=0;
  gpioMEM[1] = 0; //set to output
  while(User_cmd != 30){
      printf("what led should light up? choice:\n");
      scanf("%hu",&User_cmd);
      gpioMEM[0] = User_cmd; //set to output
  }
  munmap(gpioMEM, gpio_device.size);
  close(gpiofd);
}
