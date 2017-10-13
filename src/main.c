#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xaxivdma.h"
#include "xio.h"
#include "time.h"
#include "unistd.h"
#include <stdint.h>
#include "bitmap.h"
#include "display.h"
#include "globals.h"
#include "control.h"



int main() {
	//init everything/////
	globals_init();
	display_init();
	display_render();
	display_wrap_up();
	//////////////////////

 	while (1) {
 		control(); //call control function for lab 3
	}
 	cleanup_platform();	//cleanup
	return 0;

}
