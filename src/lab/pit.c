/*
 * pit.c
 *
 *  Created on: Nov 8, 2017
 *      Author: superman
 */
#include "pit.h"
#include "xparameters.h"
#include "xil_io.h"
#include <stdio.h>

#define GAMEPLAY_RATE 1000000
#define START_PIT_VALUE 7

uint32_t pit_read_reg(uint32_t offset){
	return Xil_In32(XPAR_PIT_0_BASEADDR + offset);
}

void pit_write_reg(uint32_t offset, uint32_t value){
	Xil_Out32(XPAR_PIT_0_BASEADDR + offset, value);
}

//init pit
void pit_init(){
	pit_load_delay_cntr(GAMEPLAY_RATE);
	pit_write_reg(CNTRL_REG_OFFSET, START_PIT_VALUE);
}

//enable interrupts
void pit_enable_interrupts(){
	pit_write_reg(CNTRL_REG_OFFSET, INT_ENABLE_MASK);
	xil_printf("interrupt CNTRL reg has value: 0x%02x\n\r", pit_read_reg(CNTRL_REG_OFFSET));
}

//disable interrupts
void pit_disable_interrupts(){
	pit_write_reg(CNTRL_REG_OFFSET, (pit_read_reg(CNTRL_REG_OFFSET) & ~INT_ENABLE_MASK));
}

//load delay register
void pit_load_delay_cntr(uint32_t delay_val){
	pit_write_reg(DELAY_REG_OFFSET, delay_val);

}

//starts countdown
void pit_start(){
	uint32_t value = pit_read_reg(CNTRL_REG_OFFSET);
	xil_printf("start CNTRL reg has value: 0x%02x\n\r", value);
	pit_write_reg(CNTRL_REG_OFFSET, (value | COUNT_DOWN_MASK));
	value = pit_read_reg(CNTRL_REG_OFFSET);
	xil_printf("post start CNTRL reg has value: 0x%02x\n\r", value);
//	pit_write_reg(CNTRL_REG_OFFSET, (cntrl_val | LOAD_DELAY_REG_MASK | ENABLE_LOAD_MASK));
}

//stops countdown
void pit_stop(){
	pit_write_reg(CNTRL_REG_OFFSET, (pit_read_reg(CNTRL_REG_OFFSET) & ~COUNT_DOWN_MASK));
	xil_printf("init CNTRL reg has value: 0x%02x\n\r", pit_read_reg(CNTRL_REG_OFFSET));
}
