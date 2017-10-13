/*
 * HWmain.c
 *
 *  Created on: Sep 27, 2017
 *      Author: superman
 */

#include <stdio.h>
//#include "platform.h"

int input = 2;
int my_array[2048];
int my_init_array[] ={
		0, 1, 2, 3, 4
};

int factorial (int n) {
  if (n==1)
	  return 1;
  else
	  return n * factorial(n-1);
}

int main() {

	xil_printf("123456789 123456789 123456789 123456789 ");
	factorial(input);
// test
		//testing comments
	return 0;
}
