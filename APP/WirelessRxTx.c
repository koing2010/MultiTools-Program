/*******************************************************************************

File_name:      WirelessRxTx.h
Version:		    0.1
Revised:        $Date:2016-2-16 ; $
Description:    this sector is processing wireless module which includes data 
                transmiting and data receiving.
Notes:          a task of this project.
Editor:		      Mr.Kon

*******************************************************************************/


/*==============================================================================
@ Include files
*/
#include "ucos_ii.h"
#include "object.h"
#include "WirelessRxTx.h"


/*==============================================================================
@ Global variable
*/




/*==============================================================================
@ All functions  as follow
*/
 extern CONTROL_LAYER CT_Info;

/*******************************************************************************
Function: ReceiveWireless
Description:  wireless module interrupt service function.
Input:        None
Output:       None
Return:       None
Others:	     koing2010@2016/2/20
*******************************************************************************/
#include "jf24d.h"
void ReceiveWireless(void)
{
    u8 val;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr;
#endif
    //quanju_a=
    val = SPI_Read_Reg(STATUS);			//��STATUS�Ĵ���
    if (val & STATUS_RX_DR)			//���������
    {
        OS_ENTER_CRITICAL()
        //��������ʱ��Ҫ�������жϴ��

        //quanju_a1=
        CT_Info.wireless_RxCount = Receive_Packet(CT_Info.wireless_RxBuffer);
        SPI_Write_Reg(JF24WRITE_REG + STATUS, val);		//��STATUS�Ĵ���
        OS_EXIT_CRITICAL()
        //�˳��ٽ��
        OSSemPost(CT_Info.SemWrieRx);


    }
    else if(val & STATUS_TX_DS)
    {   //
        SPI_Write_Reg(JF24WRITE_REG + STATUS, val);		//��STATUS�Ĵ���
        OSSemPost(CT_Info.SemWrieTx);

    }
    else
    {
        SPI_Write_Reg(JF24WRITE_REG + STATUS, val);		//��STATUS�Ĵ���
    }
    return;
}

/*******************************************************************************
Function: WaitWirelesTx
Description:  waits for semaphore that wireless_Tx succeed .
Input:        None
Output:       None
Return:       if tx succeed,return 1,if false, retun 0
Others:	     koing2010@2016/2/20
*******************************************************************************/
unsigned char WaitWirelesTx(void)
{
    u8 err ;
    OSSemPend(CT_Info.SemWrieTx,50,&err); //wait semaphore
    if(err == OS_ERR_NONE)
    {
        return 1;
    }
    return 0;
}

/*@*****************************end of file**********************************@*/
