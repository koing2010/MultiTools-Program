/**********************************************************************************

File_name:       JF24D.c
Version:		   0.0
Revised:        $Date:2014-12-28  to  2014-12-29 ; $
Description:    bottom driver of JF24D
Notes:          This version targets the stm32
Editor:		    Mr.Kong

**********************************************************************************/


/*================================================================================
@ Include files
*/
#include "JF24D.h"
#include "WirelessRxTx.h"
#include "stm32f10x.h"


/*================================================================================
@ Global variable
*/
#define SIMPLE

#if(JF24D_VSERSION04)

#ifndef SIMPLE
//新版JF24D
//寄存器组1  0-13号寄存器的初始值
const u32 RegArrFSKAnalog[]= {
    0xE2014B40,//Reg0
    0x00004BC0,//Reg1
    0x028CFCD0,//Reg2
    0x21390099,//Reg3
    0x1B8296D9,//Reg4 1B8296D9?????1Mbps?2Mbps  1B8AB6D9??????250Kbps
    0xA67F0224,//Reg5 A67F0224(??RSSI) A67F0624(??RSSI)
    0x00400000,//Reg6
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00127300,	//Reg12
    0x46B48000, //Reg13
};
//??JF24D
//寄存器组1
const u8 RegArrFSKAnalogReg14[]=
{
    0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};
#endif
/**************************0组寄存器初始值********************/
const u8  RegArrFSK[][2]= {
    {0,0x0F},
    {1,0x00},
    {2,0x3F},
    {3,0x03},
    {4,0xff},
    {5,0x17},
    {6,0x07},
    {7,0x07},
    {8,0x00},
    {9,0x00},
    {12,0xc3},
    {13,0xc4},
    {14,0xc5},
    {15,0xc6},
    {17,0x20},
    {18,0x20},
    {19,0x20},
    {20,0x20},
    {21,0x20},
    {22,0x20},
    {23,0x00},
    {28,0x3F},
    {29,0x07}
};
#else
//老版的无线模块 无线模块背面标注 VERSIN 03B
//寄存器组1  0-13号寄存器的初始值
const u32 RegArrFSKAnalog[]= {
    0xE2014B40,
    0x00004B40,
    0x028CFCD0,
    0x41390099,
    0x0B869EC1,
    0xA67F0224,
    0x00400000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x001A7300,
    0x36B48000,
};

//寄存器组1  14号寄存器的初始值
const u8 RegArrFSKAnalogReg14[]=
{
    0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};

//寄存器组0初始值
const u8 RegArrFSK[][2]= {
    {0,0x0F},		 //配置寄存器
    {1,0x3F},
    {2,0x3F},		 //使能接收地址
    {3,0x03},		 //信道的数据长度
    {4,0xff},
    {5,0x17},        //频道选择寄存器
    {6,0x17},
    {7,0x07},	     //状态寄存器
    {8,0x00},		 //射频设置寄存器
    {9,0x00},
    {12,0xc3},
    {13,0xc4},
    {14,0xc5},
    {15,0xc6},
    {17,0x20},
    {18,0x20},
    {19,0x20},
    {20,0x20},
    {21,0x20},
    {22,0x20},
    {23,0x00},
    {28,0x3F},
    {29,0x07}
};
#endif

u8 RX0_Address[5]= {0x12,0x34,0x56,0x78,0x01};
u8 RXFlag = FALSE;
u8 JTxBuf[40];
u8 JRxBuf[40];
u8 JTxCount,JRxCount;
u8 frequency;
u8 wireless_receive = 0;
/*================================================================================
@ All functions  as follow
*/
//static void SimpleDelayMs(u16 N_Ms );
extern u16 ModBus_CRC16(u8 *puchMsg,u8 crc_count);
void WriteTxAddress(u8 address);

