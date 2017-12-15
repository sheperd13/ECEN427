/*
 * main.c
 *
 *  Created on: Nov 20, 2017
 *      Author: superman
 */

#include <xuartlite.h>
#include <xuartlite_l.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "blueuart.h"
#include "xparameters.h"
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.
#include "platform.h"


uint8_t dataReceived[UART_RECEIVE_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

union floatConverter {
	float floatVal;
	uint8_t floatArray[4];
};



void convert_to_float(uint8_t bytesGot) {

    union floatConverter unionX;
    union floatConverter unionY;

	if (dataReceived[0] != '!') {
		xil_printf("Error! Corrupted data!\n\r");
		return;
	}

	if (dataReceived[1] != 'A') {
		xil_printf("Please select Accelerometer for data transfer on phone!!\n\r");
		return;
	}


	unionX.floatArray[0] = dataReceived[2];
	unionX.floatArray[1] = dataReceived[3];
	unionX.floatArray[2] = dataReceived[4];
	unionX.floatArray[3] = dataReceived[5];

	unionY.floatArray[0] = dataReceived[6];
	unionY.floatArray[1] = dataReceived[7];
	unionY.floatArray[2] = dataReceived[8];
	unionY.floatArray[3] = dataReceived[9];

	//printf("x is : %f\n\r", unionX.floatVal);
	//printf("y is : %f\n\r", unionY.floatVal);

}

void print_data(uint8_t bytesReceived) {
	uint8_t i;
	xil_printf("bytes Received is %d\n\r", bytesReceived);
	for (i = 0; i < bytesReceived; i++) {
		xil_printf("0x%02x at index %d\n\r", dataReceived[i], i);
	}
	xil_printf("=======\n\n\r");
}

int main() {
	uint8_t UARTbytesReceived = 0;
	init_platform();
	blueuart_init_uartlite();
	while (1) {
		UARTbytesReceived = blueuart_read_data(dataReceived);
		//print_data(UARTbytesReceived);
		convert_to_float(UARTbytesReceived);
	}

 	cleanup_platform();	//cleanup
	return 0;
}
