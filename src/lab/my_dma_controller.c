/*
 * my_dma_controller.c
 *
 *  Created on: Dec 13, 2017
 *      Author: superman
 */
#include "my_dma_controller.h"
#include "my_dma.h"
#include "xparameters.h"

#define ENABLE_DMA_OP 0x1	//	mask for enabling the dma hardware transfer

#define SOURCE_REGISTER MY_DMA_SLV_REG0_OFFSET	//src register offset
#define DEST_REGISTER  MY_DMA_SLV_REG1_OFFSET	//dest register offset
#define SIZE_REGISTER MY_DMA_SLV_REG2_OFFSET	//sizz register offset
#define CONTROL_REGISTER MY_DMA_SLV_REG3_OFFSET	//cntrl register offset

void my_dma_controller_load_registers(uint32_t source, uint32_t destination) {
	uint32_t status_register_values;
	Xil_Out32((XPAR_MY_DMA_0_BASEADDR) + (SOURCE_REGISTER), source);	//writes to source register
	if (Xil_In32((XPAR_MY_DMA_0_BASEADDR) + (SOURCE_REGISTER)) != source) {	//error checking
		xil_printf("Something went wrong writing to source register.\n\r");
	}

	Xil_Out32((XPAR_MY_DMA_0_BASEADDR) + (DEST_REGISTER), destination);	//writes to the dest register
	if (Xil_In32((XPAR_MY_DMA_0_BASEADDR) + (DEST_REGISTER)) != destination) {	//error checking
		xil_printf("Something went wrong writing to destination register.\n\r");
	}
}

// sets the size of the number of bytes that will be transfered. If
// interrupts are enabled, an interrupt will happen when transfer is complete
void my_dma_controller_set_size(uint32_t size) {
	Xil_Out32((XPAR_MY_DMA_0_BASEADDR) + (SIZE_REGISTER), size);
	if (Xil_In32((XPAR_MY_DMA_0_BASEADDR) + (SIZE_REGISTER)) != size) {
		xil_printf("Something went wrong writing to size register.\n\r");
	}
}

// enable hardware dependant transfer
void my_dma_controller_enable_transfer() {
	uint32_t status_register_value;
	status_register_value = Xil_In32((XPAR_MY_DMA_0_BASEADDR) + (CONTROL_REGISTER));
	Xil_Out32((XPAR_MY_DMA_0_BASEADDR) + (CONTROL_REGISTER), status_register_value | ENABLE_DMA_OP);
}

//disables transfer
void my_dma_controller_disable_transfer() {
	Xil_Out32((XPAR_MY_DMA_0_BASEADDR) + (CONTROL_REGISTER), 0);
}

// begin the transfer
void my_dma_controller_start_transfer() {
	  Xil_Out16(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);
	  Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_GO_PORT_OFFSET, MST_START);

}

// starts the transfer operation
void my_dma_controller_begin_whole_op(uint32_t source, uint32_t destination, uint32_t size) {
	my_dma_controller_load_registers(source, destination);
	my_dma_controller_set_size(size);
	my_dma_controller_enable_transfer();
	my_dma_controller_start_transfer();
}

//controls the software transfer. just uses memcpy as you can see
void my_dma_controller_software_transfer(uint32_t* source, uint32_t* destination, uint32_t size) {
	uint32_t i = 0;
	uint32_t j = 0;
	memcpy(destination,source, size);
}
