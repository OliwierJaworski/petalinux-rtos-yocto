## Problems & solutions
### Error adding lwIP library to platform project (RTOS)
To use lwIP with the EthernetLite core, configure lwIP to use `SOCKET_API` instead of the default `RAW_API`.  
EthernetLite does not support the RAW API.

### **platform.h** not found
replace the whole paltform initialization by changing:
``` c
//#include "platform.h"
#include "xil_io.h"
#include "xil_cache.h"
// comment out rest of paltform initialization
```