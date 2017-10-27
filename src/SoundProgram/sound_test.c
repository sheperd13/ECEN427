/*
 * sound_test.c
 *
 *  Created on: Oct 27, 2017
 *      Author: superman
 */

#include <stdio.h>
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xgpio.h"          // Provides access to PB GPIO driver.
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.
#include "xac97_l.h"
#include "sound.h"
#include "platform.h"
#include "xparameters.h"

extern int32_t tankFireSound[];
extern int32_t tankFireSoundFrames;


extern uint32_t fastInvader_1_soundData[];
extern uint32_t fastInvader_1_numberOfSamples;
//extern uint32_t fastInvader_2_soundData[];
//extern uint32_t fastInvader_3_soundData[];
//extern uint32_t fastInvader_4_soundData[];
//extern uint32_t invaderKilled_soundData[];
//extern uint32_t red_guy_high_soundData[];
//extern uint32_t red_guy_low_soundData[];
//extern uint32_t tankExplosion_soundData[];
//extern uint32_t tank_shoot_soundData[];




XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.
uint32_t currentButtonState = 0;



// This is invoked in response to a timer interrupt..
void timer_interrupt_handler() {

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
    		(XPAR_FIT_TIMER_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();
}

int main() {
	XAC97_HardReset(XPAR_AXI_AC97_0_BASEADDR);
	XAC97_AwaitCodecReady(XPAR_AXI_AC97_0_BASEADDR);

	XAC97_InitAudio(XPAR_AXI_AC97_0_BASEADDR, AC97_DIGITAL_LOOPBACK);
	//XAC97_mGetRegister(XPAR_AXI_AC97_0_BASEADDR, AC97_CONTROL_OFFSET);

	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_ExtendedAudioStat, AC97_EXTENDED_AUDIO_CONTROL_VRA);

	//XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);
	//interrupt_init();
	if (XAC97_isCodecReady(XPAR_AXI_AC97_0_BASEADDR)) {
	xil_printf("hi\n\r");
	}
	else {
		xil_printf("hello\n\r");
	}
	while (1) {
		XAC97_PlayAudio(XPAR_AXI_AC97_0_BASEADDR,(Xuint32) tankFireSound, (Xuint32) &tankFireSound[tankFireSoundFrames]);
		XAC97_Delay(100000);
		xil_printf("yo yo is like a\n\r");
	}
	return 0;

}
