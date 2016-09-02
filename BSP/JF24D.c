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
//�°�JF24D
//�Ĵ�����1  0-13�żĴ����ĳ�ʼֵ
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
//�Ĵ�����1
const u8 RegArrFSKAnalogReg14[]=
{
    0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};
#endif
/**************************0��Ĵ�����ʼֵ********************/
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
//�ϰ������ģ�� ����ģ�鱳���ע VERSIN 03B
//�Ĵ�����1  0-13�żĴ����ĳ�ʼֵ
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

//�Ĵ�����1  14�żĴ����ĳ�ʼֵ
const u8 RegArrFSKAnalogReg14[]=
{
    0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};

//�Ĵ�����0��ʼֵ
const u8 RegArrFSK[][2]= {
    {0,0x0F},		 //���üĴ���
    {1,0x3F},
    {2,0x3F},		 //ʹ�ܽ��յ�ַ
    {3,0x03},		 //�ŵ������ݳ���
    {4,0xff},
    {5,0x17},        //Ƶ��ѡ��Ĵ���
    {6,0x17},
    {7,0x07},	     //״̬�Ĵ���
    {8,0x00},		 //��Ƶ���üĴ���
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


    /*ʹ��GPIOB,GPIOD,���ù���ʱ��*/   /*ʹ��SPI1ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_AFIO, ENABLE);


    /*��SPI�� SCK,MISO,MOSI���ţ�PA5,PA6,PA7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù���
    GPIO_Init(GPIOA, &GPIO_InitStructure);




    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//ʱ�Ӽ��ԣ�����ʱΪ��
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge ;						//��1��������Ч��������Ϊ����ʱ��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4 ;//SPI_BaudRatePrescaler_8;  //8��Ƶ��9MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI2  */
    SPI_Cmd(SPI1, ENABLE);
}

u8 SPI_RW(u8 data)
{
    /* �� SPI���ͻ������ǿ�ʱ�ȴ� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* ͨ�� SPI2����һ�ֽ����� */
    SPI_I2S_SendData(SPI1, data);

    /* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
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
*��������InitWirelessModule
*���ã�  ��ʼ��jf24d
*************************************************/
void Wireless_Init(void)
{

    SimpleDelayMs(8);
    JF24D_Init();		//��ʼ��JF24D
    SimpleDelayMs(4);
    SimpleDelayMs(4);

    EXIT_TX_RX;		//����RX
    Enter_PRX();		//�л���PRXģʽ
    ENTER_TX_RX;		//����RX

}

/***********************************************
*��������IsInitWirelessModuleSuccess(void)
*���ã�  ���ôӼĴ�������ֵ�жϳ�ʼ���Ƿ�ɹ���
*����ֵ������0��ʾ��ʼ�����ɹ���
*************************************************/
//����ӼĴ���29����ֵΪ0x07����д���ֵ��ͬ��˵����ʼ���ɹ���
//�������ֵΪ0�򲻳ɹ�
u8 IsInitWirelessModuleSuccess(void)
{
    u8 rdata;
    rdata=SPI_Read_Reg(29); 		//���Ĵ���29
    if(rdata==0x07)
    {
        return 1;	   //��ʼ���ɹ�
    }
    else
    {
        return 0;	   //��ʼ��ʧ��
    }
}


/**************************************************
�������ƣ��������ݼ�������
����������
��ڲ�����
���ڲ�����
*************************************************/
void Wireless_RXData(void)
{
    u8 val;
    val = SPI_Read_Reg(STATUS);		//��STATUS�Ĵ���
    JRxCount = Receive_Packet(&JRxBuf[1] );	//��FIFO�������͸�PC
    if(JRxCount > 2)
    {

        SPI_Write_Reg(JF24WRITE_REG+STATUS,val);	//��STATUS�Ĵ���
        EXIT_TX_RX;	 //�˳�RX
        Enter_PRX();		//�л���PRXģʽ
        ENTER_TX_RX;	//����RX

    }

}



/****************************************************************
������������
*****************************************************************/

/**************************************************************
**�������ƣ�SPI_RW()
**����������дһ���ֽڵ�JF24D�������ض������ֽ�
**��ڲ�����������ַ
**���ڲ������������ֽ�
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
//        byte = (byte << 1);           // ��һλ���ֵ��λ����λ
//        Delayus(1) ;

//        SPI_CLK_H;

//        if(SPI_MISO)
//            byte |= 1;      		  // ��MISO��ǰֵ
//        Delayus(1) ;
//        SPI_CLK_L;
//        // SCK����
//    }
//    return(byte);           		  // ���ض�����ֵ
//}

/**************************************************************
**�������ƣ�SPI_Write_Reg()
**����������д�Ĵ�����ֵ
**��ڲ������Ĵ�����ַ+����Ĵ�����ֵ
**���ڲ�������
**************************************************************/
void SPI_Write_Reg(u8 reg, u8 value)
{
    SPI_CSN_L;                   // ����CSN��ʹ��SPI
    SPI_RW(reg);      // д�Ĵ�����ַ+����
    SPI_RW(value);             // д��Ӧ��ֵ
    SPI_CSN_H;                   // ��λCSN����ֹSPI
}

/**************************************************************
**�������ƣ�SPI_Read_Reg()
**�������������Ĵ�����ֵ
**��ڲ������Ĵ�����ַ+����
**���ڲ������Ĵ�����ֵ
**************************************************************/
u8 SPI_Read_Reg(u8 reg)
{
    u8 value;
    SPI_CSN_L;                // ����CSN��ʹ��SPI
    SPI_RW(reg);            // д�Ĵ�����ַ+����
    value = SPI_RW(0);    // ���Ĵ�����ֵ
    SPI_CSN_H;                // ��λCSN����ֹSPI

    return(value);        // ���ؼĴ�����ֵ
}

/**************************************************************
**�������ƣ�SPI_Read_Buf()
**���������������ֽڼĴ�����ֵ
**��ڲ������Ĵ�����ַ+�������ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
**���ڲ�������
**************************************************************/
void SPI_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                    		// ����CSN��ʹ��SPI
    SPI_RW(reg);       		// д�Ĵ�����ַ+����

    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);    // ���Ĵ�����ֵ

    SPI_CSN_H;                           // ��λCSN����ֹSPI
}

