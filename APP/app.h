#ifndef	_APP_H_
#define	_APP_H_

/**************** �û��������� *******************/
void Task_Start(void *p_arg);
void Task_USART1_CL(void *p_arg);
void Task_Infrared(void *p_arg);
void Task_WirelessRx(void *p_arg);
void Task_USART3_CL(void *p_arg);
void Task_CANRx_CL(void *p_arg);
#endif	//_APP_H_
