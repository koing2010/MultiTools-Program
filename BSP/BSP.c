#include "includes.h"
#include "BSP.h"
#include "JF24D.h"
static void GPIO_Config(void);
static void UARST_Config(void);
static void DMA_Config(void);
static void TIM1_Config(void);
static void TIM1_OC3_Config(void);
static void TIM2_UpdateCount_Config(void);
static void Interrupts_Config(void);
void EXTI_Configuration(void);

PERIPHRAL_CONFIG Periphral_Config=
{
    GPIO_Config,
    DMA_Config,
    UARST_Config,
    TIM1_Config,
    TIM1_OC3_Config,
    TIM2_UpdateCount_Config,
    Interrupts_Config,
    JF24D_IO_Config,
};
/*
 * 函数名：BSP_Init
 * 描述  ：时钟初始化、硬件初始化
 * 输入  ：无
 * 输出  ：无
 */
void BSP_Init(void)
{
    SystemInit();		/* 配置系统时钟为72M */
    SysTick_init();		/* 初始化并使能SysTick定时器 */
    Periphral_Config.Interrupts_Config();
    Periphral_Config.GPIO_Config();
    Periphral_Config.DMA_Config();
    Periphral_Config.UARST_Config();
    Periphral_Config.JF24D_IO_Config();
    EXTI_Configuration();
}

/*
 * 函数名：SysTick_init
 * 描述  ：配置SysTick定时器
 * 输入  ：无
 * 输出  ：无
 */
void SysTick_init(void)
{
    SysTick_Config(SystemFrequency/OS_TICKS_PER_SEC);//初始化并使能SysTick定时器
}


static void Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  //配置优先级组：优先级和子优先级

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
  
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
//    /* Configure TIM4 update interrupt */
//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
    /* Enable and set EXTI1 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*使能can接收数据中断*/
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
void EXTI_Configuration(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;

    /* Connect EXTI8 Line to PB.01 pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

    /* JF24D PKFLAG Configure EXTI1 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Clears  EXTI line pending bits. */
    EXTI_ClearITPendingBit(EXTI_Line1);
}
/*
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOA  
	                |RCC_APB2Periph_TIM1| RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART3 Rx (PB.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure USART3 Tx (PB.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure can indicator led (PB.12) as out push-pull */
    GPIO_InitStructure.GPIO_Pin = CAN_LED_Pin ;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_LED_Port, &GPIO_InitStructure);

    /* Configure 485 indicator led (PB.13) as out push-pull */
    GPIO_InitStructure.GPIO_Pin = RS485_LED_Pin ;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_LED_Port, &GPIO_InitStructure);

    /* UART3 485-DE,decide TxRx  (PA.02) as out push-pull */
    GPIO_InitStructure.GPIO_Pin = RS485_DE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_DE_Port, &GPIO_InitStructure);

    /* Configure 485 PWR (PA.00) as out push-pull */
    GPIO_InitStructure.GPIO_Pin = RS485_PWR_Pin ;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_PWR_Port, &GPIO_InitStructure);

    /* Configure CANRX (PA.11) as out Pull up the input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CANTX (PB.12) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

static void UARST_Config(void)
{
    /* USART1 default configuration */
    /* USART1 configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - Parity No
          - Hardware flow control desabled
          - Receive and transmit enabled
    */
    USART_InitTypeDef   USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No  ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable DMA Tx function */
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);

    /* Enable DMA Rx function */
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

    /* Enable the USART function */
    USART_Cmd(USART1, ENABLE);

    /* Enable the USART Receive interrupt */
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable the USART Idle interrupt */
    USART_ITConfig(USART1, USART_IT_IDLE , ENABLE);
}

