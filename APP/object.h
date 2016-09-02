/************************************************************************************

File_name:       Model.h
Description:    the header file of  Model_c .

************************************************************************************/
#ifndef     __OBJECT_H__
#define     __OBJECT_H__

/*==================================================================================
@ Include files
*/
#include "ucos_ii.h"
#include "stm32f10x.h"
#include "list.h"
/*==================================================================================
@ Typedefs
*/

typedef struct USART_
{
    OS_EVENT *semUsartTx;
    u8 currentRxCount;
    u8 usartTxbuf[40];
    u8 usartRxbuf[40];

} USART,*PUSART;

/*==================================================================================
@ Constants and defines
*/
typedef struct SYSTEM_
{
  
    OS_EVENT  *SemFramTxComplete;
    u8 wireless_address;
    u8 wireless_channel;
} SYSTEM_INFO;
typedef struct CONTROL_
{
  u8 Datadirection; //which Datadirection data stream will go to. 
  
  OS_EVENT *SemWrieRx;
  OS_EVENT *SemWrieTx;
  u8 wireless_RxCount;
  u8 wireless_RxBuffer[32];
} CONTROL_LAYER;


typedef struct COLLECT_
{
  OS_EVENT  *SemFramRxComplete;
  OS_EVENT  *SemCLCopyOver;
  
  
  /* usb->485 function parameter */
  OS_EVENT  *Sem485RxComplete;
  OS_EVENT  *Sem485TxComplete;
  u8 RS485RxLenth;
  u8 RS485Rxdata[40];
  
  /* usb->can function parameter */
  OS_EVENT  *SemCANRxComplete;
  CanRxMsg  RxMessage;
} COLLECT_LAYER;

#pragma pack(push,1)//one byte by one byte

/*
* data packed as follow format in collect layer. 
*/
typedef struct CL_FORMAT_
{
 u8  FramLenth;
  
 /*40bytes total as follow*/ 
 u16 address; 
 u8  data[38];
} CL_FORMAT,*PCL_FORMAT;

typedef struct CL_CAN_FORMAT_
{
  u8  PortNumber;
  u32 StdId;
  u8  DLC;
  u8  Data[8];
  u8  IDE;
  u8  RTR;
  u32 ExtId;
} CL_CANFORMAT,*PCL_CANFORMAT;
#pragma pack(pop)
#endif
/*@*****************************end of file**************************************@*/