/*********************************************************************************
Function:    JF24D_IO_Config
Description:
MCU           JF24D               TYPE
PB1             IRQ                I
PA4             CSN
PA7             MOSI
PA5             SCK
PA3             CE
PA6             MISO               I

Input:        None
Output:       None
Return:       local address
Others:	      don't care
*********************************************************************************/
void JF24D_IO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    /* Configure USB pull-up pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /*使能GPIOB,GPIOD,复用功能时钟*/   /*使能SPI1时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_AFIO, ENABLE);


    /*配SPI的 SCK,MISO,MOSI引脚，PA5,PA6,PA7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能
    GPIO_Init(GPIOA, &GPIO_InitStructure);




    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //双线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//数据大小8位
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//时钟极性，空闲时为高
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge ;						//第1个边沿有效，上升沿为采样时刻
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS信号由软件产生
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4 ;//SPI_BaudRatePrescaler_8;  //8分频，9MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI2  */
    SPI_Cmd(SPI1, ENABLE);
}

u8 SPI_RW(u8 data)
{
    /* 当 SPI发送缓冲器非空时等待 */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* 通过 SPI2发送一字节数据 */
    SPI_I2S_SendData(SPI1, data);

    /* 当SPI接收缓冲器为空时等待 */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return +
    6the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}
/*********************************************************************************
Function:     SimpleDelayMs
Description:  simple delay function.

Input:        the number of what you want to delay.
Output:       None
Return:       local address
Others:	      don't care
*********************************************************************************/
void SimpleDelayMs(u16 N_Ms )
{
    u16 circle = 1000;
    u8 i = 9;
    do {

        while(circle)
        {
            while( i-- );
            i = 9;
            circle --;
        }

        circle = 1000;
    } while( N_Ms--);
}
void Delayus(u8 nus)
{
    u8 i = 15;
    while(nus)
    {
        while( i -- );
        i = 15;
        nus --;
    }
}

/*********************************************************************************
Function:     Change_WirelessChannel
Description:  Change JF24D Wireless TxRx channel.

Input:        channel 0~254
Output:       None
Return:       local address
Others:	      don't care
*********************************************************************************/
void Change_WirelessChannel(u8  channel)
{

    channel = channel%127;
    SPI_Write_Reg((JF24WRITE_REG|5),channel);

    //SPI_Write_Reg(RF_CH,18);
    //frequency = SPI_Read_Reg(RF_CH);

}
/***********************************************
*函数名：InitWirelessModule
*作用：  初始化jf24d
*************************************************/
void Wireless_Init(void)
{

    SimpleDelayMs(8);
    JF24D_Init();		//初始化JF24D
    SimpleDelayMs(4);
    SimpleDelayMs(4);

    EXIT_TX_RX;		//进入RX
    Enter_PRX();		//切换到PRX模式
    ENTER_TX_RX;		//进入RX

}

/***********************************************
*函数名：IsInitWirelessModuleSuccess(void)
*作用：  利用从寄存器读出值判断初始化是否成功，
*返回值：返回0表示初始化不成功。
*************************************************/
//如果从寄存器29读出值为0x07即与写入的值相同则说明初始化成功，
//如果读出值为0则不成功
u8 IsInitWirelessModuleSuccess(void)
{
    u8 rdata;
    rdata=SPI_Read_Reg(29); 		//读寄存器29
    if(rdata==0x07)
    {
        return 1;	   //初始化成功
    }
    else
    {
        return 0;	   //初始化失败
    }
}


/**************************************************
函数名称：接收数据及处理函数
函数描述：
入口参数：
出口参数：
*************************************************/
void Wireless_RXData(void)
{
    u8 val;
    val = SPI_Read_Reg(STATUS);		//读STATUS寄存器
    JRxCount = Receive_Packet(&JRxBuf[1] );	//读FIFO，并发送给PC
    if(JRxCount > 2)
    {

        SPI_Write_Reg(JF24WRITE_REG+STATUS,val);	//清STATUS寄存器
        EXIT_TX_RX;	 //退出RX
        Enter_PRX();		//切换到PRX模式
        ENTER_TX_RX;	//进入RX

    }

}