/**************************************************************
**�������ƣ�SPI_Write_Buf()
**����������д���ֽڼĴ�����ֵ
**��ڲ������Ĵ�����ַ+���д��ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
**���ڲ�������
***************************************************************/
void SPI_Write_Buf(u8 reg,const u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                   // ����CSN��ʹ��SPI
    SPI_RW(reg);    // д�Ĵ�����ַ+����
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // д�Ĵ�����ֵ
        SPI_RW(pBuf[byte_ctr]);
    SPI_CSN_H;                 // ��λCSN����ֹSPI
}

/**************************************************************
**�������ƣ�Enter_PRX()
**�����������л���PRXģʽ
**��ڲ�������
**���ڲ�������
***************************************************************/
void Enter_PRX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_RX,0);//��ս���FIFO

    value=SPI_Read_Reg(STATUS);	// ��STATUS�Ĵ���
    SPI_Write_Reg(JF24WRITE_REG+STATUS,value);// ����RX_DR��TX_DS��MAX_RT��־

    value=SPI_Read_Reg(CONFIG);	// ��CONFIG�Ĵ���

    value=value&0xfd;//λ1����
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // ����PWR_UPλ������POWER_DOWNģʽ

    value=value|0x03; //��λλ1��λ0
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // ��λPWR_UP��PRIM_RX������PRXģʽ

    //	JRecMode=TRUE;//����ģ�����ģʽ
    //EXTI_Config(TRUE);  //�������ж�
}

/**************************************************************
**�������ƣ�Enter_PTX()
**�����������л���PTXģʽ
**��ڲ�������
**���ڲ�������
***************************************************************/
void Enter_PTX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_TX,0);//��ս���FIFO

    value=SPI_Read_Reg(STATUS);	// ��STATUS�Ĵ���
    SPI_Write_Reg(JF24WRITE_REG+STATUS,value);// ����RX_DR��TX_DS��MAX_RT��־

    value=SPI_Read_Reg(CONFIG);	// ��CONFIG�Ĵ���
    value=value&0xfd;//λ1����
    SPI_Write_Reg(JF24WRITE_REG+CONFIG, value); // ����PWR_UPλ������POWER_DOWNģʽ

    value=value|0x02;//��λλ1
    value=value&0xfe; //λ0����
    SPI_Write_Reg(JF24WRITE_REG + CONFIG, value); // ��λPWR_UP������PRIM_RX������PTXģʽ
}

