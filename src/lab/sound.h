/*
 * sound.h
 *
 *  Created on: Oct 27, 2017
 *      Author: superman
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <stdint.h>

void sound_play_sounds();

void sound_codec_init();

void sound_set_vol(uint16_t volume_in);

#endif /* SOUND_H_ */
