// Demonstrates one way to handle globals safely in C.
#include "globals.h"
#include "display.h"
#include <stdio.h>


// Here are the globals.
static uint16_t tankPosition; //tank position
static point_t tankBulletPosition; //tank bullet position
static point_t alienBlockPosition; //alien block position

//bunker block arrays containing the damage state of the bunkers
static uint8_t bunker_0_state[BUNKER_BLOCKS];
static uint8_t bunker_1_state[BUNKER_BLOCKS];
static uint8_t bunker_2_state[BUNKER_BLOCKS];
static uint8_t bunker_3_state[BUNKER_BLOCKS];

static uint8_t lives; //number of lives remaining

static uint8_t aliens_moving_direction = 1;	//direction the aliens are moving
static uint8_t aliens_hit_end = 0;	//says whether or not the aliens hit end of screen

uint8_t alien_array[NUM_ALIENS];	//contains array of aliens that says whether they are alive or not
uint8_t alien_still_alive = NUM_ALIENS;
uint8_t alien_just_died_index = INVALID_INDEX;
uint64_t alien_live_bits = 0;	//used for initialized the alien array. UNUSED?

static uint16_t max_alien_pos;	//max alien position
static uint16_t min_alien_pos;	//min alien position
static uint16_t alien_right_column_edge;	//x position of the rightmost alien column
static uint16_t alien_left_column_edge;		//x position of the leftmost alien column

static uint16_t red_guy_pos; 		//x pos of the red guy
static uint8_t red_guy_direction; 	//red guy is moving right or left
static uint8_t red_guy_death_flag = FALSE;

//indicates which of the bullets are on screen
static uint8_t tank_bullet_inflight = 0;
static uint8_t alien_bullet_0_inflight = 0;
static uint8_t alien_bullet_1_inflight = 0;
static uint8_t alien_bullet_2_inflight = 0;

//the x and y coordinates of each of the bullets
static uint16_t tankBulletPositionX;
static uint16_t tankBulletPositionY;
static uint16_t alien_bullet_0_positionX;
static uint16_t alien_bullet_0_positionY;
static uint16_t alien_bullet_1_positionX;
static uint16_t alien_bullet_1_positionY;
static uint16_t alien_bullet_2_positionX;
static uint16_t alien_bullet_2_positionY;

// the total score kept between all games
static uint16_t total_score = 0;

//boolean for whether or not all aliens are dead
static uint8_t aliens_dead = 0;

//gives an initial value to the tank and alien block
void init_pos(){
	alienBlockPosition.x = ALIEN_BLOCK_START_X;
	alienBlockPosition.y = ALIEN_BLOCK_START_Y;
	tankPosition = TANK_START_POS;
	set_aliens_dead(FALSE);
	set_red_guy_pos(OFF_SCREEN);
}

//gets the y coordinate of the lowest alien in the block
#define NUM_COLUMNS 11
#define LAST_ROW 4
#define ERROR_VAL 666
uint16_t lowest_alien_y(){
	uint8_t i,j;
	for(i = LAST_ROW;; i--){
		for(j = NUM_COLUMNS * i; j < NUM_COLUMNS * (i + 1); j++){
			if(alien_array[j] == 1){
				return get_alien_block_position().y + i * DISTANCE_BETWEEN_ALIEN_ROWS;
			}
		}
	}
	return ERROR_VAL;
}

//sets the aliens dead flag
void set_aliens_dead(uint8_t dead) {
	aliens_dead = dead;
}

//returns aliens dead flag
uint8_t get_aliens_dead() {
	return aliens_dead;
}

//sets the direction the red guy is moving
void set_red_guy_direction(uint8_t direction){
	red_guy_direction = direction;
}

//gets the direction of red guy travel
uint8_t get_red_guy_direction(){
	return red_guy_direction;
}

//returns alien direction
void set_red_guy_pos(int16_t red_guy_x) {
	red_guy_pos = red_guy_x;
}

