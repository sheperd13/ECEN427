/*
 * control.c
 *
 *  Created on: Sep 29, 2017
 *      Author: superman
 */

#include "control.h"
#include "globals.h"
#include "display.h"
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#include "sound.h"
#include "xac97_l.h"

#define KILL_ALIEN_DIGITS 2 //number of digits in the aliens killed index
#define INPUT_MULTIPLIER 10 //multiplier of sorts
#define TANK_FURTHEST_LEFT_VAL 0	//leftmost position in which a tank can be

#define BTN_CENTER_MASK 0x01	//mask for center button
#define BTN_RIGHT_MASK 0x02		//mask for right button
#define BTN_DOWN_MASK 0x04		//mask for down button
#define BTN_LEFT_MASK 0x08		//mask for left button
#define BTN_UP_MASK 0x10		//mask for up button
#define BTN_RIGHT_LEFT_MASK 0x0A//mask for right/left buttons

#define ALIEN_TOP_SCORE 40		//top row value of aliens
#define ALIEN_MIDDLE_SCORE 20	//middle row value of aliens
#define ALIEN_BOTTOM_SCORE 10	//bottom row value of aliens

#define ALIEN_SPEED_INC_VAL 10	//alien speed increases by this value when an alien dies

// Change to increase/decrease speed
#define ALIEN_INIT_MAX_COUNTER_VAL 65 	// This is initial speed of aliens at start of game.
										// Increase to slow down.
#define ALIEN_BULLET_MIN_WAIT 1 // This is the minimum that alien has to wait before shooting
#define ALIEN_BULLET_FREQ 300	// Increase to get a wider range of wait times before alien shoots

#define UNIVERSAL_BULLET_SPEED_VAL 3 // Increase to slow down bullet speeds if used
#define ALIEN_BULLET_SPEED_VAL 5 // Increase to slow down alien bullet speed
#define TANK_BULLET_SPEED_VAL 5 // Increase to slow down tank bullet speed

#define TANK_MOVE_SPEED_VAL 0 	// Increase to slow down tank
#define TANK_DEAD_VAL 80		//the length of the tank death animation

#define ALIEN_EXPLOSION_VAL 10	//length of the alien explosion animation

#define RED_GUY_VAL 1500		//minimum length until the red guy appears
#define RED_GUY_FREQ 2000		//mod value to randomize the red guy appearances
#define RED_GUY_SPEED_VAL 2		//refresh rate for red guy

#define RED_GUY_SCORE_MAX 100	//length of the red guy's score on screen

extern uint32_t fastInvader_1_soundData[];
extern uint32_t fastInvader_1_numberOfSamples;

//game states
enum game_st_t {
	init_st,
	start_screen_st,
	resume_game_st,
	dead_tank_st,
	game_over_st
} currentState = init_st;

point_t alien_pos;	//alien block position

uint8_t alien_death_index = INVALID_INDEX;		//index of deceased alien
uint8_t start_alien_explosion_counter = FALSE;	//boolean to decide if we should start explosion counter
uint8_t alien_explosion_counter = 0;			//counter for alien death animation
uint8_t alien_explosion_counter_val = ALIEN_EXPLOSION_VAL;	//max counter value

uint8_t alien_speed_curr_counter_val = 0;	//how frequently aliens update on screen
uint8_t alien_speed_max_counter_val = ALIEN_INIT_MAX_COUNTER_VAL;	//max value for counter

uint16_t alien_bullet_rate_counter = 0;	//how frequently the bullets are fired
uint16_t alien_bullet_rate_counter_max = ALIEN_BULLET_MIN_WAIT;	//max value for counter

uint8_t alien_bullet_speed_counter = 0;	//how frequently the bullets are updated on screen
static uint8_t alien_bullet_speed_counter_max = UNIVERSAL_BULLET_SPEED_VAL;	//max value for counter
static uint8_t bullet_number = 0;	//current bullet number

uint8_t tank_move_speed_counter = 0;	//how frequently tank is updated on screen
uint8_t tank_dead_counter = 0;	//how long the tank is dead
uint8_t tank_lives;	//number of lives left

uint8_t red_guy_on_screen;	//boolean indicating whether of not the red guy is on screen
uint16_t red_guy_counter = 0;	//how frequently red guy updates on screen
uint16_t red_guy_max_val = RED_GUY_VAL;	//max value for counter
uint16_t red_guy_speed_curr_counter_val = 0;	//how frequently red guy appears