/****************************************************************
基础操作部分
*****************************************************************/

/**************************************************************
**函数名称：SPI_RW()
**函数描述：写一个字节到JF24D，并返回读出的字节
**入口参数：命令或地址
**出口参数：读出的字节
**************************************************************/
//u8 SPI_RW(u8 byte)
//{
//    u8 bit_ctr;
//    for(bit_ctr=0; bit_ctr<8; bit_ctr++)
//    {
//        if((byte&0x80)!=0)
//            SPI_MOSI_H;
//        else
//            SPI_MOSI_L;
//        byte = (byte << 1);           // 下一位输出值移位到高位
//        Delayus(1) ;

//        SPI_CLK_H;

//        if(SPI_MISO)
//            byte |= 1;      		  // 读MISO当前值
//        Delayus(1) ;
//        SPI_CLK_L;
//        // SCK清零
//    }
//    return(byte);           		  // 返回读出的值
//}

/**************************************************************
**函数名称：SPI_Write_Reg()
**函数描述：写寄存器的值
**入口参数：寄存器地址+命令，寄存器的值
**出口参数：无
**************************************************************/
void SPI_Write_Reg(u8 reg, u8 value)
{
    SPI_CSN_L;                   // 清零CSN，使能SPI
    SPI_RW(reg);      // 写寄存器地址+命令
    SPI_RW(value);             // 写相应的值
    SPI_CSN_H;                   // 置位CSN，禁止SPI
}

/**************************************************************
**函数名称：SPI_Read_Reg()
**函数描述：读寄存器的值
**入口参数：寄存器地址+命令
**出口参数：寄存器的值
**************************************************************/
u8 SPI_Read_Reg(u8 reg)
{
    u8 value;
    SPI_CSN_L;                // 清零CSN，使能SPI
    SPI_RW(reg);            // 写寄存器地址+命令
    value = SPI_RW(0);    // 读寄存器的值
    SPI_CSN_H;                // 置位CSN，禁止SPI

    return(value);        // 返回寄存器的值
}

/**************************************************************
**函数名称：SPI_Read_Buf()
**函数描述：读多字节寄存器的值
**入口参数：寄存器地址+命令，返回值的地址，寄存器值的长度
**出口参数：无
**************************************************************/
void SPI_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                    		// 清零CSN，使能SPI
    SPI_RW(reg);       		// 写寄存器地址+命令

    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);    // 读寄存器的值

    SPI_CSN_H;                           // 置位CSN，禁止SPI
}

/**************************************************************
**函数名称：SPI_Write_Buf()
**函数描述：写多字节寄存器的值
**入口参数：寄存器地址+命令，写入值的地址，寄存器值的长度
**出口参数：无
***************************************************************/
void SPI_Write_Buf(u8 reg,const u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                   // 清零CSN，使能SPI
    SPI_RW(reg);    // 写寄存器地址+命令
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // 写寄存器的值
        SPI_RW(pBuf[byte_ctr]);
    SPI_CSN_H;                 // 置位CSN，禁止SPI
}

/**************************************************************
**函数名称：Enter_PRX()
**函数描述：切换到PRX模式
**入口参数：无
**出口参数：无
***************************************************************/
void Enter_PRX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_RX,0);//清空接收FIFO

    value=SPI_Read_Reg(STATUS);	// 读STATUS寄存器
    SPI_Write_Reg(JF24WRITE_REG+STATUS,value);// 清零RX_DR、TX_DS、MAX_RT标志

    value=SPI_Read_Reg(CONFIG);	// 读CONFIG寄存器

    value=value&0xfd;//位1清零
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // 清零PWR_UP位，进入POWER_DOWN模式

    value=value|0x03; //置位位1，位0
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // 置位PWR_UP，PRIM_RX，进入PRX模式

    //	JRecMode=TRUE;//无线模块接收模式
    //EXTI_Config(TRUE);  //开接收中断
}

