/**********************************************************************************

File_name:      communication.c
Version:		    0.1
Revised:        $Date:2016-2-17 to 18 ; $
Description:    Universal function.
Notes:          This version targets the stm32/UCos
Editor:		      Mr.Kong

**********************************************************************************/

/*================================================================================
@ Include files
*/
#include "communication.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
/*================================================================================
@ Global variable
*/

/*================================================================================
@ All functions  as follow
*/
/*********************************************************************************
Function:     USART1_Receive()
Description:  First function called after startup.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/

/*********************************************************************************
Function:     USART1_Receive()
Description:  First function called after startup.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/


/*********************************************************************************
Function:       USART1_SendData
Description:
Input:             str : point of tx buffer
                length : size of tx buffer
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
void USART1_SendData(u8 *str, u8 length)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    for (; length>0; length--,str++)
    {
        USART_SendData(USART1,*str);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USART2_SendData(u8 *str, u8 length)
{
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    for (; length>0; length--,str++)
    {
        USART_SendData(USART2,*str);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/*********************************************************************************
Function:     USART1_DMASendData
Description:  if use this function,DMA1_channl4 needs configurated before.
Input:        str : point of tx buffer
              length : size of tx buffer
Output:       None
Return:       don't care.
Others:       by Mr.kon
*********************************************************************************/
u8 USART1_DMASendData(u8 *str, u8 length)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC4) ==  RESET)
    {
        //last fram data has not been transmited completely.
        return 0;
    }
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA1_Channel4->CNDTR =  length;
    DMA1_Channel4->CMAR = (u32)str;
    DMA_Cmd(DMA1_Channel4, ENABLE);
    return 1;
}

u8 USART3_DMASendData(u8 *str, u8 length)
{
//    if(DMA_GetFlagStatus(DMA1_FLAG_TC4) ==  RESET)
//    {
//        //last fram data has not been transmited completely.
//        return 0;
//    }
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC2);
    DMA1_Channel2->CNDTR =  length;
    DMA1_Channel2->CMAR = (u32)str;
    DMA_Cmd(DMA1_Channel2, ENABLE);
    return 1;
}


/*********************************************************************************
Function:     USART1_DMAGetNumberRecieved
Description:  if use this function,DMA1_channl4 needs configurated before.
Input:        str : point of  next Rx buffer
              length : size of next Rx buffertx buffer
Output:       None
Return:       the number of last fram received.
Others:       by Mr.kon
*********************************************************************************/
u8 USART1_DMAGetNumbRecieved(u8 *str, u8 length)
{
    u8 Lenth = 0;
    DMA_Cmd(DMA1_Channel5, DISABLE);
    /* calculate the number of bytes received */
    Lenth= length - DMA1_Channel5->CNDTR;
  
    /* reset buffer lead address */
    DMA1_Channel4->CMAR = (u32)str;
  
    /* reset buffer size */
    DMA1_Channel5->CNDTR  = length;
    DMA_Cmd(DMA1_Channel5, ENABLE);
    return Lenth;
}

u8 USART3_DMAGetNumbRecieved(u8 *str, u8 length)
{
    u8 Lenth = 0;
    DMA_Cmd(DMA1_Channel3, DISABLE);
    /* calculate the number of bytes received */
    Lenth= length - DMA1_Channel3->CNDTR;
  
    /* reset buffer lead address */
    DMA1_Channel3->CMAR = (u32)str;
  
    /* reset buffer size */
    DMA1_Channel3->CNDTR  = length;
    DMA_Cmd(DMA1_Channel3, ENABLE);
    return Lenth;
}
/*@*****************************end of file*************************************@*/
