/*******************************************************************************

File_name:       Model.h
Description:    the header file of  Model_c .

*******************************************************************************/
#ifndef     __CONTROLLAYER_H__
#define     __CONTROLLAYER_H__

/*==============================================================================
@ Include files
*/
#include "controllayer.h"
#include "stm32f10x.h"
/*==============================================================================
@ Typedefs
*/
#pragma pack(push,1) 
 typedef struct PROTOCOL_
 {
 u16 startCode;
 u8  data[6];
 u8  portAddress;   /*  1-127     wireless channel
                          128     CAN port aadress
                          129     485 port aadress
                        */
 } *pPROTOCOL;


typedef struct USART_CFG_
{
    /* USART3 default configured as follow:
          - BaudRate = 9600 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - Parity No
          - Hardware flow control desabled
          - Receive and transmit enabled
    */
  u16 BaudRate;
  u8  Word_Length;
  u8  Stop_Bit;
  u8  Parity;
  u8  HardwareFlowControl;
}*PUSART_CFG;
#pragma pack(pop)
#define  Wireless_Port     0
#define  CAN_Port          1
#define  RS485_Port        2
/*==============================================================================
@ Constants and defines
*/
 u8 ProtocolControl( pPROTOCOL protocol );








#endif
/*@*****************************end of file**********************************@*/


