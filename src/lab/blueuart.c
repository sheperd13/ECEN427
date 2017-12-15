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

// initializes the uartlite device. Returns whether successful or not.
int blueuart_init_uartlite() {

	// uart initilization
	int status = XUartLite_CfgInitialize(&bluetooth_uartInstance, &bluetooth_uartConfig, XPAR_AXI_UARTLITE_0_BASEADDR);

	if (status != XST_SUCCESS) {
		printf("Unable to initialize bluetooth UART.\n\r");
		return status;
	}


	// clear any data in the Fifo
	XUartLite_ResetFifos(&bluetooth_uartInstance);
	return status;
}

#define BYTES_RECEIVED 15	//total number of bytes received
int blueuart_read_data(uint8_t* dataReceived) {
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t buffer[UART_RECEIVE_SIZE];	//buffer to hold data received
	uint8_t bytesReceived = 0;			//index for bytes received

	//while not all bytes received, keep grabbing
	while (bytesReceived < BYTES_RECEIVED) {
		//update bytes received
		bytesReceived += XUartLite_Recv(&bluetooth_uartInstance, buffer, BYTES_RECEIVED);
		//fill up the dataReceived array with the data received from bluefruit
		for (i = j; i < bytesReceived; i++) {
			dataReceived[i] = buffer[i - j];
		}
		j = bytesReceived;
	}
	//return number of bytes received
	return bytesReceived;
}

