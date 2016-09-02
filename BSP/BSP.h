#ifndef __BSP_H
#define	__BSP_H
#include "stm32f10x.h"
#include "controllayer.h"
typedef struct  PERIPHRAL
{
    void (*GPIO_Config)(void);
    void (*DMA_Config)(void);
    void (*UARST_Config)(void);
    void (*TIM1_config)(void);
    void (*TIM1_OC3config)(void);
    void (*TIM2_UpdateCount_Config)(void);
    void (*Interrupts_Config)(void);
    void (*JF24D_IO_Config)(void);

} PERIPHRAL_CONFIG;


#define CAN_LED_Pin       GPIO_Pin_12
#define CAN_LED_Port      GPIOB

#define RS485_LED_Pin     GPIO_Pin_13
#define RS485_LED_Port    GPIOB

#define RS485_PWR_Pin     GPIO_Pin_0
#define RS485_PWR_Port    GPIOA

#define RS485_DE_Pin      GPIO_Pin_2
#define RS485_DE_Port     GPIOA

#define  RS485_PWR_ENABLE   GPIO_SetBits(RS485_PWR_Port,RS485_PWR_Pin);
#define  RS485_PWR_DISABLE  GPIO_ResetBits(RS485_PWR_Port,RS485_PWR_Pin);

#define  RS485_Tx_EN       GPIO_SetBits(RS485_DE_Port , RS485_DE_Pin);
#define  RS485_Rx_EN       GPIO_ResetBits(RS485_DE_Port , RS485_DE_Pin);

#define  CAN_LED_OPEN      GPIO_ResetBits(CAN_LED_Port, CAN_LED_Pin);
#define  CAN_LED_CLOSE     GPIO_SetBits(CAN_LED_Port, CAN_LED_Pin);

#define  RS485_LED_OPEN    GPIO_ResetBits(RS485_LED_Port, RS485_LED_Pin);
#define  RS485_LED_CLOSE   GPIO_SetBits(RS485_LED_Port, RS485_LED_Pin);

#define  WIRELESS_LED_OPEN  RS485_LED_OPEN;RS485_LED_OPEN
#define  WIRELESS_LED_CLOSE  RS485_LED_CLOSE;CAN_LED_CLOSE

/* this function is used for configurating can port */
void Can_Config(u8 CAN_Speed, u16 Can_Addr);

void UARST3_Config(PUSART_CFG usart_cfg);
void SysTick_init(void);
void BSP_Init(void);
#endif // __BSP_H