uint8_t start_red_guy_score_counter = FALSE;	//boolean indicating whether or not should start counter for red guy score
uint16_t red_guy_score_counter = 0;	//how long score stays on screen
uint16_t red_guy_score_val = RED_GUY_SCORE_MAX;	//max counter value
uint8_t red_guy_just_died = FALSE;	//boolean indicating red guys just died
int16_t red_guy_death_pos;	//x position of red guy's demise

uint8_t aliens_too_low = FALSE;	//boolean indicating whether or not the aliens are too low


//possible object types
enum object_type {tank, bunker, alien, saucer};

//summons red guy to screen
void summon_red_guy() {
	uint8_t direction = get_red_guy_direction();
	//only summon him if he is not already on screen
	if(!red_guy_on_screen){
		red_guy_on_screen = TRUE;
		//if he's moving right then start him on the left side of screen
		if(direction == RED_GUY_RIGHT){
			set_red_guy_pos(RED_GUY_LEFT_START);
		}else{
		//if he's moving left then start him on the right side of screen
			set_red_guy_pos(RED_GUY_RIGHT_START);
		}
	}else return;
}

#define DOUBLE_BITMAP 2
//updates the red guy coordinates on screen
void move_red_guy(){
	//if moving right then move red guy right by RED_GUY_SPEED
	int16_t cur_pos = (int16_t)get_red_guy_pos();
	uint8_t direction = get_red_guy_direction();
	//if he is already on screen
	if(red_guy_on_screen){
		//if he is moving right
		if(direction == RED_GUY_RIGHT){
			set_red_guy_pos(cur_pos + RED_GUY_SPEED);
			//if he moves off screen the set him out of the way and take him off the screen
			if(cur_pos >= SCREEN_WIDTH){
				set_red_guy_pos(OFF_SCREEN);
				set_red_guy_direction(!direction);
				red_guy_on_screen = FALSE;
			}
		}else{
		//else if he's moving left the do the opposite
			set_red_guy_pos(cur_pos - RED_GUY_SPEED);
			//if he moves off screen the set him out of the way and take him off the screen
			if((int16_t)cur_pos < 0 - RED_GUY_WIDTH * DOUBLE_BITMAP){
				set_red_guy_pos(OFF_SCREEN);
				set_red_guy_direction(!direction);
				red_guy_on_screen = FALSE;
			}
		}
	}
}

//moves the tank right by TANK_SPEED pixels
void move_tank_right() {
	uint16_t pos = getTankPositionGlobal();
	//if the next move right will move the tanks past the SCREEN_WIDTH
	//then just set the tank to max position on the screen.
	if(pos + TANK_WIDTH >= SCREEN_WIDTH){
		setTankPositionGlobal(SCREEN_WIDTH - TANK_WIDTH);
	}
	else{
		setTankPositionGlobal(pos + TANK_SPEED);
	}
}
//moves the tank left by TANK_SPEED pixels
void move_tank_left() {
	uint16_t pos = getTankPositionGlobal();
	//if the next move left will move the tank less than 0
	//then just set the tank to minimum position on the screen.
	if(pos - TANK_SPEED < TANK_FURTHEST_LEFT_VAL){
		setTankPositionGlobal(TANK_FURTHEST_LEFT_VAL);
	}
	else{
		setTankPositionGlobal(pos - TANK_SPEED);
	}
}

