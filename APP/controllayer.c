/*******************************************************************************

File_name:       controllayer.c
Version:		   0.0
Revised:        $Date:2014-5-  ; $
Description:    the main file of this project.
Notes:          This version targets the SH99F01
Editor:		    Mr.Kong

*******************************************************************************/


/*==============================================================================
@ Include files
*/
#include "controllayer.h"
#include "object.h"
#include "JF24D.h"
#include "BSP.h"

/*==============================================================================
@ Global variable
*/
CONTROL_LAYER CT_Info;



/*==============================================================================
@ All functions  as follow
*/



/*******************************************************************************
Function: Init_System_Peripher
Description:
     Every peripher used in program could  got initialized in this module .


Input:        None
Output:       None
Return:       None
Others:	   You can add what you need in.
*******************************************************************************/
u8 ProtocolControl( pPROTOCOL protocol )
{
    /* USB_RF */
    if(protocol->portAddress < 128)
    {
        Change_WirelessChannel(protocol->portAddress);

        WirteRxAddrToJf24D(protocol->data[0],protocol->data[1],
                           protocol->data[2],protocol->data[3],
                           protocol->data[4],protocol->data[5]);
                           //接收六通道

                           return 0;
    }
    /* USN_CAN */
    if(protocol->portAddress == 128)
    {
        /* set CAN Speed & CAN address */
        Can_Config(protocol->data[0], protocol->data[1]);	//此子函数用来配置can
      
       /*close wireless receive interrupt*/
        return 1;
    }  
    
    /* USB_485 */
    if(protocol->portAddress == 129)
    {
       /**/
       UARST3_Config((PUSART_CFG)protocol->data);
       return 2;
    }


    return 0;
}



/*@*****************************end of file**********************************@*/