//returns alien direction
int16_t get_red_guy_pos() {
	return red_guy_pos;
}

//sets the global flag when red_guy dies
void set_red_guy_destroyed_flag(uint8_t flag) {
	red_guy_death_flag = flag;
}

uint8_t get_red_guy_destroyed_flag() {
	return red_guy_death_flag;
}

//sets the direction of alien movement
void set_aliens_direction(uint8_t direction) {
	aliens_moving_direction = direction;
}

//returns alien direction
uint8_t get_aliens_direction() {
	return aliens_moving_direction;
}

//returns whether or not the aliens have hit the end
uint8_t get_aliens_hit_end() {
	return aliens_hit_end;
}

//set whether or not the aliens have hit the end
void set_aliens_hit_end(uint8_t hit_end) {
	aliens_hit_end = hit_end;
}

//returns the bunker block array of the given bunker
uint8_t* get_bunker(uint8_t bunker_number) {
	switch(bunker_number) {
	case 0:
		return bunker_0_state;
	case 1:
		return bunker_1_state;
	case 2:
		return bunker_2_state;
	case 3:
		return bunker_3_state;
	default:
		printf("SHOULD NOT BE HERE: INVALID BUNKER IN GET_BUNKER()\n\r");
		return 0;
	}
}

//initializes the bunker states to full health
void init_bunker_state(){
	uint8_t i = 0;
	for(i = 0; i < BUNKER_BLOCKS; i++){
		//for the two blocks in the archway of the bunker
		//initialize them at 0
		if(i == 9 || i == 10){
			bunker_0_state[i] = 0;
			bunker_1_state[i] = 0;
			bunker_2_state[i] = 0;
			bunker_3_state[i] = 0;
		}
		//else they start at full health
		else {
			bunker_0_state[i] = BUNKER_HEALTH;
			bunker_1_state[i] = BUNKER_HEALTH;
			bunker_2_state[i] = BUNKER_HEALTH;
			bunker_3_state[i] = BUNKER_HEALTH;
		}
	}
}

//gives initial values to each alien in the alien array
void init_alien_array(){
	uint8_t i = 0;
	for(i = 0; i < NUM_ALIENS; i++){
		alien_array[i] = 1;
	}
	alien_still_alive = NUM_ALIENS;

}

//calles the init functions listed above
void globals_init(){
	init_bunker_state();
	init_alien_array();
	lives = STARTING_LIVES;
	init_pos();
	total_score = 0;
	tank_bullet_inflight = 0;
	alien_bullet_0_inflight = 0;
	alien_bullet_1_inflight = 0;
	alien_bullet_2_inflight = 0;
	set_red_guy_direction(RED_GUY_RIGHT);
	alien_just_died_index = INVALID_INDEX;
}

//returns the alien array
uint8_t* get_alien_array() {
	return alien_array;
}

//returns alien lives bits
uint64_t get_alien_lives_bits() {
	return alien_live_bits;
}

//sets the position of the alien block
void set_alien_block_position(point_t point){
	alienBlockPosition.x = point.x;
	alienBlockPosition.y = point.y;
}

//get the position (top left corner) of the alien block
point_t get_alien_block_position(){
	return alienBlockPosition;
}

// Here are the accessors.

//sets the tank x position
void setTankPositionGlobal(uint16_t val) {
  tankPosition = val;
}

//gets the tank position
uint16_t getTankPositionGlobal() {
  return tankPosition;
}

//sets the tank bullet position
void setTankBulletPosition(point_t val) {
  tankBulletPosition.x = val.x;
  tankBulletPosition.y = val.y;
}

//gets the tank bullet position
point_t getTankBulletPosition() {
  return tankBulletPosition;
}

//////////////// Another way to do this without structs ////////////////

//sets the tank bullet position
void setTankBulletPositionX(uint16_t val) {tankBulletPositionX = val;}
void setTankBulletPositionY(uint16_t val) {tankBulletPositionY = val;}

