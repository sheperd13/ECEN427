#include <stdio.h>
#include <math.h>
#include "blueuart.h"
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
#include "pit.h"
#include "my_dma_controller.h"
#include <xtmrctr.h>


#define VOLUME_CHANGE_INTERVAL 10


#define TIMER_0 0
#define BTN_CENTER_MASK 0x01	//mask for center button
#define BTN_RIGHT_MASK 0x02		//mask for right button
#define BTN_DOWN_MASK 0x04		//mask for down button
#define BTN_LEFT_MASK 0x08		//mask for left button
#define BTN_UP_MASK 0x10		//mask for up button
#define BTN_RIGHT_LEFT_MASK 0x0A//mask for right/left buttons
#define SIZE_RECEIVE 16			// mask used for size of UART string
#define CLK_RATE 100000000


#define PIT_DELAY_COUNTER_VAL 147483640

uint8_t dataReceived[UART_RECEIVE_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// we get interrupts 100 times a second. That's every 10ms.

XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.
XGpio gpSWITCH; // this is the handle for the switches GPIO block.
XTmrCtr xtmr0;

double seconds4Transfer = 0;

uint32_t currentButtonState = 0;
uint32_t currentSwitchState = 0;
uint32_t lastSwitchState = 0;
uint8_t dma_xfer_in_progress = 0; // this will go high when a transfer is in progress

union floatConverter {
	float floatVal;
	uint8_t floatArray[4];
};

//converts the individual bytes received into
//floats usable for moving the tank
#define DATA_INDEX_0 0
#define DATA_INDEX_1 1
#define DATA_INDEX_2 2
#define DATA_INDEX_3 3
#define DATA_INDEX_4 4
#define DATA_INDEX_5 5
#define DATA_INDEX_6 6
#define DATA_INDEX_7 7
#define DATA_INDEX_8 8
#define DATA_INDEX_9 9
void convert_to_float(uint8_t bytesGot) {

    union floatConverter unionX;	//hold x value
    union floatConverter unionY;	//hold y value

    //if not proper data format print error
	if (dataReceived[DATA_INDEX_0] != '!') {
		xil_printf("Error! Corrupted data!\n\r");
		return;
	}

	//if not proper data format print error
	if (dataReceived[DATA_INDEX_1] != 'A') {
		xil_printf("Please select Accelerometer for data transfer on phone!!\n\r");
		return;
	}

	//get the x coordinate float
	unionX.floatArray[0] = dataReceived[DATA_INDEX_2];
	unionX.floatArray[1] = dataReceived[DATA_INDEX_3];
	unionX.floatArray[2] = dataReceived[DATA_INDEX_4];
	unionX.floatArray[3] = dataReceived[DATA_INDEX_5];

	//get the y coordinate float
	unionY.floatArray[0] = dataReceived[DATA_INDEX_6];
	unionY.floatArray[1] = dataReceived[DATA_INDEX_7];
	unionY.floatArray[2] = dataReceived[DATA_INDEX_8];
	unionY.floatArray[3] = dataReceived[DATA_INDEX_9];

	 globals_setAccel_X(unionX.floatVal);
	 globals_setAccel_Y(unionY.floatVal);
}

// This is invoked in response to a timer interrupt.
// It does 2 things: 1) debounce switches, and 2) advances the time.
void timer_interrupt_handler() {
	game_tick(currentButtonState, currentSwitchState);
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

// this is invoked each time there is a change in the switch state
void switches_interrupt_handler() {
	  // Clear the GPIO interrupt.
	  XGpio_InterruptGlobalDisable(&gpSWITCH);                // Turn off all PB interrupts for now.
	  lastSwitchState = currentSwitchState;
	  currentSwitchState = XGpio_DiscreteRead(&gpSWITCH, 1);  // Get the current state of the buttons.
	  // You need to do something here.
	  XGpio_InterruptClear(&gpSWITCH, 0xFFFFFFFF);            // Ack the PB interrupt.
	  XGpio_InterruptGlobalEnable(&gpSWITCH);                 // Re-enable PB interrupts.

}

// handler for the dma controller
void dma_interrupt_handler(){
	static uint32_t time_value;
	XTmrCtr_Stop(&xtmr0, TIMER_0);
	time_value = XTmrCtr_GetValue(&xtmr0, TIMER_0);
	seconds4Transfer = (double) time_value / CLK_RATE;
	xil_printf("dma transfer took %d\r\n", time_value);
	XTmrCtr_SetResetValue(&xtmr0, TIMER_0, 0);
	XTmrCtr_Reset(&xtmr0, TIMER_0);
	my_dma_controller_disable_transfer();

	dma_xfer_in_progress = 0; // disable global flag indicating a transfer was in progress
}

//handler for ac97 interrupts
void sound_interrupt_handler() {
	if((currentButtonState & BTN_UP_MASK) == BTN_UP_MASK){
		if(get_volume() - VOLUME_CHANGE_INTERVAL > AC97_VOL_MAX){
			set_volume(get_volume() - VOLUME_CHANGE_INTERVAL);
		}
	}else if((currentButtonState & BTN_DOWN_MASK) == BTN_DOWN_MASK){
		if(get_volume() + VOLUME_CHANGE_INTERVAL < AC97_VOL_MIN){
			set_volume(get_volume() + VOLUME_CHANGE_INTERVAL);
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
	// Check the PIT interrupt first.
	if (intc_status & XPAR_PIT_0_INTERRUPT_MASK){
		timer_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_PIT_0_INTERRUPT_MASK);
	}
	// Check the push buttons.
	if (intc_status & XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK){
		pb_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK);
	}
	// Check the switches switched.
	if (intc_status & XPAR_SWITCHES_IP2INTC_IRPT_MASK){
		switches_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_SWITCHES_IP2INTC_IRPT_MASK);
	}

	if (intc_status & XPAR_AXI_AC97_0_INTERRUPT_MASK) {
		sound_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_AXI_AC97_0_INTERRUPT_MASK);
	}

	// checks to see if dma has finished transfering
	if (intc_status & XPAR_MY_DMA_0_MY_INTERRUPT_MASK) {
		dma_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_MY_DMA_0_MY_INTERRUPT_MASK);
	}
}

