#include "system_core.h"

int main()
{
	LOG_UART(LOG_TRACE, "-- BEGIN MAIN--", NULL);
	struct SYSHandle_t sys;
	memset(&sys, 0,sizeof(struct SYSHandle_t)); // prevent garbage values -> easier debug

	sys.tcpServer.xClientsemphrCounting = xSemaphoreCreateCounting(TCP_MAX_CLIENTS,TCP_MAX_CLIENTS);	
	sys.sd.xSDsemphr = xSemaphoreCreateBinary();

	sys.sd.volume = SD_DEFAULT_VOLUME;

	sys.graphics.frame_buffers[0] = FRAME_BUFFER_PTR1;
	sys.graphics.frame_buffers[1] = FRAME_BUFFER_PTR2;
	sys.graphics.frame_buffers[2] = FRAME_BUFFER_PTR3;
	
	prvSetupHardware(&sys);
	sys_thread_new("main_thrd", (void (*)(void *))main_thread, (void*)&sys,
				   TCP_THREAD_STACKSIZE,
				   DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while (1);
		
	return 0;
}