/**************************************************************
**函数名称：Enter_PTX()
**函数描述：切换到PTX模式
**入口参数：无
**出口参数：无
***************************************************************/
void Enter_PTX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_TX,0);//清空接收FIFO

    value=SPI_Read_Reg(STATUS);	// 读STATUS寄存器
    SPI_Write_Reg(JF24WRITE_REG+STATUS,value);// 清零RX_DR、TX_DS、MAX_RT标志

    value=SPI_Read_Reg(CONFIG);	// 读CONFIG寄存器
    value=value&0xfd;//位1清零
    SPI_Write_Reg(JF24WRITE_REG+CONFIG, value); // 清零PWR_UP位，进入POWER_DOWN模式

    value=value|0x02;//置位位1
    value=value&0xfe; //位0清零
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // 置位PWR_UP，清零PRIM_RX，进入PTX模式
}

/**************************************************************
**函数名称：SwitchCFG()
**函数描述：切换寄存器组
**入口参数：将要切换到寄存器组，0或1
**出口参数：无
***************************************************************/
void SwitchCFG(u8 cfg)
{
    u8 Tmp;

    Tmp=SPI_Read_Reg(STATUS);		//读STATUS寄存器
    Tmp=Tmp&0x80;

    if(((Tmp)&&(cfg==0))||((Tmp==0)&&(cfg)))			 //判断当前寄存器组是否是将要切换的
    {
        SPI_Write_Reg(ACTIVATE_CMD,0x53);		   //执行切换
    }
}
/*********************************************************************************
Function:     WirteRxAddrToJf24D
Description:  set wireless module   Rx/Tx address & RF_channle
Input:        u8 channel   which channel will be operated.
              u8 status    open or close

Output:       None
Return:       None
Others:	      don't care
*********************************************************************************/
void WirteRxAddrToJf24D(u8 address0,u8 address1,u8 address2,u8 address3,u8 address4,u8 address5)
{

    RX0_Address[0] = address0;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P0),RX0_Address,5); 		//写寄存器10，通道0地址
    RX0_Address[0]  = address1;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P1),RX0_Address,5); 		//写寄存器11，通道1地址，及其余通道高位地址
    RX0_Address[0]  = address2;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P2),RX0_Address,1); 		//写寄存器11，通道2地址，及其余通道高位地址
    RX0_Address[0]  = address3;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P3),RX0_Address,1); 		//写寄存器11，通道3地址，及其余通道高位地址
    RX0_Address[0]  = address4;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P4),RX0_Address,1); 		//写寄存器11，通道4地址，及其余通道高位地址
    RX0_Address[0]  = address5;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P5),RX0_Address,1); 		//写寄存器11，通道5地址，及其余通道高位地址

}

void WriteTxAddress(u8 address)
{
    RX0_Address[0] = address;
    SPI_Write_Buf((JF24WRITE_REG+TX_ADDR),RX0_Address,5); 			//写寄存器16，发射通道地址
}
/**************************************************************
**函数名称：JF24D_Init()
**函数描述：初始化JF24D
**入口参数：无
**出口参数：无
***************************************************************/
void JF24D_Init(void)
{

    u8 i;
    for(i=21; i>0; i--)
    {
        SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[i-1][0]),RegArrFSK[i-1][1]);
    }
    WriteTxAddress(0x01);
    i=SPI_Read_Reg(29); 		//读寄存器29
    if(i==0) 			//是否需要激活
    {
       SPI_Write_Reg(ACTIVATE_CMD,0x73);  //激活寄存器29
       SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[22][0]),RegArrFSK[22][1]); 	//写寄存器28、29
       SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[21][0]),RegArrFSK[21][1]); 	//写寄存器28、29
    }


}




