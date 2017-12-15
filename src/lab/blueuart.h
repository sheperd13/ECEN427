/*
 * blueuart.h
 *
 *  Created on: Nov 21, 2017
 *      Author: superman
 */

#ifndef BLUEUART_H_
#define BLUEUART_H_

#include <xuartlite.h>
#include <xuartlite_l.h>

#define UART_RECEIVE_SIZE 16

// initializes the uartlite device. Returns whether successful or not.
int blueuart_init_uartlite();

int blueuart_read_data();

#endif /* BLUEUART_H_ */
