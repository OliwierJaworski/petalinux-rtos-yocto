#include "system.h" /* */

int main()
{
	LOG_UART(LOG_TRACE, "-- BEGIN MAIN--", NULL);

	prvSetupHardware();

	sys_thread_new("main_thrd", (void (*)(void *))main_thread, 0,
				   THREAD_STACKSIZE,
				   DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while (1)
		;
	return 0;
}
