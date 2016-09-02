/************************************************************************************

File_name:       communication.h
Description:    the header file of  Model_c .

************************************************************************************/
#ifndef     __COMMUNICATION_H
#define     __COMMUNICATION_H

/*==================================================================================
@ Include files
*/
#include "stm32f10x.h"
#include  "ucos_ii.h"
/*==================================================================================
@ Typedefs
*/

/*==================================================================================
@ Constants and defines
*/
 void USART1_SendData(u8 *str, u8 length) ;
 void USART2_SendData(u8 *str, u8 length) ;
 u8 USART1_DMASendData(u8 *str, u8 length);
 u8 USART3_DMASendData(u8 *str, u8 length);
 u8 USART1_DMAGetNumbRecieved(u8 *str, u8 length);
 u8 USART3_DMAGetNumbRecieved(u8 *str, u8 length);








#endif
/*@*****************************end of file**************************************@*/


