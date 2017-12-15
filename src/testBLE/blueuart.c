/*
 * blueuart.c
 *
 *  Created on: Nov 21, 2017
 *      Author: superman
 */

#include "blueuart.h"
#include "xparameters.h"
#include <stdio.h>
#include <stdint.h>

#define DELAY_CONST 1000

XUartLite bluetooth_uartInstance;        // Handle to the bluetooth UART.
XUartLite_Config bluetooth_uartConfig;   // Handle to the bluetooth UART config.

int blueuart_init_uartlite() {

	// uart initilization
	//int status = XUartLite_Initialize(&bluetooth_uartInstance, XPAR_AXI_UARTLITE_0_DEVICE_ID);
	int status = XUartLite_CfgInitialize(&bluetooth_uartInstance, &bluetooth_uartConfig, XPAR_AXI_UARTLITE_0_BASEADDR);

	if (status != XST_SUCCESS) {
		printf("Unable to initialize bluetooth UART.\n\r");
		return status;
	}


	// clear any data in the Fifo
	XUartLite_ResetFifos(&bluetooth_uartInstance);
	return status;
}


int blueuart_read_data(uint8_t* dataReceived) {
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t buffer[UART_RECEIVE_SIZE];
	uint8_t bytesReceived = 0;

	while (bytesReceived < 15) {
		bytesReceived += XUartLite_Recv(&bluetooth_uartInstance, buffer, 15);
		//if (buffer[0] != '!') { continue; }
		//j = bytesReceived;
		for (i = j; i < bytesReceived; i++) {
			dataReceived[i] = buffer[i - j];
		}
		j = bytesReceived;
	}
	//XUartLite_ResetFifos(&bluetooth_uartInstance);
	return bytesReceived;
}

void blueuart_shortDelay() {
	uint16_t i;
	for (i = 0; i < DELAY_CONST; i++);
}
