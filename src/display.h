#ifndef DISPLAY_H_
#define DISPLAY_H_

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

#define WORD_WIDTH 32	//bitmap width
#define SCREEN_HEIGHT 480	//screen height
#define SCREEN_WIDTH 640	//screen width
#define FRAME_BUFFER_0_ADDR 0xC2000000  // Starting location in DDR where we will store the images that we display.
#define DISTANCE_BETWEEN_ALIEN_ROWS 30	//distance from on starting x coordinate to the next column
#define DISTANCE_BETWEEN_ALIEN_COLUMNS (ALIEN_WIDTH * 2 + 5)	//distance between starting y to next row starting y
#define DISPLAY_WHITE 0xFFFFFFFF	//white
#define DISPLAY_BLACK 0x00000000	//black
#define DISPLAY_GREEN 0x0000FF00	//green
#define DISPLAY_RED 0xFFF00000		//red
#define TOP_SCREEN_PLAYABLE_AREA (LIVES_TANK_Y_POS + TANK_HEIGHT)	//the space occupied by score and lives
#define SCORE_X_POS 7	//x pos of score text
#define SCORE_Y_POS 10	//y pos of score text
#define LIVES_X_POS 330	//x pos of lives text
#define LIVES_Y_POS 10	//y pos of lives text
#define LIVES_TANK_Y_POS 4	//y pos of lives tanks
#define LIVES_TANK_0_X 390	//x pos of lives tank 0
#define LIVES_TANK_1_X 360	//x pos of lives tank 1
#define LIVES_TANK_2_X 360	//x pos of lives tank 2
#define BOTTOM_LINE_Y 465	//y pos of bottom line
#define BUNKER_NUMBER_0 0
#define BUNKER_NUMBER_1 1
#define BUNKER_NUMBER_2 2
#define BUNKER_NUMBER_3 3
#define BUNKER_GAP 90	//gap between bunkers starting points
#define BUNKER_Y_POS 320	//bunker y pos
#define BUNKER_0_X  BUNKER_GAP									//starting point for bunker 0
#define BUNKER_1_X  (BUNKER_0_X+BUNKER_GAP+(BUNKER_WIDTH*2))	//starting point for bunker 1
#define BUNKER_2_X	(BUNKER_1_X+BUNKER_GAP+(BUNKER_WIDTH*2))	//starting point for bunker 2
#define BUNKER_3_X	(BUNKER_2_X+BUNKER_GAP+(BUNKER_WIDTH*2))	//starting point for bunker 3
#define TANK_Y_POS 415	//tank y pos
#define ALIEN_DOWN_SPEED 10	//number of pixel aliens move down
#define ALIEN_BLOCK_WIDTH (ALIEN_WIDTH * 2 * 11 + ALIEN_SPEED * 10) //width of the block of aliens
#define ALIEN_COLOR DISPLAY_WHITE	//alien color
#define ALIEN_BULLET_COLOR DISPLAY_WHITE	//alien bullet color
#define TANK_COLOR DISPLAY_GREEN	//tank color
#define TANK_BULLET_COLOR DISPLAY_GREEN	//tank bullet color
#define BUNKER_COLOR DISPLAY_GREEN	//bunker color
#define LIVES_TEXT_COLOR DISPLAY_WHITE	//lives text color
#define LIVES_TANKS_COLOR DISPLAY_GREEN	//lives tanks color
#define SCORE_TEXT_COLOR DISPLAY_WHITE	//score text color
#define SCORE_NUMBER_COLOR DISPLAY_GREEN	//score number color
#define BOTTOM_LINE_COLOR DISPLAY_GREEN	//bottom line color
#define BACKGROUND_COLOR DISPLAY_BLACK	//background color

#define RED_GUY_COLOR DISPLAY_RED
#define RED_GUY_Y_POS 35
#define RED_GUY_LEFT_START_X (SCREEN_WIDTH - RED_GUY_WIDTH)
#define RED_GUY_RIGHT_START_X 0

#define MIN_TANK_POS 0	//minimum tank position
#define MAX_TANK_POS (SCREEN_WIDTH-TANK_WIDTH-TANK_SPEED)	//maximum tank position
#define MAX_ALIEN_POS (SCREEN_WIDTH-ALIEN_BLOCK_WIDTH-ALIEN_SPEED)	//maximum alien position
#define MIN_ALIEN_POS ALIEN_SPEED	//minimum alien position
#define ALIEN_BLOCK_START_X (320 - (ALIEN_BLOCK_WIDTH/2))	//alien block starting x pos
#define ALIEN_BLOCK_START_Y 50								//alien block starting y pos

#define TANK_BULLET_HEIGHT 4	//tank bullet height
#define TANK_BULLET_WIDTH 1		//tank bullet width
#define ALIEN_BULLET_HEIGHT 5	//alien bullet height
#define ALIEN_BULLET_WIDTH 3	//alien bullet width
#define BULLET_COLOR DISPLAY_WHITE	//bullet_color

#define IS_TANK_BULLET 1


uint8_t check_bullet_collision(uint16_t x, uint16_t y, uint8_t bullet_type);

//initializes display prepping it for use
void display_init();

//kills alien at given index
void kill_alien(uint8_t index);

//erase alien at given index
uint16_t erase_alien(uint8_t alien_index);

//draw pixel at given point to the given color
void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

//returns the color at specified point
uint32_t find_pixel(uint16_t x, uint16_t y);

//fires an alien bullet. bullet number is 0, 1, or 2. representing the number of alien
//bullets on screen.
void shoot_alien_bullet(uint8_t bullet_number);

//firest a tank bullet
void shoot_tank_bullet();

//draws all bullets to screen
void draw_bullets();

//draws the tank
void draw_tank();

//degrades a whole bunker for lab 3
void degrade_bunker(uint8_t bunker_number);

//blacks out the whole screen for init purposes
void display_black_screen();

//finishes display initialization
void display_wrap_up();

//draws all aliens to screen
void draw_aliens();

//render function that draws everything to the screen
void display_render();

void erase_tank_bullet();

void erase_alien_bullet(uint8_t bullet_num);

#endif /* DISPLAY_H_ */
