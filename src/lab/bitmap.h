/*
 * bitmap.h
 *
 *  Created on: Sep 20, 2017
 *      Author: superman
 *
 *
 *
 *      This file holds all the macros and bitmaps for each of the objects that can be drawn on the screen.
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>

#define SCORE_HEIGHT 5	//height of score text bitmap
#define SCORE_WIDTH 30	//width of score text bitmap
#define SCORE_SCORE_WIDTH 6	//width of a digit in score bitmap
#define LIVES_HEIGHT 5	//height of lives bitmap
#define LIVES_WIDTH 24	//width of lives bitmap
#define ALIEN_HEIGHT 8	//height of alien bitmap
#define ALIEN_WIDTH 12	//width of alien bitmap
#define ALIEN_BODY_WIDTH 12	//width of alien bitmap
#define BUNKER_HEIGHT 18	//height of bunker bitmap
#define BUNKER_WIDTH 24	//width of bunker bitmap
#define TANK_HEIGHT 16	//height of tank bitmap
#define TANK_WIDTH 30	//width of tank bitmap
#define BB_HEIGHT 6	//height of bunker block bitmap
#define BB_WIDTH 6	//width of bunker block bitmap
#define RED_GUY_WIDTH 32	//width of red guy bitmap
#define RED_GUY_HEIGHT 14	//height of red guy bitmap

#define packword32(b31,b30,b29,b28,b27,b26,b25,b24,b23,b22,b21,b20,b19,b18,b17,b16,b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0) \
((b31 << 31) | (b30 << 30) | (b29 << 29) | (b28 << 28) | (b27 << 27) | (b26 << 26) | (b25 << 25) | (b24 << 24) |						  \
 (b23 << 23) | (b22 << 22) | (b21 << 21) | (b20 << 20) | (b19 << 19) | (b18 << 18) | (b17 << 17) | (b16 << 16) |	  \
 (b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |	  \
 (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )
#define packword30(b29,b28,b27,b26,b25,b24,b23,b22,b21,b20,b19,b18,b17,b16,b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0) \
((b29 << 29) | (b28 << 28) | (b27 << 27) | (b26 << 26) | (b25 << 25) | (b24 << 24) |						  \
 (b23 << 23) | (b22 << 22) | (b21 << 21) | (b20 << 20) | (b19 << 19) | (b18 << 18) | (b17 << 17) | (b16 << 16) |	  \
 (b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |	  \
 (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword24(b23,b22,b21,b20,b19,b18,b17,b16,b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)   \
	((b23 << 23) | (b22 << 22) | (b21 << 21) | (b20 << 20) | (b19 << 19) | (b18 << 18) | (b17 << 17) | (b16 << 16) |	\
	 (b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |    \
	 (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword16(b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)  \
	 ((b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |	\
	  (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword15(b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)   \
	((b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) | (b7  << 7 ) | (b6  << 6 ) |    \
	 (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword12(b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)   \
	((b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) | (b7  << 7 ) | (b6  << 6 ) |    \
	 (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword6(b5,b4,b3,b2,b1,b0)   \
	 ((b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword3(b2,b1,b0)   \
	 ((b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packword2(b1,b0)   \
	 ((b1  << 1 ) | (b0  << 0 ) )

#define packword1(b0)   \
	 (b0  << 0 )


// Must define packword for each of the different bit-widths.
//BULLETS/////////////////////////////////
static const uint32_t tank_bullet_1x4[] =
{
packword1(1),
packword1(1),
packword1(1),
packword1(1),
};

static const uint32_t cross_bullet_1_3x5[] =
{
packword3(0,1,0),
packword3(0,1,0),
packword3(0,1,0),
packword3(1,1,1),
packword3(0,1,0)
};

static const uint32_t cross_bullet_2_3x5[] =
{
packword3(0,1,0),
packword3(1,1,1),
packword3(0,1,0),
packword3(0,1,0),
packword3(0,1,0)
};

static const uint32_t zigzag_bullet_1_3x5[] =
{
packword3(0,1,0),
packword3(1,0,0),
packword3(0,1,0),
packword3(0,0,1),
packword3(0,1,0)
};

static const uint32_t zigzag_bullet_2_3x5[] =
{
packword3(0,1,0),
packword3(0,0,1),
packword3(0,1,0),
packword3(1,0,0),
packword3(0,1,0)
};
//////////////////////////////////////////

//DIGITS//////////////////////////////////
static const uint32_t zero_6x5[] =
{
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,1,0),
packword6(1,0,0,0,1,0),
packword6(1,0,0,0,1,0),
packword6(0,1,1,1,0,0)
};

static const uint32_t one_6x5[] =
{
packword6(1,0,0,0,0,0),
packword6(1,0,0,0,0,0),
packword6(1,0,0,0,0,0),
packword6(1,0,0,0,0,0),
packword6(1,0,0,0,0,0)
};

static const uint32_t two_6x5[] =
{
packword6(1,1,1,1,0,0),
packword6(0,0,0,0,1,0),
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,0,0),
packword6(1,1,1,1,1,0)
};

static const uint32_t three_6x5[] =
{
packword6(1,1,1,1,0,0),
packword6(0,0,0,0,1,0),
packword6(0,1,1,1,1,0),
packword6(0,0,0,0,1,0),
packword6(1,1,1,1,0,0)
};

static const uint32_t four_6x5[] =
{
packword6(1,0,0,1,0,0),
packword6(1,0,0,1,0,0),
packword6(1,1,1,1,1,0),
packword6(0,0,0,1,0,0),
packword6(0,0,0,1,0,0)
};

static const uint32_t five_6x5[] =
{
packword6(1,1,1,1,1,0),
packword6(1,0,0,0,0,0),
packword6(1,1,1,1,0,0),
packword6(0,0,0,0,1,0),
packword6(1,1,1,1,0,0)
};

static const uint32_t six_6x5[] =
{
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,0,0),
packword6(1,1,1,1,0,0),
packword6(1,0,0,0,1,0),
packword6(0,1,1,1,0,0)
};

static const uint32_t seven_6x5[] =
{
packword6(1,1,1,1,1,0),
packword6(0,0,0,0,1,0),
packword6(0,0,0,1,0,0),
packword6(0,0,1,0,0,0),
packword6(0,0,1,0,0,0)
};

static const uint32_t eight_6x5[] =
{
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,1,0),
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,1,0),
packword6(0,1,1,1,0,0)
};

static const uint32_t nine_6x5[] =
{
packword6(0,1,1,1,0,0),
packword6(1,0,0,0,1,0),
packword6(0,1,1,1,1,0),
packword6(0,0,0,0,1,0),
packword6(0,1,1,1,0,0)
};
/////////////////////////////////////

//TEXT///////////////////////////////
static const uint32_t score_30x5[] =
{
packword30(0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,0),
packword30(1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0),
packword30(0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,1,1,1,0,0,1,1,1,1,1,0),
packword30(0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0),
packword30(1,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,1,1,1,1,1,0)
};

static const uint32_t lives_24x5[] =
{
packword24(1,0,0,0,0,1,0,1,0,0,0,1,0,1,1,1,1,1,0,0,1,1,1,1),
packword24(1,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0),
packword24(1,0,0,0,0,1,0,1,0,0,0,1,0,1,1,1,1,1,0,0,1,1,1,0),
packword24(1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1),
packword24(1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,0,1,1,1,1,0)
};
///////////////////////////////////////

//ALIENS///////////////////////////////
static const uint32_t saucer_16x7[] =
{
packword16(0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0),
packword16(0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0),
packword16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
packword16(0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0),
packword16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword16(0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0),
packword16(0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0)
};

static const uint32_t red_guy_32x14[] =
{
packword32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0),
packword32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0),
packword32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
packword32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
packword32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
packword32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
packword32(0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0),
packword32(0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0),
packword32(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword32(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword32(0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0),
packword32(0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0),
packword32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0),
packword32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0)
};

static const uint32_t alien_explosion_12x10[] =
{
packword12(0,0,0,0,0,0,1,0,0,0,0,0),
packword12(0,0,0,1,0,0,1,0,0,0,1,0),
packword12(1,0,0,1,0,0,0,0,0,1,0,0),
packword12(0,1,0,0,1,0,0,0,1,0,0,0),
packword12(0,0,0,0,0,0,0,0,0,0,1,1),
packword12(1,1,0,0,0,0,0,0,0,0,0,0),
packword12(0,0,0,1,0,0,0,1,0,0,1,0),
packword12(0,0,1,0,0,0,0,0,1,0,0,1),
packword12(0,1,0,0,0,1,0,0,1,0,0,0),
packword12(0,0,0,0,0,1,0,0,0,0,0,0)
};

static const uint32_t alien_top_in_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,0,0,1,1,0,0,0,0,0),
packword12(0,0,0,0,1,1,1,1,0,0,0,0),
packword12(0,0,0,1,1,1,1,1,1,0,0,0),
packword12(0,0,1,1,0,1,1,0,1,1,0,0),
packword12(0,0,1,1,1,1,1,1,1,1,0,0),
packword12(0,0,0,1,0,1,1,0,1,0,0,0),
packword12(0,0,1,0,0,0,0,0,0,1,0,0),
packword12(0,0,0,1,0,0,0,0,1,0,0,0)
};

static const uint32_t alien_top_out_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,0,0,1,1,0,0,0,0,0),
packword12(0,0,0,0,1,1,1,1,0,0,0,0),
packword12(0,0,0,1,1,1,1,1,1,0,0,0),
packword12(0,0,1,1,0,1,1,0,1,1,0,0),
packword12(0,0,1,1,1,1,1,1,1,1,0,0),
packword12(0,0,0,0,1,0,0,1,0,0,0,0),
packword12(0,0,0,1,0,1,1,0,1,0,0,0),
packword12(0,0,1,0,1,0,0,1,0,1,0,0)
};

static const uint32_t alien_middle_in_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,1,0,0,0,0,0,1,0,0),
packword12(0,0,0,0,1,0,0,0,1,0,0,0),
packword12(0,0,0,1,1,1,1,1,1,1,0,0),
packword12(0,0,1,1,0,1,1,1,0,1,1,0),
packword12(0,1,1,1,1,1,1,1,1,1,1,1),
packword12(0,1,1,1,1,1,1,1,1,1,1,1),
packword12(0,1,0,1,0,0,0,0,0,1,0,1),
packword12(0,0,0,0,1,1,0,1,1,0,0,0)
};

static const uint32_t alien_middle_out_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,1,0,0,0,0,0,1,0,0),
packword12(0,1,0,0,1,0,0,0,1,0,0,1),
packword12(0,1,0,1,1,1,1,1,1,1,0,1),
packword12(0,1,1,1,0,1,1,1,0,1,1,1),
packword12(0,1,1,1,1,1,1,1,1,1,1,1),
packword12(0,0,1,1,1,1,1,1,1,1,1,0),
packword12(0,0,0,1,0,0,0,0,0,1,0,0),
packword12(0,0,1,0,0,0,0,0,0,0,1,0)
};

static const uint32_t alien_bottom_in_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,0,1,1,1,1,0,0,0,0),
packword12(0,1,1,1,1,1,1,1,1,1,1,0),
packword12(1,1,1,1,1,1,1,1,1,1,1,1),
packword12(1,1,1,0,0,1,1,0,0,1,1,1),
packword12(1,1,1,1,1,1,1,1,1,1,1,1),
packword12(0,0,1,1,1,0,0,1,1,1,0,0),
packword12(0,1,1,0,0,1,1,0,0,1,1,0),
packword12(0,0,1,1,0,0,0,0,1,1,0,0)
};

static const uint32_t alien_bottom_out_12x8[ALIEN_BODY_WIDTH] =
{
packword12(0,0,0,0,1,1,1,1,0,0,0,0),
packword12(0,1,1,1,1,1,1,1,1,1,1,0),
packword12(1,1,1,1,1,1,1,1,1,1,1,1),
packword12(1,1,1,0,0,1,1,0,0,1,1,1),
packword12(1,1,1,1,1,1,1,1,1,1,1,1),
packword12(0,0,0,1,1,0,0,1,1,0,0,0),
packword12(0,0,1,1,0,1,1,0,1,1,0,0),
packword12(1,1,0,0,0,0,0,0,0,0,1,1)
};
//////////////////////////////////////

//TANK////////////////////////////////
static const uint32_t tank_15x8[] =
{
packword15(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0),
packword15(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0),
packword15(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0),
packword15(0,1,1,1,1,1,1,1,1,1,1,1,1,1,0),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
};

static const uint32_t tank_explosion_1_15x8[] =
{
packword15(0,1,0,0,0,0,0,0,0,1,0,0,0,0,0),
packword15(1,0,1,0,0,0,1,0,0,0,0,1,0,1,0),
packword15(0,1,0,1,0,0,0,0,0,0,1,0,0,1,0),
packword15(0,0,0,0,0,1,0,1,1,0,1,0,0,0,0),
packword15(1,0,0,1,1,1,1,1,1,1,1,1,1,0,0),
packword15(0,1,1,1,1,1,1,1,1,1,1,1,1,1,0),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
};

static const uint32_t tank_explosion_2_15x8[] =
{
packword15(1,0,0,0,0,0,0,0,0,0,1,0,0,0,0),
packword15(0,0,0,1,0,0,0,0,0,0,0,0,1,0,0),
packword15(1,0,1,0,1,0,0,0,0,0,0,1,0,0,1),
packword15(0,0,0,0,0,1,1,1,1,1,1,0,0,0,0),
packword15(0,0,1,1,1,1,1,1,1,1,1,1,1,0,0),
packword15(0,1,1,1,1,1,1,1,1,1,1,1,1,1,0),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword15(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
};


static const uint32_t tank_30x16[] =
{
packword30(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
packword30(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
packword30(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword30(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
};
//////////////////////////////////////////

//BUNKER STUFF////////////////////////////
static const uint32_t bunker_24x18[] =
{
packword24(0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0),
packword24(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
packword24(0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0),
packword24(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1),
packword24(1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1)
};

// These are the blocks that comprise the bunker and each time a bullet
// strikes one of these blocks, you erode the block as you sequence through
// these patterns.
static const uint32_t bunkerDamage0_6x6[] = {
packword6(0,0,0,0,0,0),
packword6(0,0,0,0,0,0),
packword6(0,0,0,0,0,0),
packword6(0,0,0,0,0,0),
packword6(0,0,0,0,0,0),
packword6(0,0,0,0,0,0)
};

static const uint32_t bunkerDamage1_6x6[] = {
packword6(0,1,1,0,0,0),
packword6(0,0,0,0,0,1),
packword6(1,1,0,1,0,0),
packword6(1,0,0,0,0,0),
packword6(0,0,1,1,0,0),
packword6(0,0,0,0,1,0)
};

static const uint32_t bunkerDamage2_6x6[] = {
packword6(1,1,1,0,1,0),
packword6(1,0,1,0,0,1),
packword6(1,1,0,1,1,1),
packword6(1,0,0,0,0,0),
packword6(0,1,1,1,0,1),
packword6(0,1,1,0,1,0)
};

static const uint32_t bunkerDamage3_6x6[] = {
packword6(1,1,1,1,1,1),
packword6(1,0,1,1,0,1),
packword6(1,1,0,1,1,1),
packword6(1,1,0,1,1,0),
packword6(0,1,1,1,0,1),
packword6(1,1,1,1,1,1)
};

static const uint32_t bunkerDamage4_6x6[] = {
packword6(1,1,1,1,1,1),
packword6(1,1,1,1,1,1),
packword6(1,1,1,1,1,1),
packword6(1,1,1,1,1,1),
packword6(1,1,1,1,1,1),
packword6(1,1,1,1,1,1)
};
//////////////////////////////////////////////

#endif /* BITMAP_H_ */
