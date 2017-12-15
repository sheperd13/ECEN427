/*
 * main.c
 *
 *  Created on: Nov 29, 2017
 *      Author: superman
 */

#include <stdio.h>
#include <stdint.h>
#include "xintc_l.h"
#include "my_dma.h"
#include "platform.h"
#include "xparameters.h"
#include "my_dma_controller.h"

void print(char *str);

uint32_t *source_addr;

void dma_interrupt_handler(){
	xil_printf("dma_interrupt\r\n");
	my_dma_controller_disable_transfer();


}

// Main interrupt handler, queries the interrupt controller to see what peripheral
// fired the interrupt and then dispatches the corresponding interrupt handler.
// This routine acks the interrupt at the controller level but the peripheral
// interrupt must be ack'd by the dispatched interrupt handler.
// Question: Why is the timer_interrupt_handler() called after ack'ing the interrupt controller
// but pb_interrupt_handler() is called before ack'ing the interrupt controller?
void interrupt_handler_dispatcher(void* ptr) {
	int intc_status = XIntc_GetIntrStatus(XPAR_INTC_0_BASEADDR);
	if (intc_status & XPAR_MY_DMA_0_MY_INTERRUPT_MASK) {
		dma_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_MY_DMA_0_MY_INTERRUPT_MASK);
	}
}


void interrupt_init() {
    //init_platform();
    // Initialize the GPIO peripherals.
    microblaze_register_handler(interrupt_handler_dispatcher, NULL);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
    		(XPAR_MY_DMA_0_MY_INTERRUPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();
}

void dma_write(uint32_t* source_array) {
    /*
     * Set user logic master control register for read transfer.
     */
    Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_CNTL_REG_OFFSET, MST_SGRD);

    /*
     * Set user logic master address register to drive IP2Bus_Mst_Addr signal.
     */
    Xil_Out32(XPAR_MY_DMA_0_BASEADDR+ MY_DMA_MST_ADDR_REG_OFFSET, source_array);


    /*
     * Set user logic master byte enable register to drive IP2Bus_Mst_BE signal.
     */
    Xil_Out16(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);


    /*
     * Start user logic master read transfer by writting special pattern to its go port.
     */
    Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_GO_PORT_OFFSET, MST_START);
}


void dma_read(uint32_t* DstAddress) {
	  /*
	   * Set user logic master control register for write transfer.
	   */
	  Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_CNTL_REG_OFFSET, MST_SGWR);

	  /*
	   * Set user logic master address register to drive IP2Bus_Mst_Addr signal.
	   */
	  Xil_Out32(XPAR_MY_DMA_0_BASEADDR+ MY_DMA_MST_ADDR_REG_OFFSET, DstAddress);


	  /*
	   * Set user logic master byte enable register to drive IP2Bus_Mst_BE signal.
	   */
	  Xil_Out16(XPAR_MY_DMA_0_BASEADDR+ MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);

	  /*
	   * Start user logic master write transfer by writting special pattern to its go port.
	   */
	  Xil_Out8(XPAR_MY_DMA_0_BASEADDR + MY_DMA_MST_GO_PORT_OFFSET, MST_START);
}

void example_of_software_solution(uint32_t *source_array, uint32_t *destination_array) {
	uint8_t i = 0;
  xil_printf("inside software solution\n\r");
   while (i < 4) {
	   xil_printf("i is %d\n\r", i);
		Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_CNTL_REG_OFFSET, MST_BRRD);
		Xil_Out32(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_ADDR_REG_OFFSET, &source_array[i]);
		Xil_Out16(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);
		Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_GO_PORT_OFFSET, MST_START);
		xil_printf("i is %d partway\n\r", i);
		Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_CNTL_REG_OFFSET, MST_BRWR);
		Xil_Out32(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_ADDR_REG_OFFSET, &destination_array[i]);
		Xil_Out16(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_BE_REG_OFFSET, 0xFFFF);
		Xil_Out8(XPAR_MY_DMA_0_BASEADDR+MY_DMA_MST_GO_PORT_OFFSET, MST_START);
		i++;
    }

}

void hardware_DMA_stuff(uint32_t *source_array, uint32_t *destination_array, uint32_t size) {
	my_dma_controller_load_registers(source_array, destination_array);
	my_dma_controller_set_size(size);
	my_dma_controller_enable_transfer();
	my_dma_controller_start_transfer();
}

int main() {
	uint8_t i = 0;
	uint32_t source_array[6] = {0xDEADBEEF, 0xDEADBEEB, 0xDEADBEAD, 0xDEADBEEA, 0xDEADBE12, 0xDEADB331};
	uint32_t source_array2[6] = {0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666};
	uint32_t destination_array[6] = {0,0,0,0,0,0};
	uint32_t destination_array2[6] = {0,0,0,0,0,0};
	//uint32_t source_word = 0xDEADBEEF; // 4 bytes is 32 bits
	//uint32_t destination_word = 0x0;

    init_platform();
    interrupt_init();

    print("Hello World\n\r");
    cleanup_platform();

    while(i < 3) {

    	xil_printf("\n i is %d\n\n\r", i);
    	//
        printf("Printing value before DMA transfer.\n\r");
        xil_printf("%x\r\n", destination_array[0]);
        xil_printf("%x\r\n", destination_array[1]);
        xil_printf("%x\r\n", destination_array[2]);
        xil_printf("%x\r\n", destination_array[3]);
        xil_printf("%x\r\n", destination_array[4]);
        xil_printf("%x\r\n", destination_array[5]);
        hardware_DMA_stuff(&source_array[0], destination_array, 20);

        printf("Printing value after DMA transfer.\n\r");
        xil_printf("%x\r\n", destination_array[0]);
        xil_printf("%x\r\n", destination_array[1]);
        xil_printf("%x\r\n", destination_array[2]);
        xil_printf("%x\r\n", destination_array[3]);
        xil_printf("%x\r\n", destination_array[4]);
        xil_printf("%x\r\n", destination_array[5]);
        //
        hardware_DMA_stuff(&source_array2[0], destination_array, 4);

        printf("Printing value of source2 after DMA transfer.\n\r");
        xil_printf("%x\r\n", destination_array[0]);
        xil_printf("%x\r\n", destination_array[1]);
        xil_printf("%x\r\n", destination_array[2]);
        xil_printf("%x\r\n", destination_array[3]);
        xil_printf("%x\r\n", destination_array[4]);
        xil_printf("%x\r\n", destination_array[5]);
        //
        //
       i++;
   	destination_array[0] = i;
   	destination_array[1] = i;
   	destination_array[2] = i;
   	destination_array[3] = i;
   	destination_array[4] = i;
   	destination_array[5] = i;

    }

//    example_of_software_solution(&source_array[0], destination_array);
//    printf("Printing value after while loop DMA transfer.\n\r");
//    xil_printf("%x\r\n", destination_array[0]);
//    xil_printf("%x\r\n", destination_array[1]);
//    xil_printf("%x\r\n", destination_array[2]);
//    xil_printf("%x\r\n", destination_array[3]);
//    xil_printf("%x\r\n", destination_array[4]);
//    xil_printf("%x\r\n", destination_array[5]);

    cleanup_platform();

    return 0;
}
