/*
 * globals.h
 *
 *  Created on: Sep 22, 2017
 *      Author: superman
 */

#ifndef globals_h
#define globals_h

#include <stdint.h>

#define TRUE 1 //boolean true
#define FALSE 0	//boolean false

#define OFF_SCREEN 1000
#define BUNKER_BLOCKS 12	//number of bunker blocks
#define NUM_ALIENS 55	//number of aliens
#define INVALID_INDEX (NUM_ALIENS + 1)
#define ALIEN_SPEED 5	//movement of aliens (pixels)
#define TANK_START_POS 312	//starting x position of tank
#define STARTING_LIVES 3	//starting lives
#define TANK_SPEED 2	//movement of tank (pixels)
#define RED_GUY_SPEED 2	//saucer movement speed
#define RED_GUY_RIGHT 1
#define RED_GUY_LEFT 0
#define BUNKER_HEALTH 4	//bunker health
#define ALIENS_MOVING_RIGHT 1	//alien right movement
#define ALIENS_MOVING_LEFT 0	//alien left movement
#define BULLET_SPEED 8	//movement of bullets (pixels)
#define ALIEN_BULLET_SPEED 10

//alien bullet numbers
#define ALIEN_BULLET_0 0
#define ALIEN_BULLET_1 1
#define ALIEN_BULLET_2 2

//initialize bunker states
void init_bunker_state();

//definition of a point
typedef struct {uint16_t x; uint16_t y;} point_t;

//returns the y position of the lowest living alien
uint16_t lowest_alien_y();

//gets red guy moving direction
void set_red_guy_direction(uint8_t direction);

//sets red guy moving direction
uint8_t get_red_guy_direction();

void set_aliens_dead(uint8_t dead);
uint8_t get_aliens_dead();

//sets saucer position
void set_red_guy_pos(uint16_t red_guy_x);

//returns red guy x position
uint16_t get_red_guy_pos();

//set alien direction
void set_aliens_direction(uint8_t direction);

//get alien direction
uint8_t get_aliens_direction();

//get alien hit end
uint8_t get_aliens_hit_end();

//set alien hit end
void set_aliens_hit_end(uint8_t hit_end);

//get alien block position
point_t get_alien_block_position();

//set alien block position
void set_alien_block_position(point_t point);

//get bunker block array of given bunker
uint8_t* get_bunker(uint8_t bunker_number);

//get alien array
uint8_t* get_alien_array();

// returns the number of aliens still alive
uint8_t get_aliens_still_alive();

// sets how many aliens should still be alive
void set_aliens_still_alive(uint8_t alive);

//get alien lives bits
uint64_t get_alien_lives_bits();

//get max alien pos
uint16_t get_max_alien_pos();

//set max alien pos
void set_max_alien_pos(uint16_t);

//get min alien pos
uint16_t get_min_alien_pos();

//set min alien pos
void set_min_alien_pos(uint16_t);

//get alien right column
uint16_t get_alien_right_column_edge();

//set alien right column
void set_alien_right_column_edge(uint16_t right_col);

//get alien left column
uint16_t get_alien_left_column_edge();

//set alien left column
void set_alien_left_column_edge(uint16_t left_col);

//calls all of ther init functions in globals.h
void globals_init();

//set tank position
void setTankPositionGlobal(uint16_t val);

//get tank position
uint16_t getTankPositionGlobal();

//set tank bullet position
void setTankBulletPosition(point_t val);

//get tank bullet position
point_t getTankBulletPosition();

//////////// Another way to do it without structs. //////////////
//set tank bullet position x
void setTankBulletPositionX(uint16_t val);
//set tank bullet position y
void setTankBulletPositionY(uint16_t val);

//get tank bullet position x
uint16_t getTankBulletPositionX();

//get tank bullet position y
uint16_t getTankBulletPositionY();

//set tank bullet inflight
void set_tank_bullet_inflight(uint8_t is_inflight);

//get tank bullet inflight
uint8_t get_tank_bullet_inflight();

//set alien bullet position x
void setAlienBulletPositionX(uint16_t val, uint8_t bullet_number);

//set alien bullet position y
void setAlienBulletPositionY(uint16_t val, uint8_t bullet_number);

//get alien bullet position x
uint16_t getAlienBulletPositionX(uint8_t bullet_number);

//get alien bullet position y
uint16_t getAlienBulletPositionY(uint8_t bullet_number);

//set alien bullet inflight
void set_alien_bullet_inflight(uint8_t is_inflight, uint8_t bullet_number);

//get alien bullet inflight
uint8_t get_alien_bullet_inflight(uint8_t bullet_number);

//get lives
uint8_t getLives();

//set lives
void setLives(uint8_t lives);

// get's the index of the alien that just died
uint8_t get_most_recent_alien_death();

// sets the index of the alien that just died.
void set_most_recent_alien_death(uint8_t index);

void set_score(uint16_t score);

uint16_t get_score();

#endif
