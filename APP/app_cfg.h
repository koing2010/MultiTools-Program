#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


/*******************设置任务优先级*******************/
#define STARTUP_TASK_PRIO       4  
#define	TASK_INFRARED_PRIO			6
#define	TASK_USART1CL_PRIO			5
#define TASK_WIRELESS_PRIO      7
#define TASK_USART3CL_PRIO      8
#define TASK_CANRxCL_PRIO       9      

/************设置栈大小（单位为 OS_STK ）************/
#define STARTUP_TASK_STK_SIZE   80   
#define	TASK_INFRARED_STK_SIZE  80
#define	TASK_USART1CL_STK_SIZE	80
#define TASK_USART3CL_STK_SIZE  80
#define TASK_CANRxCL_STK_SIZE   80
#define REMOTE_SCAN_STK_SIZE    80




#endif

