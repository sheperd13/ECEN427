/*****************************************************************************
* Filename:          C:\Users\superman\Desktop\SandC/drivers/my_dma_v1_00_a/src/my_dma.c
* Version:           1.00.a
* Description:       my_dma Driver Source File
* Date:              Thu Dec 14 11:00:16 2017 (by Create and Import Peripheral Wizard)
*****************************************************************************/


/***************************** Include Files *******************************/

#include "my_dma.h"

/************************** Function Definitions ***************************/


/**
 *
 * User logic master module to send/receive word to/from remote system memory.
 * While sending, one word is read from user logic local FIFO and written to remote system memory.
 * While receiving, one word is read from remote system memory and written to user logic local FIFO.
 *
 * @param   BaseAddress is the base address of the MY_DMA device.
 * @param   Src/DstAddress is the destination system memory address from/to which the data will be fetched/stored.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void MY_DMA_MasterSendWord(Xuint32 BaseAddress, Xuint32 DstAddress)
{
  /*
   * Set user logic master control register for write transfer.
   */
  Xil_Out8(BaseAddress+MY_DMA_MST_CNTL_REG_OFFSET, MST_SGWR);

  /*
   * Set user logic master address register to drive IP2Bus_Mst_Addr signal.
   */
  Xil_Out32(BaseAddress+MY_DMA_MST_ADDR_REG_OFFSET, DstAddress);

  /*
   * Set user logic master byte enable register to drive IP2Bus_Mst_BE signal.
   */
  Xil_Out16(BaseAddress+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);

  /*
   * Start user logic master write transfer by writting special pattern to its go port.
   */
  Xil_Out8(BaseAddress+MY_DMA_MST_GO_PORT_OFFSET, MST_START);
}

void MY_DMA_MasterRecvWord(Xuint32 BaseAddress, Xuint32 SrcAddress)
{
  /*
   * Set user logic master control register for read transfer.
   */
  Xil_Out8(BaseAddress+MY_DMA_MST_CNTL_REG_OFFSET, MST_SGRD);

  /*
   * Set user logic master address register to drive IP2Bus_Mst_Addr signal.
   */
  Xil_Out32(BaseAddress+MY_DMA_MST_ADDR_REG_OFFSET, SrcAddress);

  /*
   * Set user logic master byte enable register to drive IP2Bus_Mst_BE signal.
   */
  Xil_Out16(BaseAddress+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);

  /*
   * Start user logic master read transfer by writting special pattern to its go port.
   */
  Xil_Out8(BaseAddress+MY_DMA_MST_GO_PORT_OFFSET, MST_START);
}

