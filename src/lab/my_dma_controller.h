/*
 * my_dma_controller.h
 *
 *  Created on: Dec 13, 2017
 *      Author: superman
 */

#ifndef MY_DMA_CONTROLLER_H_
#define MY_DMA_CONTROLLER_H_

#include <stdint.h>

// loads the source and destination registers with the address specified
void my_dma_controller_load_registers(uint32_t source, uint32_t destination);

// sets the size of the number of bytes that will be transfered. If
// interrupts are enabled, an interrupt will happen when transfer is complete
void my_dma_controller_set_size(uint32_t size);

// enable hardware dependant transfer
void my_dma_controller_enable_transfer();

void my_dma_controller_disable_transfer();

// begin the transfer
void my_dma_controller_start_transfer();

void my_dma_controller_begin_whole_op(uint32_t source, uint32_t destination, uint32_t size);

void my_dma_controller_software_transfer(uint32_t* source, uint32_t* destination, uint32_t size);

#endif /* MY_DMA_CONTROLLER_H_ */
