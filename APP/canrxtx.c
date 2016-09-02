/*******************************************************************************
File_name:      canrxtx.c
Version:		    0.0
Revised:        $Date:2016-2-23  ; $
Description:    can port reiceive or transimit.
Notes:          This version targets the SH99F01
Editor:		      Mr.Kong

*******************************************************************************/


/*==============================================================================
@ Include files
*/
#include "stm32f10x.h"
#include "canrxtx.h"
#include "string.h"

/*==============================================================================
@ Global variable
*/




/*==============================================================================
@ All functions  as follow
*/



/*******************************************************************************
Function:     CanTx
Description:  transmit data by can port
Input:        PCL_CANFORMAT CanFormat  CAN5.0 protocol format
Output:       None
Return:       None
Others:	      You can add what you need in.
*******************************************************************************/
void CanTx(PCanTxMsg CanFormat)
{
    u8 i = 0;
    u8 TransmitMailbox;
    TransmitMailbox = CAN_Transmit(CAN1, CanFormat);
    while ((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
    {
        i++;
        if (i > 200)  //10-20s 发送超时然后进行下一帧数据发送，重新配置CAN 进行下一帧数据发送
        {
            break;
        }
    }
    //DCAN_H;
}

/*@*****************************end of file**********************************@*/