/**************************************************************
**�������ƣ�SwitchCFG()
**�����������л��Ĵ�����
**��ڲ�������Ҫ�л����Ĵ����飬0��1
**���ڲ�������
***************************************************************/
void SwitchCFG(u8 cfg)
{
    u8 Tmp;

    Tmp=SPI_Read_Reg(STATUS);		//��STATUS�Ĵ���
    Tmp=Tmp&0x80;

    if(((Tmp)&&(cfg==0))||((Tmp==0)&&(cfg)))			 //�жϵ�ǰ�Ĵ������Ƿ��ǽ�Ҫ�л���
    {
        SPI_Write_Reg(ACTIVATE_CMD,0x53);		   //ִ���л�
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
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P0),RX0_Address,5); 		//д�Ĵ���10��ͨ��0��ַ
    RX0_Address[0]  = address1;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P1),RX0_Address,5); 		//д�Ĵ���11��ͨ��1��ַ��������ͨ����λ��ַ
    RX0_Address[0]  = address2;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P2),RX0_Address,1); 		//д�Ĵ���11��ͨ��2��ַ��������ͨ����λ��ַ
    RX0_Address[0]  = address3;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P3),RX0_Address,1); 		//д�Ĵ���11��ͨ��3��ַ��������ͨ����λ��ַ
    RX0_Address[0]  = address4;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P4),RX0_Address,1); 		//д�Ĵ���11��ͨ��4��ַ��������ͨ����λ��ַ
    RX0_Address[0]  = address5;
    SPI_Write_Buf((JF24WRITE_REG+RX_ADDR_P5),RX0_Address,1); 		//д�Ĵ���11��ͨ��5��ַ��������ͨ����λ��ַ

}

void WriteTxAddress(u8 address)
{
    RX0_Address[0] = address;
    SPI_Write_Buf((JF24WRITE_REG+TX_ADDR),RX0_Address,5); 			//д�Ĵ���16������ͨ����ַ
}
/**************************************************************
**�������ƣ�JF24D_Init()
**������������ʼ��JF24D
**��ڲ�������
**���ڲ�������
***************************************************************/
void JF24D_Init(void)
{

    u8 i;
    for(i=21; i>0; i--)
    {
        SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[i-1][0]),RegArrFSK[i-1][1]);
    }
    WriteTxAddress(0x01);
    i=SPI_Read_Reg(29); 		//���Ĵ���29
    if(i==0) 			//�Ƿ���Ҫ����
    {
       SPI_Write_Reg(ACTIVATE_CMD,0x73);  //����Ĵ���29
       SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[22][0]),RegArrFSK[22][1]); 	//д�Ĵ���28��29
       SPI_Write_Reg((JF24WRITE_REG|RegArrFSK[21][0]),RegArrFSK[21][1]); 	//д�Ĵ���28��29
    }


}




/**************************************************************
**�������ƣ�Send_Packet()
**�����������������ݰ�
**��ڲ�����д����FIFO���д��ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
**���ڲ�������
***************************************************************/
void Send_Packet(u8 type,u8* pbuf,u8 len)
{
    u8 fifo_sta;

    fifo_sta=SPI_Read_Reg(FIFO_STATUS);	// ���Ĵ���FIFO_STATUS

    if((fifo_sta&FIFO_STATUS_TX_FULL)==0)//�жϷ���FIFO�Ƿ���
        SPI_Write_Buf(type, pbuf, len); // д����FIFO
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


    JTxCount = count + 2;//����CRC
    if(JTxCount>32)JTxCount=32;
    CrcData = ModBus_CRC16(JTxBuf,JTxCount-2);//�õ�crcУ����
    JTxBuf[JTxCount-2] = CrcData & 0xff;
    JTxBuf[JTxCount-1] = CrcData >>8;  //��crcУ������뷢�ͻ�������
    WriteTxAddress(JTxBuf[0]);


    EXIT_TX_RX;
    Enter_PTX();		//�л���PTXģʽ
    ENTER_TX_RX;			//����TX
#if(WITH_PA==1)
    PA_Tx;//
    SimpleDelayMs(2);
#endif
    SimpleDelayMs(1);
    Send_Packet(W_TX_PAYLOAD_NOACK_CMD,JTxBuf,JTxCount);
    WaitWirelesTx();
    EXIT_TX_RX;
    Enter_PRX();		//�л���PRXģʽ
    ENTER_TX_RX;
#if(WITH_PA==1)
    PA_Rx;//
#endif
}
/**************************************************************
**�������ƣ�Receive_Packet()
**�����������������ݰ�
**��ڲ�����������FIFO�������ֵ�ĵ�ַ
**���ڲ��������ݰ��ĳ���
***************************************************************/
u8 Receive_Packet(u8* rx_buf)
{
    u8 len,fifo_sta;

    do
    {
        len=SPI_Read_Reg(R_RX_PL_WID_CMD);	// �����ݰ�����

        if(len<=MAX_PACKET_LEN)
        {
            SPI_Read_Buf(RD_RX_PLOAD,rx_buf,len);// ������FIFO
        }
        else
        {
            SPI_Write_Reg(FLUSH_RX,0);//���ݰ����ȴ�����󳤶ȣ����FIFO
        }
        fifo_sta=SPI_Read_Reg(FIFO_STATUS);	//���Ĵ���FIFO_STATUS
    } while((fifo_sta&FIFO_STATUS_RX_EMPTY)==0); //�����Ϊ�գ�������

    return(len);		  //�������ݰ�����
}


/*@*****************************end of file*************************************@*/

