/*
 * pit.h
 *
 *  Created on: Nov 8, 2017
 *      Author: superman
 */

#ifndef PIT_H_
#define PIT_H_

#include <stdint.h>

#define COUNT_DOWN_MASK 	0x00000001
#define INT_ENABLE_MASK 	0x00000002
#define ENABLE_LOAD_MASK	0x00000004
#define LOAD_DELAY_REG_MASK 0x00000008

#define DELAY_REG_OFFSET 0x04
#define CNTRL_REG_OFFSET 0x00

/*  Initialized the pit timer by writing a 7 to the cntrl
	register. This enables the counter to read the value
	in the delay register, enables interrupts, and starts
	the counter (it starts decrementing)

	@Param none

	@Return none
*/
void pit_init();

/*  Enables interrupts for the pit timer. interrupts will happen
    for 1 clock cycle when the counter reaches 0

	@Param none

	@Return none
*/
void pit_enable_interrupts();

/*  Disables interrupts for the pit timer. interrupts will not
    happen regardless if the counter reaches 0

	@Param none

	@Return none
*/
void pit_disable_interrupts();

/*  Loads the delay register with the passed in argument. The
    counter register will read from the delay register when
    the counter register reaches 0. The bigger the delay_val,
    the less interrupts there will be.

	@Param delay_value - a 32 bit value that will be written to
						 the delay register

	@Return none
*/
void pit_load_delay_cntr(uint32_t delay_val);

#endif /* PIT_H_ */