void interrupt_init() {
    // Initialize the GPIO peripherals.
    int success;
    success = XGpio_Initialize(&gpPB, XPAR_PUSH_BUTTONS_5BITS_DEVICE_ID);
    if (success != XST_SUCCESS) xil_printf("something went wrong init buttons\n\r");
    success = XGpio_Initialize(&gpSWITCH, XPAR_SWITCHES_DEVICE_ID);
    if (success != XST_SUCCESS) xil_printf("something went wrong init switches\n\r");
    // Set the push button peripheral to be inputs.
    XGpio_SetDataDirection(&gpPB, 1, 0x0000001F);
    XGpio_SetDataDirection(&gpSWITCH, 1, 0x000000FF);
    // Enable the global GPIO interrupt for push buttons.
    XGpio_InterruptGlobalEnable(&gpSWITCH);
    XGpio_InterruptGlobalEnable(&gpPB);
    // Enable the global GPIO interrupt for switches.


    // Enable all interrupts in the push button peripheral.
    XGpio_InterruptEnable(&gpPB, 0xFFFFFFFF);
    // Enable all interrupts in the switches peripheral.
    XGpio_InterruptEnable(&gpSWITCH, 0xFFFFFFFF);

    microblaze_register_handler(interrupt_handler_dispatcher, NULL);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
    		(XPAR_PIT_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK | XPAR_MY_DMA_0_MY_INTERRUPT_MASK
    				| XPAR_AXI_AC97_0_INTERRUPT_MASK | XPAR_SWITCHES_IP2INTC_IRPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();
}

#define ZERO_VAL 48
#define NINE_VAL 48
#define MAGNITUDE_VAL 10
void pit_timer_loop() {
	static uint32_t val = 0;
	static uint8_t count = 0;
	char c = getchar();
	xil_printf("Grabbed %d\n\r", c-'0');


	if(c < ZERO_VAL || c > NINE_VAL){
		//do math
		xil_printf("setting pit timer delay to: %d\r\n", val);
		pit_load_delay_cntr(val);
		val = 0;
		return;
	}
	c = c - '0';
	if(count > 0){
		val *= MAGNITUDE_VAL;
		val += c;
	}else{
		val += c;
	}
	count++;

}
void timer_init() {
	int success;
	success = XTmrCtr_Initialize(&xtmr0, XPAR_TMRCTR_0_DEVICE_ID);
	if (success != XST_SUCCESS) { xil_printf("it failed!"); }
}

#define BEGIN_DMA_TRANSFER 0x80
#define HARDWARE_DMA 0x40
#define SAVED_SCREEN 0x20
int main() {
	uint32_t time_value = 0;
	uint32_t* source_ptr = FRAME_BUFFER_0_ADDR;
	uint32_t* dest_ptr = FRAME_BUFFER_SAVED_ADDR;
	uint32_t last_switch_state = 0;
	//init everything/////
	display_init();
	display_wrap_up();
	sound_codec_init();
	pit_init();
	//blueuart_init_uartlite();
	//bluetooth_init();
	timer_init();
	interrupt_init();
	//pit_load_delay_cntr(PIT_DELAY_COUNTER_VAL);
	//////////////////////
	while (1){
		// check if switches are set to begin op

		if ((currentSwitchState & BEGIN_DMA_TRANSFER) && (last_switch_state != currentSwitchState)) {
			last_switch_state = currentSwitchState;
			// first check if there is no operation in progress
			if (!dma_xfer_in_progress) {
				dma_xfer_in_progress = 1; // set flag to indicate operation in progress
				if (currentSwitchState & HARDWARE_DMA ) { // if hardware only mode
					// begin the transfer
					XTmrCtr_Start(&xtmr0, 0);
					my_dma_controller_begin_whole_op(FRAME_BUFFER_0_ADDR, FRAME_BUFFER_SAVED_ADDR, BYTE_ON_SCREEN);
				}
				else {
					XTmrCtr_Start(&xtmr0, TIMER_0);
					my_dma_controller_disable_transfer();
					my_dma_controller_software_transfer(source_ptr, dest_ptr, SCREEN_HEIGHT * SCREEN_WIDTH * 4);
					XTmrCtr_Stop(&xtmr0, TIMER_0);
					time_value = XTmrCtr_GetValue(&xtmr0, TIMER_0);
					seconds4Transfer = (double) time_value / CLK_RATE;
					xil_printf("software transfer took %d\r\n", time_value);
					XTmrCtr_SetResetValue(&xtmr0, TIMER_0, 0);
					XTmrCtr_Reset(&xtmr0, TIMER_0);
					source_ptr = FRAME_BUFFER_0_ADDR;
					dest_ptr = FRAME_BUFFER_SAVED_ADDR;
					dma_xfer_in_progress = 0;

				}
			}
		}
		// check which screen to view.
		if (currentSwitchState & SAVED_SCREEN) {
			display_set_frame_buffer(SAVED_GAME_DISPLAY_BUFFER);
		}
		else {
			display_set_frame_buffer(GAME_DISPLAY_BUFFER);
		}
	}
 	cleanup_platform();	//cleanup
	return 0;
}
