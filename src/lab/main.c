#include <stdio.h>
#include "xgpio.h"          // Provides access to PB GPIO driver.
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.
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
#include "sound.h"
#include "xac97_l.h"

#define BTN_CENTER_MASK 0x01	//mask for center button
#define BTN_RIGHT_MASK 0x02		//mask for right button
#define BTN_DOWN_MASK 0x04		//mask for down button
#define BTN_LEFT_MASK 0x08		//mask for left button
#define BTN_UP_MASK 0x10		//mask for up button
#define BTN_RIGHT_LEFT_MASK 0x0A//mask for right/left buttons

// we get interrupts 100 times a second. That's every 10ms.

XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.
uint32_t currentButtonState = 0;



// This is invoked in response to a timer interrupt.
// It does 2 things: 1) debounce switches, and 2) advances the time.
void timer_interrupt_handler() {
	game_tick(currentButtonState);
}

// This is invoked each time there is a change in the button state (result of a push or a bounce).
void pb_interrupt_handler() {
  // Clear the GPIO interrupt.
  XGpio_InterruptGlobalDisable(&gpPB);                // Turn off all PB interrupts for now.
  currentButtonState = XGpio_DiscreteRead(&gpPB, 1);  // Get the current state of the buttons.
  // You need to do something here.
  XGpio_InterruptClear(&gpPB, 0xFFFFFFFF);            // Ack the PB interrupt.
  XGpio_InterruptGlobalEnable(&gpPB);                 // Re-enable PB interrupts.
}

void sound_interrupt_handler() {
	if((currentButtonState & BTN_UP_MASK) == BTN_UP_MASK){
		if(get_volume() - 10 > AC97_VOL_MAX){
			set_volume(get_volume() - 10);
		}
	}else if((currentButtonState & BTN_DOWN_MASK) == BTN_DOWN_MASK){
		if(get_volume() + 10 < AC97_VOL_MIN){
			set_volume(get_volume() + 10);
		}
	}
	sound_set_vol(get_volume());
	sound_play_sounds();
}

// Main interrupt handler, queries the interrupt controller to see what peripheral
// fired the interrupt and then dispatches the corresponding interrupt handler.
// This routine acks the interrupt at the controller level but the peripheral
// interrupt must be ack'd by the dispatched interrupt handler.
// Question: Why is the timer_interrupt_handler() called after ack'ing the interrupt controller
// but pb_interrupt_handler() is called before ack'ing the interrupt controller?
void interrupt_handler_dispatcher(void* ptr) {
	int intc_status = XIntc_GetIntrStatus(XPAR_INTC_0_BASEADDR);
	// Check the FIT interrupt first.
	if (intc_status & XPAR_FIT_TIMER_0_INTERRUPT_MASK){
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_FIT_TIMER_0_INTERRUPT_MASK);
		timer_interrupt_handler();
	}
	// Check the push buttons.
	if (intc_status & XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK){
		pb_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK);
	}

	if (intc_status & XPAR_AXI_AC97_0_INTERRUPT_MASK) {
		sound_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_AXI_AC97_0_INTERRUPT_MASK);
	}


}

void interrupt_init() {
    init_platform();
    // Initialize the GPIO peripherals.
    int success;
    success = XGpio_Initialize(&gpPB, XPAR_PUSH_BUTTONS_5BITS_DEVICE_ID);
    // Set the push button peripheral to be inputs.
    XGpio_SetDataDirection(&gpPB, 1, 0x0000001F);
    // Enable the global GPIO interrupt for push buttons.
    XGpio_InterruptGlobalEnable(&gpPB);
    // Enable all interrupts in the push button peripheral.
    XGpio_InterruptEnable(&gpPB, 0xFFFFFFFF);

    microblaze_register_handler(interrupt_handler_dispatcher, NULL);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
    		(XPAR_FIT_TIMER_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK | XPAR_AXI_AC97_0_INTERRUPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();
}
#define MAX_VAL 2147483647
#define MAX_VAL_LESS 100000000 // roughly takes 12 seconds to get to this value. This is .12 micro seconds per count
int main() {
	//init everything/////

	display_init();
	display_wrap_up();
	sound_codec_init();
	interrupt_init();
	//////////////////////
	while (1); // loop here forever
 	cleanup_platform();	//cleanup
	return 0;
}