/*******************************************************************************
Function:  UARST3_Config
Description:  used for usb->485 function config
Input:        PUSART_CFG usart_cfg : the point of USART config parameter
Output:       None
Return:       None
Others:	   You can add what you need in.
*******************************************************************************/
void UARST3_Config(PUSART_CFG usart_cfg)
{
    /* USART3 default configuration */
    /* USART3 default configured as follow:
          - BaudRate = 9600 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - Parity No
          - Hardware flow control desabled
          - Receive and transmit enabled
    */
    USART_InitTypeDef   USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);
    if(usart_cfg->BaudRate == 0)
    {
        /*default Baurate 9600 bps*/
        usart_cfg->BaudRate = 96;
    }
    USART_InitStructure.USART_BaudRate =  (u32)usart_cfg->BaudRate * 100;
    USART_InitStructure.USART_WordLength = (u16)usart_cfg->Word_Length << 12;
    USART_InitStructure.USART_StopBits = (u16)usart_cfg->Stop_Bit << 12;;
    USART_InitStructure.USART_Parity = (u16)usart_cfg->Parity << 8;  ;
    USART_InitStructure.USART_HardwareFlowControl = (u16)usart_cfg->HardwareFlowControl << 8;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USART3 */
    USART_Init(USART3, &USART_InitStructure);

    /* Enable DMA Tx function */
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);

    /* Enable DMA Rx function */
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);

    /* Enable the USART function */
    USART_Cmd(USART3, ENABLE);

    /* Enable the USART Receive interrupt */
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable the USART Idle interrupt */
    USART_ITConfig(USART3, USART_IT_IDLE , ENABLE);

    /* enable DMA1 CH2&CH3 */
}

#include "object.h"
static void DMA_Config()
{
    extern  USART usart1;
    extern  USART usart3;

    DMA_InitTypeDef DMA_InitStructure;


    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE);

    /* USART1 Tx DMA1 channel4 config */
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&usart1.usartTxbuf[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //data source
    DMA_InitStructure.DMA_BufferSize = 1;// number of data transmited
    //DMA传输完数据后外设基地址不增加
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //DMA传输完数据后存储器基地址增加以便把下一次转换结果传递
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设的数据长度为8位
    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_MemoryDataSize_Byte;
    //存储器数据长度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //正常模式时DMA只进	//行一次数据传输
    DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal;
    //定义DMA的优先级
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //不使用存储器到存储器传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    //配置完成后，启动DMA通道
    DMA_Cmd(DMA1_Channel4, ENABLE);


    /* USART3 Tx DMA1 channel2 config,enable */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&usart3.usartTxbuf[0];
    DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE); 

    /* USART1 Rx DMA1 channel5 config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & USART1->DR;
    //配置DMA通道的存储器基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&usart1.usartRxbuf[0];
    //循环传输
    DMA_InitStructure.DMA_Mode =   DMA_Mode_Normal;
    //外设是DMA通道的数据源
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = sizeof(usart1.usartRxbuf);
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    //配置完成后，启动DMA通道
    DMA_Cmd(DMA1_Channel5, ENABLE);


    /* USART3 Rx DMA1 channel3 config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & USART3->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&usart3.usartRxbuf[0];
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void Can_Config(u8 CAN_Speed, u16 Can_Addr)	 //	   此子函数用来配置can
{
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE);

    //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE;						 //非时间触发通信模式  //
//		CAN_InitStructure.CAN_ABOM=DISABLE;						 //软件自动离线管理	 //
    CAN_InitStructure.CAN_ABOM = ENABLE;					//3.22can自动离线管理使能
    CAN_InitStructure.CAN_AWUM = DISABLE;	//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)//
//		CAN_InitStructure.CAN_NART=ENABLE;						 	//禁止报文自动传送 //
    CAN_InitStructure.CAN_NART = DISABLE;
//	CAN_InitStructure.CAN_RFLM=DISABLE;						 //报文不锁定,新的覆盖旧的 //
    CAN_InitStructure.CAN_RFLM = ENABLE; //使能fifo锁定模式
    CAN_InitStructure.CAN_TXFP = DISABLE;						//优先级由报文标识符决定 //
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//模式设置： mode:0,普通模式;1,回环模式; //
    //设置波特率
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
//  	CAN_InitStructure.CAN_Prescaler=brp;            //分频系数(Fdiv)为brp+1	//
    switch (CAN_Speed)
    {
    case 20:
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
        CAN_InitStructure.CAN_Prescaler = 120;
    }
    break;
    case 50:
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
        CAN_InitStructure.CAN_Prescaler = 48;
    }
    break;
    case 100:
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
        CAN_InitStructure.CAN_Prescaler = 24;
    }
    break;
    case 125:
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
        CAN_InitStructure.CAN_Prescaler = 18;
    }
    break;
    case 250:
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
        CAN_InitStructure.CAN_Prescaler = 9;
    }
    break;
    default:
        break;
    }
    CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1

    CAN_FilterInitStructure.CAN_FilterNumber = 0;	  //过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit; //16位
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;//Can_Addr << 5; ////16位ID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; //16位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; //32位
//	CAN_FilterInitStructure.CAN_FilterIdHigh   = (((u32)Can_Addr<<21)&0xffff0000)>>16;
//	CAN_FilterInitStructure.CAN_FilterIdLow   = (((u32)Can_Addr<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0xFFFF;
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; //过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器0

    CAN_FilterInit(&CAN_FilterInitStructure); //滤波器初始化

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); //FIFO0消息挂号中断允许.
}

/*定时器1 配置*/
static void TIM1_Config(void)
{
    TIM_TimeBaseInitTypeDef timbaseInitStruct;
    TIM_DeInit(TIM1);
    /*定时器时间基数初始化*/
    timbaseInitStruct.TIM_ClockDivision = 0;//TIM_CKD_DIV1;	 // 定时器输入采样
    timbaseInitStruct.TIM_Prescaler = 8;					   // 计数频率为8MHZ,  0.125us
    timbaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    timbaseInitStruct.TIM_Period = 210;  //0xA000;// 设定周期为26.3157us，对应频率为38KHZ.
    TIM_TimeBaseInit(TIM1, &timbaseInitStruct);
    //TIM_InternalClockConfig(TIM1);
}

