#include "system.h" /* */

int main()
{
	sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
	                THREAD_STACKSIZE,
	                DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while(1);
	return 0;
}


