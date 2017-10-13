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

#define KILL_ALIEN_DIGITS 2
#define INPUT_MULTIPLIER 10
#define TANK_FURTHEST_LEFT_VAL 0

uint8_t start_from_left;
uint8_t red_guy_on_screen;

enum object_type {tank, bunker, alien, saucer};

void summon_red_guy() {
	uint16_t red_guy_pos = get_red_guy_pos();
	if (red_guy_pos == OFF_SCREEN) {
		return;
	}

	start_from_left = !start_from_left;

	if (start_from_left) {
		set_red_guy_pos(RED_GUY_LEFT_START_X);
	}
	else { // starting on the right
		set_red_guy_pos(RED_GUY_RIGHT_START_X);
	}
}

void move_red_guy(){
	//if moving right then move red guy right by RED_GUY_SPEED
	int16_t cur_pos = (int16_t)get_red_guy_pos();
	if(start_from_left){
		if(cur_pos >= SCREEN_WIDTH){
			set_red_guy_pos(OFF_SCREEN);
		}
		set_red_guy_pos(cur_pos + RED_GUY_SPEED);
	}else{
		if(cur_pos <= 0){
			set_red_guy_pos(OFF_SCREEN);
		}
		set_red_guy_pos(cur_pos - RED_GUY_SPEED);
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

//contains the switch statement required for lab 3 passoff. Should eventually
//contain the control for the gameplay
void control() {
	char input;
	char d;
	uint8_t temp;
	uint8_t index = 0;
	static uint8_t bullet_number = 0; //indicates how many alien bullets are on screen
	char k;
	input = getchar(); //get user input for switch

	//switches on input from user
	switch (input) {
	case '4':
	//moves the tank left
		if(getTankPositionGlobal() > TANK_FURTHEST_LEFT_VAL){
			move_tank_left();
			draw_tank();
		}
		break;
	case '6':
	//moves the tank right
		if(getTankPositionGlobal() < MAX_TANK_POS){
			move_tank_right();
			draw_tank();
		}
		break;
	case '8':
	//moves the aliens
		move_aliens();
		draw_aliens();
		break;
	case '2':
	//kill aliens. this will wait for a two digit input (00-54) and will
	//kill the alien on the screen that corresponds to the input value

		//for each digit input compute the integer value
		for (temp = 0; temp < KILL_ALIEN_DIGITS; temp++) {
			k = getchar();
			index = index * INPUT_MULTIPLIER + atoi(&k);
		}
		kill_alien(index);	//kills the alien
		index = 0;	//reset the index
		break;
	case '5':
	//fire a tank bullet
		shoot_tank_bullet();
		break;
	case '3':
	//fire an alien bullet
		if (bullet_number > ALIEN_BULLET_2) {
		//if we have the max number of alien bullets on screen, then
		//try to shoot bullet 0 again
			bullet_number = 0;
		}
		shoot_alien_bullet(bullet_number);	//shoots the bullet
		bullet_number++;
		break;
	case '9':
	//updates all bullets on screen by moving them according to bullet speed
		update_bullets();
		draw_bullets();
		break;
	case '7':
	//degrades a bunker by 1 value given a specific bunker index
		d = getchar();
		//switch to determine which bunker to degrade according to input
		switch(d){
		case '0':
			degrade_bunker(BUNKER_NUMBER_0);
			break;
		case '1':
			degrade_bunker(BUNKER_NUMBER_1);
			break;
		case '2':
			degrade_bunker(BUNKER_NUMBER_2);
			break;
		case '3':
			degrade_bunker(BUNKER_NUMBER_3);
			break;
		default:
			break;
		}
		break;
	case 'e':
		//exits the program and performs cleanup routine
		cleanup_platform();
		exit(0);
	default :
		break;

	}
	/*
	4 move the tank-position to the left by a constant number of pixels. This should just be the minimum amount the tank can move. Choose a value that makes sense. If you repeatedly hit this key, the tank should move smoothly across the screen.
	6 same as above, but move the tank to the right.
	8 update alien position. Each time you hit this key, the alien block should shift over as it does in the game, with the aliens switching guises on each update. Pick an x-y update value that looks similar to the game. The aliens should shift left and right, dropping down a row each time they hit the right or left side (just like the game).
	2 kill alien. This will query for a number between 0 and 54 (0 is upper left, 54 is lower right) that indicates which alien to kill. The selected alien should disappear.
	5 fire tank bullet. This sets the coordinate of the tank bullet so that it is just above the tank turret and will be drawn as if it had just fired.
	3 fire random alien missile. Randomly pick an alien from the bottom row and pick one of the available 4 alien bullet coordinates and update it so the missile is dropping from the randomly-chosen alien.
	9 update all bullets. All bullets should go up or down a constant amount each time this key is pressed. They should disappear and become available for reuse once they leave the screen.
	7 erode bunker. This will query for a number between 0 and 3 and erode the bunker by one step.
	*/
}