//moves the aliens by ALIEN_SPEED pixels either left or right
//and ALIEN_DOWN_SPEED when moving down.
void move_aliens(){
	uint8_t moving_direction = get_aliens_direction();	//get current direction of aliens
	uint8_t hit_end = get_aliens_hit_end();	//find out whether or not they are at an end position
	point_t cur_pos = get_alien_block_position();	//get the top left corner of the alien block

	if((((int16_t)cur_pos.x >= get_max_alien_pos()) || (((int16_t)cur_pos.x) <= ((int16_t)get_min_alien_pos()))) && !hit_end){
		//if the current position is greater/less than the max/min x value of alien block position and it has not yet hit
		//the end the move the alien block down.
		hit_end = TRUE;
		set_aliens_hit_end(hit_end);
		moving_direction = !moving_direction;   //change moving direction of aliens
		set_aliens_direction(moving_direction); //change moving direction of aliens
		cur_pos.y = cur_pos.y + ALIEN_DOWN_SPEED; //move the block position down
		set_alien_block_position(cur_pos);		  //move the block position down
		if(lowest_alien_y() > BUNKER_Y_POS + BUNKER_HEIGHT * DOUBLE_BITMAP){
			aliens_too_low = TRUE;
		}
	}else if(moving_direction == ALIENS_MOVING_RIGHT){
		//this one is pretty obvi. If they are moving right then move right
		hit_end = FALSE;				//reset hit_end variable
		set_aliens_hit_end(hit_end); 	//reset hit_end variable
		cur_pos.x += ALIEN_SPEED;			//move aliens to the right
		set_alien_block_position(cur_pos);  //move aliens to the right
		set_alien_right_column_edge(get_alien_right_column_edge() + ALIEN_SPEED); //move the x coordinate of the rightmost alien column
		set_alien_left_column_edge(get_alien_left_column_edge() + ALIEN_SPEED);   //move the y coordinate of the rightmost alien column
	}else{
		//else if not moving down or right then move left
		hit_end = FALSE;				//reset hit_end variable
		set_aliens_hit_end(hit_end);	//reset hit_end variable
		cur_pos.x -= ALIEN_SPEED;			//move aliens to the left
		set_alien_block_position(cur_pos);	//move aliens to the left
		set_alien_right_column_edge(get_alien_right_column_edge() - ALIEN_SPEED); //move the x coordinate of the rightmost alien column
		set_alien_left_column_edge(get_alien_left_column_edge() - ALIEN_SPEED);	  //move the y coordinate of the rightmost alien column
	}
}