//get tank bullet position
uint16_t getTankBulletPositionX(){return tankBulletPositionX;}
uint16_t getTankBulletPositionY(){return tankBulletPositionY;}

//gets lives
uint8_t getLives(){
	return lives;
}

//sets lives
void setLives(uint8_t num_lives){
	lives = num_lives;
}

//gets max alien position
uint16_t get_max_alien_pos(){
	return max_alien_pos;
}

//sets max alien position
void set_max_alien_pos(uint16_t max_pos){
	max_alien_pos = max_pos;
}

//gets minimum alien position
uint16_t get_min_alien_pos(){
	return min_alien_pos;
}

//sets minimum alien position
void set_min_alien_pos(uint16_t min_pos){
	min_alien_pos = min_pos;
}

//get alien right column x coordinate
uint16_t get_alien_right_column_edge(){
	return alien_right_column_edge;
}

//set alien right column x coordinate
void set_alien_right_column_edge(uint16_t right_col){
	alien_right_column_edge = right_col;
}

//get alien left column x coordinate
uint16_t get_alien_left_column_edge(){
	return alien_left_column_edge;
}

//set alien left column x coordinate
void set_alien_left_column_edge(uint16_t left_col){
	alien_left_column_edge = left_col;
}

//set tank bullet inflight
void set_tank_bullet_inflight(uint8_t is_inflight) {
	tank_bullet_inflight = is_inflight;
}

//get tank bullet inflight
uint8_t get_tank_bullet_inflight() {
	return tank_bullet_inflight;
}

//get alien x bullet position given a bullet number
uint16_t getAlienBulletPositionX(uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		return alien_bullet_0_positionX;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		return alien_bullet_1_positionX;
	}
	else {
		return alien_bullet_2_positionX;
	}
}

//set alien x bullet position given a bullet number
void setAlienBulletPositionX(uint16_t val, uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		alien_bullet_0_positionX = val;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		alien_bullet_1_positionX = val;
	}
	else {
		alien_bullet_2_positionX = val;
	}
}

//set alien y bullet position given a bullet number
void setAlienBulletPositionY(uint16_t val, uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		alien_bullet_0_positionY = val;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		alien_bullet_1_positionY = val;
	}
	else {
		alien_bullet_2_positionY = val;
	}
}

//get alien y bullet position given a bullet number
uint16_t getAlienBulletPositionY(uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		return alien_bullet_0_positionY;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		return alien_bullet_1_positionY;
	}
	else {
		return alien_bullet_2_positionY;
	}
}

//set alien bullet inflight
void set_alien_bullet_inflight(uint8_t is_inflight, uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		alien_bullet_0_inflight = is_inflight;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		alien_bullet_1_inflight = is_inflight;
	}
	else {
		alien_bullet_2_inflight = is_inflight;
	}
}

//get alien bullet inflight
uint8_t get_alien_bullet_inflight(uint8_t bullet_number) {
	if (bullet_number == ALIEN_BULLET_0) {
		return alien_bullet_0_inflight;
	}
	if (bullet_number == ALIEN_BULLET_1) {
		return alien_bullet_1_inflight;
	}
	else {
		return alien_bullet_2_inflight;
	}

}

// returns the number of aliens still alive
uint8_t get_aliens_still_alive() {
	return alien_still_alive;
}

// sets how many aliens should still be alive
void set_aliens_still_alive(uint8_t alive) {
	alien_still_alive = alive;
}

// get's the index of the alien that just died
uint8_t get_most_recent_alien_death() {
	return alien_just_died_index;
}

// sets the index of the alien that just died.
void set_most_recent_alien_death(uint8_t index) {
	alien_just_died_index = index;
}

//sets the score
void set_score(uint16_t score) {
	total_score = score;
}

//gets the score
uint16_t get_score() {
	return total_score;
}