/**************************************************************
**函数名称：Send_Packet()
**函数描述：发射数据包
**入口参数：写发射FIFO命令，写入值的地址，寄存器值的长度
**出口参数：无
***************************************************************/
void Send_Packet(u8 type,u8* pbuf,u8 len)
{
    u8 fifo_sta;

    fifo_sta=SPI_Read_Reg(FIFO_STATUS);	// 读寄存器FIFO_STATUS

    if((fifo_sta&FIFO_STATUS_TX_FULL)==0)//判断发射FIFO是否满
        SPI_Write_Buf(type, pbuf, len); // 写发射FIFO
}
/*********************************************************************************
Function:  ModBus_CRC16
Description:
Calculate  ModBus_CRC16

Input:       uchar *puchMsg :   the buffer address
           uchar crc_count :   length of data buffer

Output:       None
Return:       CRC
Others:	      don't care
*********************************************************************************/
u16 ModBus_CRC16(u8 *puchMsg,u8 crc_count)
{
    u8 i ,j;
    u8 XORResult;
    u16 xorCRC = 0xA001; //??CRC????
    u16 CRCdata = 0xFFFF;
    for ( i = 0; i <crc_count; i++)
    {
        CRCdata ^= puchMsg[i];
        for (j = 0; j < 8; j++)
        {
            XORResult = CRCdata & 1;
            CRCdata >>= 1;
            if (XORResult)
                CRCdata ^= xorCRC;
        }
    }

    return  CRCdata;
}


/*********************************************************************************
Function:     Wireless_SendData
Description:  
Input:       u8 *OrderData  the point of data cache.
             u8 count       lent of data transmited

Output:       None
Return:       None
Others:	      don't care
*********************************************************************************/
void Wireless_SendData(u8 *OrderData,u8 count)
{
    u16 CrcData;
    u8 i;
    for(i=0; i<=count; i++)
    {
        JTxBuf[i]=OrderData[i];
    }


    JTxCount = count + 2;//两个CRC
    if(JTxCount>32)JTxCount=32;
    CrcData = ModBus_CRC16(JTxBuf,JTxCount-2);//得到crc校验码
    JTxBuf[JTxCount-2] = CrcData & 0xff;
    JTxBuf[JTxCount-1] = CrcData >>8;  //将crc校验码加入发送缓存数组
    WriteTxAddress(JTxBuf[0]);


    EXIT_TX_RX;
    Enter_PTX();		//切换到PTX模式
    ENTER_TX_RX;			//进入TX
#if(WITH_PA==1)
    PA_Tx;//
    SimpleDelayMs(2);
#endif
    SimpleDelayMs(1);
    Send_Packet(W_TX_PAYLOAD_NOACK_CMD,JTxBuf,JTxCount);
    WaitWirelesTx();
    EXIT_TX_RX;
    Enter_PRX();		//切换到PRX模式
    ENTER_TX_RX;
#if(WITH_PA==1)
    PA_Rx;//
#endif
}
/**************************************************************
**函数名称：Receive_Packet()
**函数描述：接收数据包
**入口参数：读接收FIFO命令，读出值的地址
**出口参数：数据包的长度
***************************************************************/
u8 Receive_Packet(u8* rx_buf)
{
    u8 len,fifo_sta;

    do
    {
        len=SPI_Read_Reg(R_RX_PL_WID_CMD);	// 读数据包长度

        if(len<=MAX_PACKET_LEN)
        {
            SPI_Read_Buf(RD_RX_PLOAD,rx_buf,len);// 读接收FIFO
        }
        else
        {
            SPI_Write_Reg(FLUSH_RX,0);//数据包长度大于最大长度，清空FIFO
        }
        fifo_sta=SPI_Read_Reg(FIFO_STATUS);	//读寄存器FIFO_STATUS
    } while((fifo_sta&FIFO_STATUS_RX_EMPTY)==0); //如果不为空，继续读

    return(len);		  //返回数据包长度
}


/*@*****************************end of file*************************************@*/