/*******************************************************************************
Function:  TIM4_SlaveMode_Reset
Description:   input caputure ,rising edge reset count, 1khz timer counter
Input:        None
Output:       None
Return:       None
Others:	   You can add what you need in.
*******************************************************************************/
void TIM4_SlaveMode_Reset(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    /* TIM4 configuration ----------------------------------------------------*/
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 10 - 1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* Output Compare  Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 20;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);

    /* TIM4 Channel 2 Input Capture Configuration */
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    /* TIM4 Input trigger configuration: External Trigger connected to TI1 */
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1);

    /* TIM4 configuration in slave reset mode  where the timer counter is
       re-initialied in response to rising edges on an input capture (TI1) */
    TIM_SelectSlaveMode(TIM4,  TIM_SlaveMode_Reset);

    /* TIM4 IT CC1 enable */
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
}

/*定时器1 输出通道3配置*/
static void  TIM1_OC3_Config(void)
{
    TIM_OCInitTypeDef TIM_OCStructure;
    TIM_OCStructure.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStructure.TIM_Pulse = 140;
    TIM_OCStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Enable;

    TIM_OC3Init(TIM1,&TIM_OCStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1,ENABLE);//使能或关闭 ARR寄存器上的预载入寄存器
//  TIM_GenerateEvent(TIM1,TIM_EventSource_Update);
    TIM_Cmd(TIM1,ENABLE);  	 //使能定时器
//    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/********************************************************************************************
*函数名称：TIM2_SendConfiguration
*入口参数：无
*出口参数：无
*功能说明: 用于遥控码的发送
*******************************************************************************************/
static void TIM2_UpdateCount_Config(void)
{
    TIM_TimeBaseInitTypeDef timInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    TIM_DeInit(TIM2);
    timInitStruct.TIM_ClockDivision =0;//TIM_CKD_DIV1;	 // 定时器基准频率8MHz
    timInitStruct.TIM_Prescaler = 71;//8;					   // 计数频率为8MHz  1us
    timInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    timInitStruct.TIM_Period = 1199;// 这个值实际上就是TIMX->ARR，延时开始时重新设定即可15us
    TIM_TimeBaseInit(TIM2, &timInitStruct);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 计数溢出时触发中断
    TIM_Cmd(TIM2, ENABLE);


}