//updates the position of all bullets currently on screen
#define DOUBLE_BITMAP 2
void update_bullets() {
	uint16_t posX;
	uint16_t posY;

	//if there is a tank bullet on screen then update it
	if (get_tank_bullet_inflight()) {
		posX = getTankBulletPositionX();
		posY = getTankBulletPositionY() - BULLET_SPEED;
		setTankBulletPositionY(posY);
	}
	//check for each possible alien bullet on screen and if it exists, then update it.//////////////////
	if (get_alien_bullet_inflight(ALIEN_BULLET_0)) {
		posX = getAlienBulletPositionX(ALIEN_BULLET_0);
		posY = getAlienBulletPositionY(ALIEN_BULLET_0) + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP + 1;
		setAlienBulletPositionY(getAlienBulletPositionY(ALIEN_BULLET_0) + BULLET_SPEED, ALIEN_BULLET_0);
	}
	if (get_alien_bullet_inflight(ALIEN_BULLET_1)) {
		posX = getAlienBulletPositionX(ALIEN_BULLET_1);
		posY = getAlienBulletPositionY(ALIEN_BULLET_1) + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP + 1;
		setAlienBulletPositionY(getAlienBulletPositionY(ALIEN_BULLET_1) + BULLET_SPEED, ALIEN_BULLET_1);
	}
	if (get_alien_bullet_inflight(ALIEN_BULLET_2)) {
		posX = getAlienBulletPositionX(ALIEN_BULLET_2);
		posY = getAlienBulletPositionY(ALIEN_BULLET_2) + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP + 1;
		setAlienBulletPositionY(getAlienBulletPositionY(ALIEN_BULLET_2) + BULLET_SPEED, ALIEN_BULLET_2);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
}


#define RED_GUY_INC_SIZE 50
//contains the logic for everything that happens while the game is running
void game_running(uint32_t btn) {
	// checks to see which alien just died, if any and then draws it's explosion animation
	// and increments total score

	//if the alien index is in valid range
	if (get_most_recent_alien_death() < INVALID_INDEX) {
		alien_pos = get_alien_block_position();
		alien_death_index = get_most_recent_alien_death();
		//if index is on top row
		if (alien_death_index < ROW_2) {
			set_score(get_score() + ALIEN_TOP_SCORE);
		}
		//if index is on second or third row
		else if (alien_death_index >= ROW_2 && alien_death_index < ROW_4) {
			set_score(get_score() + ALIEN_MIDDLE_SCORE);
		}
		//else it's on the bottom two rows
		else {
			set_score(get_score() + ALIEN_BOTTOM_SCORE);
		}
		//update the score every time an alien dies
		display_draw_score(get_score());
		set_most_recent_alien_death(INVALID_INDEX);	//update the recent death variable
		display_explode_alien(alien_death_index, alien_pos);	//draw death animation
		start_alien_explosion_counter = TRUE;	//start the death animation counter
		set_alien_just_died(TRUE);
	}

	// counter for the duration of the alien explosion
	//if the explosion counter maxes out then reset it and stop the counter
	if (alien_explosion_counter >= alien_explosion_counter_val) {
		display_erase_alien(alien_death_index, alien_pos);
		start_alien_explosion_counter = FALSE;
		alien_explosion_counter = 0;
	}

	// Moves and updates the aliens
	//if counter maxes then update alien position on screen
	if (alien_speed_curr_counter_val >= alien_speed_max_counter_val) {
		move_aliens();
		draw_aliens();
		set_move_alien_sound(TRUE);
		//increases alien movement speed as more of them die
		alien_speed_max_counter_val = get_aliens_still_alive() + ALIEN_SPEED_INC_VAL;
		alien_speed_curr_counter_val = 0;
	}

	// the frequency the aliens shoot their bullets
	if (alien_bullet_rate_counter >= alien_bullet_rate_counter_max) {
		if (bullet_number > ALIEN_BULLET_2) {
		//if we have the max number of alien bullets on screen, then
		//try to shoot bullet 0 again
			bullet_number = 0;
		}
		shoot_alien_bullet(bullet_number);	//shoots the bullet
		bullet_number++;
		//randomizes fire rate for alien bullets
		alien_bullet_rate_counter_max = rand() % ALIEN_BULLET_FREQ + ALIEN_BULLET_MIN_WAIT;
		alien_bullet_rate_counter = 0;
	}

	// Tank shoots
	if ((btn & BTN_CENTER_MASK) == BTN_CENTER_MASK) {
		shoot_tank_bullet();
	}

	// Moves the bullets and updates them
	if (alien_bullet_speed_counter >= alien_bullet_speed_counter_max) {
		update_bullets();
		draw_bullets();
		alien_bullet_speed_counter = 0;
	}

	//ends the program and performs cleanup routine
	if ((btn & (BTN_DOWN_MASK | BTN_UP_MASK)) == (BTN_DOWN_MASK | BTN_UP_MASK)) {
		cleanup_platform();	//cleanup
		exit(0);
	}

	//move the tank right when right button is pushed
	if ((btn & BTN_RIGHT_MASK) == BTN_RIGHT_MASK) {
		//currently TANK_MOVE_SPEED_VAL = 0 so we get warnings when this compiles
		//so it's commented out for now
		//if (tank_move_speed_counter >= TANK_MOVE_SPEED_VAL) {
			move_tank_right();
			draw_tank();
			tank_move_speed_counter = 0;
		//}
		//else {
			tank_move_speed_counter++;
		//}
	}

	//moves tank left when left button is pushed
	else if ((btn & BTN_LEFT_MASK) == BTN_LEFT_MASK) {
		//currently TANK_MOVE_SPEED_VAL = 0 so we get warnings when this compiles
		//so it's commented out for now
		//if (tank_move_speed_counter >= TANK_MOVE_SPEED_VAL) {
			move_tank_left();
			draw_tank();
			tank_move_speed_counter = 0;
		//}
		//else {
			tank_move_speed_counter++;
		//}
	}

	//if we lost a life then go to tank death animation
	if (tank_lives > getLives()) {
		set_tank_just_died(TRUE);
		currentState = dead_tank_st;
		tank_dead_counter = 0;
		tank_lives = getLives();
	}

	//if all aliens are dead the end the game
	if(get_aliens_dead()){
		set_red_guy_pos(OFF_SCREEN);
		currentState = game_over_st;
	}

	//determines when/how frequently red guy will appear
	if(red_guy_counter > red_guy_max_val){
		//randomizes red guy appearances
		red_guy_max_val = rand() % ALIEN_BULLET_FREQ + RED_GUY_VAL;
		red_guy_counter = 0;
		summon_red_guy();
		display_draw_red_guy();
	}

	//move the red guy as long as he is still alive
	if(red_guy_speed_curr_counter_val > RED_GUY_SPEED && !red_guy_just_died){
		move_red_guy();
		display_draw_red_guy();
		red_guy_speed_curr_counter_val = 0;
	}

	//if the red guy is destroyed then set things up for death animation
	if (get_red_guy_destroyed_flag()) {
		red_guy_on_screen = FALSE;
		display_erase_red_guy();	//erase red guy
		red_guy_just_died = TRUE;	//say that he just died
		set_red_guy_destroyed_flag(FALSE);	//reset destroyed flag
		red_guy_death_pos = get_red_guy_pos();	//set death position
		//if his position was to the left of the screen then set the position to 0
		if (red_guy_death_pos < 0) {
			set_red_guy_pos(0);
			red_guy_death_pos = 0;
		}
		//if he is to the right of the screen then set his death position to be SCREEN WIDTH - RED_GUY_WIDTH
		if (red_guy_death_pos > SCREEN_WIDTH - RED_GUY_WIDTH) {
			set_red_guy_pos(SCREEN_WIDTH - RED_GUY_WIDTH);
			red_guy_death_pos = SCREEN_WIDTH - RED_GUY_WIDTH;
		}

		//randomize the red guy's value
		uint16_t red_score = (RED_GUY_INC_SIZE * (rand() % INPUT_MULTIPLIER));
		//sets his minimum score to be RED_GUY_INC_SIZE
		if(red_score == 0){
			red_score = RED_GUY_INC_SIZE;
		}
		set_score(get_score() + red_score);	//change total score to reflect red guy
		display_draw_score(get_score());	//redraw score
		display_draw_red_guy_score(red_score, red_guy_death_pos, RED_GUY_Y_POS );	//draw red guy score
	}

	//if red guy score counter maxes out then reset red guy to make him available for
	//relaunch, erase the score, and set him off screen
	if (red_guy_score_counter >= red_guy_score_val) {
		red_guy_score_counter = 0;
		red_guy_just_died = FALSE;
		display_erase_red_guy();
		set_red_guy_pos(OFF_SCREEN);
	}

}

//the game state machine
void game_tick(uint32_t btn) {

	//moore machine. Actions taken in states
	switch(currentState) {
		case init_st:	//self explanatory name
			display_black_screen();	//blacks out screen
			globals_init();	//initializes global variables
			init_stuff();	//inits other stuff
			tank_lives = getLives();	//resets lives
			aliens_too_low = FALSE;	//reset aliens too low
			srand(1);	//seeds rand()
			break;
		case start_screen_st:	//waits here after initialization
			break;
		case resume_game_st:	//all gameplay is done here
			// Increments various counters
			alien_speed_curr_counter_val++;
			alien_bullet_rate_counter++;
			alien_bullet_speed_counter++;
			red_guy_counter++;
			red_guy_speed_curr_counter_val++;
			if (start_alien_explosion_counter) { // if alien is in explode state
				alien_explosion_counter++;
			}
			if (red_guy_just_died) { // if alien just died, keep score up
				red_guy_score_counter++;
			}
			break;
		case dead_tank_st:	//comes here for tank death animation
			tank_dead_counter++;
			//this flips the tank animation
			display_draw_tank_death(tank_dead_counter % 2);
			break;
		case game_over_st:	//sits here until up button pushed
			break;
		default:
			break;
	}

	//transitions/mealy
	switch(currentState) {
		case init_st:	//self explanatory name
			currentState = start_screen_st;
			break;
		case start_screen_st:	//waits here after initialization
			//if up button is pushed then start the game
				display_render();
				alien_bullet_rate_counter_max = rand() % ALIEN_BULLET_FREQ + ALIEN_BULLET_MIN_WAIT; // random value between 1-1+ALIEN_BULLET_FREQ
				currentState = resume_game_st;

			break;
		case resume_game_st:	//all gameplay done here
			//calls the tick function
			game_running(btn);
			//if the aliens are too low then end the game
			if(aliens_too_low){
				set_red_guy_pos(OFF_SCREEN);
				currentState = game_over_st;
			}
			break;
		case dead_tank_st:	//tank death animation
			if (tank_dead_counter >= TANK_DEAD_VAL) {
				tank_dead_counter = 0;
				//if we are out of lives then go to game over
				if(getLives() == 0){
					set_red_guy_pos(OFF_SCREEN);
					currentState = game_over_st;
					break;
				}
				draw_tank();
				currentState = resume_game_st;
			}
			break;
		case game_over_st:	//sits here until up button pushed to start new game
			if((btn & (BTN_LEFT_MASK | BTN_RIGHT_MASK)) == (BTN_LEFT_MASK | BTN_RIGHT_MASK)){
				set_red_guy_pos(OFF_SCREEN);
				currentState = init_st;
			}
			break;
		default:
			break;
	}

}

