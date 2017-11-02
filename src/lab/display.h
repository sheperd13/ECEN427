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
#define SCORE_SCORE_X (SCORE_X_POS + SCORE_WIDTH * DOUBLE_BITMAP + 2 ) // the starting point of the score numbers. 2 is small gap
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
#define ALIEN_BLOCK_HEIGHT (DISTANCE_BETWEEN_ALIEN_ROWS * 5 - 14)
#define ALIEN_COLOR DISPLAY_WHITE	//alien color
#define ALIEN_BULLET_COLOR (DISPLAY_WHITE - 1)	//alien bullet color
#define TANK_COLOR DISPLAY_GREEN	//tank color
#define TANK_BULLET_COLOR DISPLAY_GREEN	//tank bullet color
#define BUNKER_COLOR DISPLAY_GREEN	//bunker color
#define LIVES_TEXT_COLOR DISPLAY_WHITE	//lives text color
#define LIVES_TANKS_COLOR DISPLAY_GREEN	//lives tanks color
#define SCORE_TEXT_COLOR DISPLAY_WHITE	//score text color
#define SCORE_NUMBER_COLOR DISPLAY_GREEN	//score number color
#define BOTTOM_LINE_COLOR DISPLAY_GREEN	//bottom line color
#define BACKGROUND_COLOR DISPLAY_BLACK	//background color

#define RED_GUY_COLOR DISPLAY_RED	//red guy color
#define RED_GUY_Y_POS 30			//red guy y position
#define RED_GUY_LEFT_START (0 - RED_GUY_WIDTH)	//starting position of left side of screen
#define RED_GUY_RIGHT_START SCREEN_WIDTH		//starting position of right side of screen

#define MIN_TANK_POS 0	//minimum tank position
#define MAX_TANK_POS (SCREEN_WIDTH-TANK_WIDTH-TANK_SPEED)	//maximum tank position
#define MAX_ALIEN_POS (SCREEN_WIDTH-ALIEN_BLOCK_WIDTH-ALIEN_SPEED)	//maximum alien position
#define MIN_ALIEN_POS ALIEN_SPEED	//minimum alien position
#define ALIEN_BLOCK_START_X (320 - (ALIEN_BLOCK_WIDTH/2))	//alien block starting x pos
#define ALIEN_BLOCK_START_Y 60								//alien block starting y pos

#define TANK_BULLET_HEIGHT 4	//tank bullet height
#define TANK_BULLET_WIDTH 1		//tank bullet width
#define ALIEN_BULLET_HEIGHT 5	//alien bullet height
#define ALIEN_BULLET_WIDTH 3	//alien bullet width
#define BULLET_COLOR (DISPLAY_WHITE - 1)	//bullet_color. The -1 is to prevent mix-up of colliding with
											// alien_color to alien_bullet_color

//alien indices corresponding to begging of rows
#define ROW_2 11
#define ROW_3 22
#define ROW_4 33
#define ROW_5 44

#define IS_TANK_BULLET 1	//defines whether or not tank bullet

//INIT FUNCTIONS//////////////////////////////////////////////
//initializes display prepping it for use
void display_init();

//inits stuff
void init_stuff();

//finishes display initialization
void display_wrap_up();

//blacks out the whole screen for init purposes
void display_black_screen();
//END OF INIT FUNCTIONS///////////////////////////////////////



//DRAW FUNCTIONS/////////////////////////////////////////////
//draws all aliens to screen
void draw_aliens();

//draws death animation for aliens
uint16_t display_explode_alien(uint8_t alien_index, point_t curr_alien_pos);

//draw pixel at given point to the given color
void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

//draws all bullets to screen
void draw_bullets();

void display_draw_score(uint16_t score);

void display_draw_red_guy_score(uint16_t score, uint32_t x_coor, uint32_t y_coor);

void display_draw_red_guy();

//draws the tank
void draw_tank();

//draws the tank death animation
void display_draw_tank_death(uint8_t guise);

//render function that draws everything to the screen
void display_render();
//END OF DRAW FUNCTIONS///////////////////////////////////////



//ERASE FUNCTIONS/////////////////////////////////////////////
//erase alien at given index
uint16_t display_erase_alien(uint8_t alien_index, point_t curr_alien_pos);

//erase the red guy
void display_erase_red_guy();

//erase tank bullet
void erase_tank_bullet();

//erase alien bullet
void erase_alien_bullet(uint8_t bullet_num);
//END OF ERASE FUNCTIONS//////////////////////////////////////



//SUPPORT FUNCTIONS///////////////////////////////////////////
//checks what to do if a bullet has collided with something
uint8_t check_bullet_collision(uint16_t x, uint16_t y, uint8_t bullet_type);

//degrades a whole bunker for lab 3
void degrade_bunker(uint8_t bunker_number);

//degrades a given bunker block by 1 step
void degrade_bunker_block(uint8_t* bunker, uint8_t block_num);

//returns the color at specified point
uint32_t find_pixel(uint16_t x, uint16_t y);

//kills alien at given index
void kill_alien(uint8_t index);

//fires an alien bullet. bullet number is 0, 1, or 2. representing the number of alien
//bullets on screen.
void shoot_alien_bullet(uint8_t bullet_number);

//fires a tank bullet
void shoot_tank_bullet();
//END OF SUPPORT FUNCTIONS///////////////////////////////////////


#endif /* DISPLAY_H_ */
