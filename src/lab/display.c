#include "display.h"
#include "globals.h"
#include <time.h>
#include <stdlib.h>

#define DOUBLE_BITMAP 2	//used for doubling the size of the bitmaps for drawing
#define INITIAL_SHOT_IS_TRUE 1	//value used when the shot is being fired
#define BULLET_CROSS 1	//used to indicate that an alien bullet is a cross
#define BULLET_ZIGZAG 2	//used to indicate that an alien bullet is a zigzag
#define LAST_ORIGINAL_ALIEN_ROW_INDEX 44	//alien index at the beginning of the bottom row
#define IMPOSSIBLE_COOR 65535	//ambiguous coordinate that is not possible
#define NUM_COLUMNS 11	//number of alien columns
#define SMALL_OFFSET 1
#define DEGRADE_BUNKER_BY_1 1
#define RAND_LIMIT_4_BULLET_TYPE 2
#define draw_pixel(x, y, c) framePointer0[y * SCREEN_WIDTH + x] = c;	//macro for drawing to screen
#define SCORE_ARRAY_SIZE 5
#define RESET_SCORE 0 // the score should reset to 0 whenever the game is over
#define SCORE_OFFSET_FROM_EDGE 10
#define MAX_NUM_DIGITS 10
#define INVALID_SCORE -1 // this helps keep track of the score

XAxiVdma_DmaSetup myFrameBuffer;
static uint8_t legs_in; //aliens in/out legs
XAxiVdma videoDMAController;	//part of VDMA code
unsigned int * framePointer0 = (unsigned int *) FRAME_BUFFER_0_ADDR;	//definition of frame pointer
uint8_t alien_array[NUM_ALIENS];	//global alien array to indicate if they are alive or dead
point_t curr_alien_pos = {0,0};	//global coordinates for the current alien being pointed to
uint16_t curr_tank_pos = 0;	//current tank x position
uint8_t initializing = 1;	//flag that indicates initializing
uint8_t bullet0_type = BULLET_CROSS;	//initializes the types of alien bullets to BULLET_CROSS
uint8_t bullet1_type = BULLET_CROSS;
uint8_t bullet2_type = BULLET_CROSS;

//returns the color of a given point on the screen
uint32_t find_pixel(uint16_t x, uint16_t y) {
	return framePointer0[y * SCREEN_WIDTH + x];
}

//erases the entire screen with background color. only runs at initialization.
void display_black_screen() {
	uint32_t display_width = 0;
	uint32_t display_height = 0;

	for (display_width = 0; display_width < SCREEN_WIDTH; display_width++) {
		for (display_height = 0; display_height < SCREEN_HEIGHT; display_height++) {
			draw_pixel(display_width, display_height, BACKGROUND_COLOR);
		}
	}
}

//draws the red guy on the screen
void display_draw_red_guy(){
	int16_t y = 0;
	int16_t x = 0;
	int16_t x_coor = (int16_t)get_red_guy_pos();
	int16_t y_coor = RED_GUY_Y_POS;
	uint8_t red_guy_direction = get_red_guy_direction();

	// erases the trail behind the red guy after it moves/////////////
	//if moving to the left then erase RED_GUY_SPEED to the right
	if(red_guy_direction == RED_GUY_LEFT){
		for (y = y_coor; y < y_coor + RED_GUY_HEIGHT; y++) {
			for (x = x_coor + RED_GUY_WIDTH; x < x_coor + RED_GUY_WIDTH + RED_GUY_SPEED; x++) {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
	//else if  moving to the right then erase RED_GUY_SPEED to the left
	else{
		for (y = y_coor; y < y_coor + RED_GUY_HEIGHT; y++) {
			for (x = x_coor - RED_GUY_SPEED; x < x_coor; x++) {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
	//////////////////////////////////////////////////////////////////

	// draws the red guy
	for (y = y_coor; y < y_coor + RED_GUY_HEIGHT; y++) {
		for (x = x_coor; x < x_coor + RED_GUY_WIDTH; x++) {
			if(x > 0 && x < SCREEN_WIDTH){
				if ((red_guy_32x14[(y - y_coor)] & (1 << (RED_GUY_WIDTH - SMALL_OFFSET
						- ((x - x_coor)))))) {
					//should gradually draw him in
					if(find_pixel(x,y) != RED_GUY_COLOR){
						draw_pixel(x, y, RED_GUY_COLOR);
					}
				} else {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
	}
}

void display_erase_red_guy(){
	uint16_t y = 0;
	int16_t x = 0;
	int16_t alien_x = get_red_guy_pos();
	uint16_t alien_y = RED_GUY_Y_POS;

	//draws black over the red guy that was killed
	for (y = alien_y; y < alien_y + RED_GUY_HEIGHT; y++) {
		for (x = alien_x; x < alien_x + RED_GUY_WIDTH; x++) {
			draw_pixel(x, y, BACKGROUND_COLOR);
		}
	}
}

//draws the scoreboard text bitmap
void draw_scoreboard_text() {
	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor = SCORE_X_POS;
	uint32_t y_coor = SCORE_Y_POS;

	for (y = y_coor; y < y_coor + SCORE_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = x_coor; x < x_coor + SCORE_WIDTH * DOUBLE_BITMAP; x++) {
			if ((score_30x5[(y - y_coor) / DOUBLE_BITMAP] & (1 << (SCORE_WIDTH - SMALL_OFFSET - ((x
					- x_coor) / DOUBLE_BITMAP))))) {
			//if the bitmap is 1 then draw
				draw_pixel(x, y, SCORE_TEXT_COLOR);
			} else {
			//else draw background color
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//draws the lives text bitmap
void draw_lives_text() {
	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor = LIVES_X_POS;
	uint32_t y_coor = LIVES_Y_POS;

	for (y = y_coor; y < y_coor + LIVES_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = x_coor; x < x_coor + LIVES_WIDTH * DOUBLE_BITMAP; x++) {
			if ((lives_24x5[(y - y_coor) / DOUBLE_BITMAP] & (1 << (LIVES_WIDTH - SMALL_OFFSET - ((x
					- x_coor) / DOUBLE_BITMAP))))) {
			//if the bitmap is 1 then draw
				draw_pixel(x, y, LIVES_TEXT_COLOR);
			} else {
			//else draw background color
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//draws the line across the bottom of the screen
void draw_bottom_line() {
	uint16_t x = 0;
	uint16_t y = BOTTOM_LINE_Y - SMALL_OFFSET;
	for(; y < BOTTOM_LINE_Y + SMALL_OFFSET; y++){
		for (x = 0; x < SCREEN_WIDTH; x++) {
			draw_pixel(x, y, BOTTOM_LINE_COLOR);
		}
	}
}

//random stuff that needs initialization but didn't seem to fit anywhere else
void init_stuff(){
	curr_tank_pos = getTankPositionGlobal();
	set_max_alien_pos(SCREEN_WIDTH-ALIEN_BLOCK_WIDTH-ALIEN_SPEED);
	set_min_alien_pos(ALIEN_SPEED);
	point_t block_pos = get_alien_block_position();
	set_alien_right_column_edge(block_pos.x + ALIEN_BLOCK_WIDTH - ALIEN_WIDTH*DOUBLE_BITMAP);
	set_alien_left_column_edge(block_pos.x);
}

//initializes the display and preps it for use. It also gives values to the display globals.
//most of this code is from the VDMA test provided in lab materials
void display_init() {
	init_platform(); // Necessary for all programs.
	int Status; // Keep track of success/failure of system function calls.
	// There are 3 steps to initializing the vdma driver and IP.
	// Step 1: lookup the memory structure that is used to access the vdma driver.
	XAxiVdma_Config * VideoDMAConfig = XAxiVdma_LookupConfig(
			XPAR_AXI_VDMA_0_DEVICE_ID);
	// Step 2: Initialize the memory structure and the hardware.
	if (XST_FAILURE == XAxiVdma_CfgInitialize(&videoDMAController,
			VideoDMAConfig, XPAR_AXI_VDMA_0_BASEADDR)) {
		xil_printf("VideoDMA Did not initialize.\r\n");
	}
	// Step 3: (optional) set the frame store number.
	if (XST_FAILURE == XAxiVdma_SetFrmStore(&videoDMAController, 2,
			XAXIVDMA_READ)) {
		xil_printf("Set Frame Store Failed.");
	}
	// Initialization is complete at this point.

	// Setup the frame counter. We want two read frames. We don't need any write frames but the
	// function generates an error if you set the write frame count to 0. We set it to 2
	// but ignore it because we don't need a write channel at all.
	XAxiVdma_FrameCounter myFrameConfig;
	myFrameConfig.ReadFrameCount = 2;
	myFrameConfig.ReadDelayTimerCount = 10;
	myFrameConfig.WriteFrameCount = 2;
	myFrameConfig.WriteDelayTimerCount = 10;
	Status = XAxiVdma_SetFrameCounter(&videoDMAController, &myFrameConfig);
	if (Status != XST_SUCCESS) {
		xil_printf("Set frame counter failed %d\r\n", Status);
		if (Status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("DMA Mismatch Error\r\n");
	}
	// Now we tell the driver about the geometry of our frame buffer and a few other things.
	// Our image is 480 x 640.
	XAxiVdma_DmaSetup myFrameBuffer;
	myFrameBuffer.VertSizeInput = 480; // 480 vertical pixels.
	myFrameBuffer.HoriSizeInput = 640 * 4; // 640 horizontal (32-bit pixels).
	myFrameBuffer.Stride = 640 * 4; // Dont' worry about the rest of the values.
	myFrameBuffer.FrameDelay = 0;
	myFrameBuffer.EnableCircularBuf = 1;
	myFrameBuffer.EnableSync = 0;
	myFrameBuffer.PointNum = 0;
	myFrameBuffer.EnableFrameCounter = 0;
	myFrameBuffer.FixedFrameStoreAddr = 0;
	if (XST_FAILURE == XAxiVdma_DmaConfig(&videoDMAController, XAXIVDMA_READ,
			&myFrameBuffer)) {
		xil_printf("DMA Config Failed\r\n");
	}
	// We need to give the frame buffer pointers to the memory that it will use. This memory
	// is where you will write your video data. The vdma IP/driver then streams it to the HDMI
	// IP.
	myFrameBuffer.FrameStoreStartAddr[0] = FRAME_BUFFER_0_ADDR;
	myFrameBuffer.FrameStoreStartAddr[1] = FRAME_BUFFER_SAVED_ADDR;

	if (XST_FAILURE == XAxiVdma_DmaSetBufferAddr(&videoDMAController,
			XAXIVDMA_READ, myFrameBuffer.FrameStoreStartAddr)) {
		xil_printf("DMA Set Address Failed Failed\r\n");
	}
}

void display_set_frame_buffer(uint8_t isGameScreen) {
	static uint8_t last_buffer = GAME_DISPLAY_BUFFER;
	if (last_buffer == isGameScreen) {
		return;
	}
	last_buffer = isGameScreen;

	if (isGameScreen) {
		if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, 0, XAXIVDMA_READ)) {
			xil_printf("DMA Set Address Failed Failed\r\n");
		}
	}
	else {
		if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, 1, XAXIVDMA_READ)) {
			xil_printf("DMA Set Address Failed Failed\r\n");
		}
	}

}

// grabs each of the digits in score and places inside score_array in reverse order.
// the reason for reverse is that "score mod 10" takes the least significant digit
// first.
void grab_digits(int8_t score_array[SCORE_ARRAY_SIZE], uint16_t score) {
	int8_t score_index = SCORE_ARRAY_SIZE - SMALL_OFFSET; // the starting index. It's the highest index
	uint8_t digit = 0;
	uint8_t score_is_zero = TRUE;
	while (score || score_index == -1) { // while score is not 0 or score_index dips below 0
		digit = score % MAX_NUM_DIGITS; // grab the least sig digit
		score_array[score_index] = digit; // place it inside array
		score_index--;	// lower index
		score /= MAX_NUM_DIGITS; // divid by 10. So 453 will become 45 (notice, 3 was grabed already)
		score_is_zero = FALSE;	// flag to let us know if score is not 0
	}
	if (score_is_zero) { // if score is 0, we still have to draw it, so put that into score_array
		score_array[score_index] = score;
	}

}

// draws the values onto the screen specified by the parameters
void display_draw_score(uint16_t score) {
	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor = SCORE_SCORE_X + SCORE_SCORE_WIDTH; // this is starting location of score post
	uint32_t y_coor = SCORE_Y_POS;						 // this is starting y pos
	uint8_t i = 0;
	int8_t digit;	// will save each of the digits
	uint8_t want_to_draw_bit = 0;
	int8_t score_array[SCORE_ARRAY_SIZE] = {-1, -1, -1, -1, -1}; // init to all invalid score
	uint32_t* bitmap;

	// save each of the digits of the score individually
	grab_digits(score_array, score);

	// loop through the score array
	for (i = 0; i < SCORE_ARRAY_SIZE; i++) {
		// save the digit into a variable
		digit = score_array[i];

		switch (digit) { // see which digit it is
		case 0:
			bitmap = (uint32_t*) zero_6x5;	// if digit is 0, get the right bitmap
			break;
		case 1:
			bitmap = (uint32_t*) one_6x5;	// if digit is 1, get the right bitmap
			break;
		case 2:
			bitmap = (uint32_t*) two_6x5;	// if digit is 2, get the right bitmap
			break;
		case 3:
			bitmap = (uint32_t*) three_6x5;	// if digit is 3, get the right bitmap
			break;
		case 4:
			bitmap = (uint32_t*) four_6x5;	// if digit is 4, get the right bitmap
			break;
		case 5:
			bitmap = (uint32_t*) five_6x5;	// if digit is 5, get the right bitmap
			break;
		case 6:
			bitmap = (uint32_t*) six_6x5;	// if digit is 6, get the right bitmap
			break;
		case 7:
			bitmap = (uint32_t*) seven_6x5;	// if digit is 7, get the right bitmap
			break;
		case 8:
			bitmap = (uint32_t*) eight_6x5;	// if digit is 8, get the right bitmap
			break;
		case 9:
			bitmap = (uint32_t*) nine_6x5;	// if digit is 9, get the right bitmap
			break;
		default:
			continue;
		}

		// after digit bitmap is found, we'll draw it
		for (y = y_coor; y < y_coor + SCORE_HEIGHT * DOUBLE_BITMAP; y++) { // loops through y coord
			for (x = x_coor; x < x_coor + SCORE_SCORE_WIDTH * DOUBLE_BITMAP; x++) { // loops through x coord
				want_to_draw_bit = (bitmap[(y - y_coor) / DOUBLE_BITMAP] & (1 << (SCORE_SCORE_WIDTH - SMALL_OFFSET - ((x - x_coor) / DOUBLE_BITMAP))));
				if (want_to_draw_bit) {
					draw_pixel(x, y, DISPLAY_GREEN);
				}
				else {
					draw_pixel(x, y, DISPLAY_BLACK);
				}
			}
		}
		x_coor += SCORE_SCORE_WIDTH * DOUBLE_BITMAP; // move x_coor right so we can move to the corrent spot on the screen
	}
}

// draws the values onto the screen specified by the parameters
void display_draw_red_guy_score(uint16_t score, uint32_t x_coor, uint32_t y_coor) {
	x_coor += SCORE_OFFSET_FROM_EDGE; // needed to keep from drawing out of the screen
	uint32_t y = 0;
	uint32_t x = 0;
	uint8_t i = 0;
	int8_t digit;
	uint8_t want_to_draw_bit = 0;
	int8_t score_array[SCORE_ARRAY_SIZE] = {INVALID_SCORE, INVALID_SCORE, INVALID_SCORE, INVALID_SCORE, INVALID_SCORE}; // init to all invalid score
	uint32_t* bitmap;


	grab_digits(score_array, score);

	for (i = 0; i < SCORE_ARRAY_SIZE; i++) {
		digit = score_array[i];

		switch (digit) { // see which digit it is
		case 0:
			bitmap = (uint32_t*) zero_6x5;	// if digit is 0, get the right bitmap
			break;
		case 1:
			bitmap = (uint32_t*) one_6x5;	// if digit is 1, get the right bitmap
			break;
		case 2:
			bitmap = (uint32_t*) two_6x5;	// if digit is 2, get the right bitmap
			break;
		case 3:
			bitmap = (uint32_t*) three_6x5;	// if digit is 3, get the right bitmap
			break;
		case 4:
			bitmap = (uint32_t*) four_6x5;	// if digit is 4, get the right bitmap
			break;
		case 5:
			bitmap = (uint32_t*) five_6x5;	// if digit is 5, get the right bitmap
			break;
		case 6:
			bitmap = (uint32_t*) six_6x5;	// if digit is 6, get the right bitmap
			break;
		case 7:
			bitmap = (uint32_t*) seven_6x5;	// if digit is 7, get the right bitmap
			break;
		case 8:
			bitmap = (uint32_t*) eight_6x5;	// if digit is 8, get the right bitmap
			break;
		case 9:
			bitmap = (uint32_t*) nine_6x5;	// if digit is 9, get the right bitmap
			break;
		default:
			continue;						// this digit must be -1
		}
		// loop through drawing the bitmap that was chosen
		for (y = y_coor; y < y_coor + SCORE_HEIGHT; y++) {
			for (x = x_coor; x < x_coor + SCORE_SCORE_WIDTH; x++) {
				want_to_draw_bit = (bitmap[(y - y_coor)] & (1 << (SCORE_SCORE_WIDTH - SMALL_OFFSET - ((x - x_coor)))));
				if (want_to_draw_bit) {
					draw_pixel(x, y, DISPLAY_WHITE);
				}
				else {
					draw_pixel(x, y, DISPLAY_BLACK); // draws black
				}
			}
		}
		x_coor += SCORE_SCORE_WIDTH; // for the next loop through, we need to move to the next digit place
	}
}

//finishes initialization. kind of a misnomer, but don't really want to change it.
//This is also provided in the VDMA test.
void display_wrap_up() {
	// This tells the HDMI controller the resolution of your display (there must be a better way to do this).
	XIo_Out32(XPAR_AXI_HDMI_0_BASEADDR, 640 * 480);

	// Start the DMA for the read channel only.
	if (XST_FAILURE == XAxiVdma_DmaStart(&videoDMAController, XAXIVDMA_READ)) {
		xil_printf("DMA START FAILED\r\n");
	}
	int frameIndex = 0;
	// We have two frames, let's park on frame 0. Use frameIndex to index them.
	// Note that you have to start the DMA process before parking on a frame.
	if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, frameIndex,
			XAXIVDMA_READ)) {
		xil_printf("vdma parking failed\n\r");
	}
}

//draws an individual alien given it's coordinates and the alien array that indicates
//whether or not the alien is alive
void draw_alien(point_t curr_alien_pos, uint32_t* alien) {
	uint16_t y = 0;
	uint16_t x = 0;
	uint16_t alien_x = curr_alien_pos.x;	//get the x pos of the alien
	uint16_t alien_y = curr_alien_pos.y;	//get the y pos of the alien
	uint16_t alien_prev_x = alien_x;	//set a prev pos for comparison
	uint16_t alien_prev_y = alien_y;	//set a prev pos for comparison

	//find out which direction they are moving so we know which part of the
	//alien we will need to erase after they have moved
	uint8_t hit_end = get_aliens_hit_end();
	uint8_t moving_direction = get_aliens_direction();

	uint16_t want_to_draw_bit; 	//value of the bitmap at a certain point

//ERASES THE LEFTOVERS OF ALIENS AFTER THEY HAVE MOVED/////////////////////////////////////////////////
	//if the aliens moved down then we need to erase the top of the given alien
	if (hit_end) {
		alien_prev_y -= ALIEN_DOWN_SPEED;	//move the prev_y coordinate up by ALIEN_DOWN_SPEED to know where to start erasing
		for (y = alien_prev_y; y < alien_y + ALIEN_DOWN_SPEED; y++) {
			for (x = alien_x; x < alien_x + (ALIEN_WIDTH * DOUBLE_BITMAP); x++) {
				if (find_pixel(x, y) == ALIEN_COLOR) {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
	}
	//if the aliens moved right then we need to erase the left of the given alien
	else if (moving_direction == ALIENS_MOVING_RIGHT) {
		alien_prev_x -= ALIEN_SPEED;	//move the prev_x coordinate left by ALIEN_SPEED to know where to start erasing
		for (y = alien_y; y < alien_y + (ALIEN_HEIGHT * DOUBLE_BITMAP); y++) {
			for (x = alien_prev_x; x < alien_x + ALIEN_BODY_WIDTH * DOUBLE_BITMAP; x++) {
				want_to_draw_bit = (alien[(y - alien_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BODY_WIDTH - SMALL_OFFSET - ((x - alien_x) / DOUBLE_BITMAP))));
				if (x < alien_x) {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
	}
	//if the aliens moved left then we need to erase the right of the given alien
	else {
		uint16_t x_coor_end = alien_x + (ALIEN_WIDTH * DOUBLE_BITMAP) + ALIEN_SPEED;
		uint16_t erase_alien_x = alien_x;
		erase_alien_x += (ALIEN_WIDTH * DOUBLE_BITMAP);
		for (y = alien_y; y < alien_y + (ALIEN_HEIGHT * DOUBLE_BITMAP); y++) {
			for (x = erase_alien_x; x < x_coor_end; x++) {
				if (find_pixel(x, y) == ALIEN_COLOR) {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////

	//draws the alien where it needs to be
	for (y = alien_y; y < alien_y + ALIEN_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = alien_x; x < alien_x + ALIEN_BODY_WIDTH * DOUBLE_BITMAP; x++) {
			want_to_draw_bit = (alien[(y - alien_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BODY_WIDTH - SMALL_OFFSET - ((x - alien_x) / DOUBLE_BITMAP))));
			if (want_to_draw_bit) {
				draw_pixel(x, y, ALIEN_COLOR);
			} else {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//only gets called when an alien is killed. it detects if the killed alien was on either the
//left or right edge, then check the rest of the edge for other aliens. if no aliens are found
//on the edge then it will recursively check edges until it finds an alien while adjusting the
//edges inwards
void update_alien_edge(uint16_t alien_column, uint8_t index, uint8_t* alien_array) {
	uint8_t i;
	uint8_t alien_found = 0;	//flag stating whether or not an alien has been found on edge
	uint8_t edge_is_gone = 1;	//flag stating that the current edge is gone
	uint16_t right_edge = get_alien_right_column_edge();	//get current x coor of right edge
	uint16_t left_edge = get_alien_left_column_edge();		//get current x coor of left edge
	//this while recursively checks for aliens on the edges
	while(!alien_found){
		if (alien_column == right_edge) { // if alien died at the right edge
			uint8_t alien_starting_number = index % NUM_COLUMNS; //goes to top of the column of the dead alien
			//for each alien in the column, check if they are alive. if any of them are alive, then
			//alien found is true and edge_is_gone is false.
			for (i = 0; i < NUM_ALIENS; i+=NUM_COLUMNS) {
				if (alien_array[alien_starting_number + i] == 1) {
					alien_found = 1;
					edge_is_gone = 0;
					break;
				}
			}
			if(edge_is_gone){
			//if the whole edge is gone then move the right edge to the left by DISTANCE_BETWEEN_ALIEN_COLUMNS
			//increase the max_alien_pos by DISTANCE_BETWEEN_ALIEN_COLUMNS, get ready to run the check again by
			//decrementing alien_column by DISTANCE_BETWEEN_ALIEN_COLUMNS, and decrementing alien index
				set_alien_right_column_edge(right_edge - DISTANCE_BETWEEN_ALIEN_COLUMNS);
				set_max_alien_pos(get_max_alien_pos() + DISTANCE_BETWEEN_ALIEN_COLUMNS);
				alien_column -= DISTANCE_BETWEEN_ALIEN_COLUMNS;
				right_edge = get_alien_right_column_edge();
				//if the edges pass each other then we know that there are no aliens left
				//we could have a global next_level variable that will be set to true here.
				if(right_edge < left_edge){
					set_aliens_dead(TRUE);
					break;
				}
				index--;
			}
		}else if(alien_column == left_edge){ //else if alien died at the left edge
			uint8_t alien_starting_number = index % NUM_COLUMNS;
			//for each alien in the column, check if they are alive. if any of them are alive, then
			//alien found is true and edge_is_gone is false.
			for (i = 0; i < NUM_ALIENS; i+=NUM_COLUMNS) {
				if (alien_array[alien_starting_number + i] == 1) {
					alien_found = 1;
					edge_is_gone = 0;
					break;
				}
			}
			if(edge_is_gone){
			//if the whole edge is gone then move the left edge to the right by DISTANCE_BETWEEN_ALIEN_COLUMNS
			//decrease the min_alien_pos by DISTANCE_BETWEEN_ALIEN_COLUMNS, get ready to run the check again by
			//incrementing alien_column by DISTANCE_BETWEEN_ALIEN_COLUMNS, and incrementing alien index
				set_alien_left_column_edge(left_edge + DISTANCE_BETWEEN_ALIEN_COLUMNS);
				set_min_alien_pos(get_min_alien_pos() - DISTANCE_BETWEEN_ALIEN_COLUMNS);
				alien_column += DISTANCE_BETWEEN_ALIEN_COLUMNS;
				left_edge = get_alien_left_column_edge();
				//if the edges pass each other then we know that there are no aliens left
				//we could have a global next_level variable that will be set to true here.
				if(right_edge < left_edge){
					set_aliens_dead(TRUE);
					break;
				}
				index++;
			}
		}else{
			break;
		}
	}
}

//kills an alien at a given index
void kill_alien(uint8_t index) {
	uint16_t alien_column;
	uint8_t* alien_array = get_alien_array();
	//bounds checking for input to make sure input is valid and
	//hasn't already been killed
	if (index >= NUM_ALIENS || alien_array[index] == 0) {return;}
	alien_array[index] = 0;

	alien_column = display_erase_alien(index, get_alien_block_position());	//erase the alien that was killed
	update_alien_edge(alien_column, index, alien_array);	//update the edges
	set_aliens_still_alive(get_aliens_still_alive() - 1); // set global variable to one less then what it was
}

//#defines for erase_alien()///////////
#define ROW_3_MULT 2
#define ROW_4_MULT 3
#define ROW_5_MULT 4
///////////////////////////////////////
uint16_t calculate_alineY_pos(uint16_t alien_y, uint8_t alien_index) {
	if(alien_index >= ROW_2 && alien_index < ROW_3) {
	//if the alien is on the second row move y coord accordingly
		alien_y += DISTANCE_BETWEEN_ALIEN_ROWS;
	}
	else if(alien_index >= ROW_3 && alien_index < ROW_4) {
	//if the alien is on the third row move y coord accordingly
		alien_y += DISTANCE_BETWEEN_ALIEN_ROWS * ROW_3_MULT;
	}
	else if(alien_index >= ROW_4 && alien_index < ROW_5) {
	//if the alien is on the fourth row move y coord accordingly
		alien_y += DISTANCE_BETWEEN_ALIEN_ROWS * ROW_4_MULT;
	}
	else if(alien_index >= ROW_5) {
	//if the alien is on the fifth row move y coord accordingly
		alien_y += DISTANCE_BETWEEN_ALIEN_ROWS * ROW_5_MULT;
	}
	return alien_y;
}

//erases the alien at the given index
uint16_t display_erase_alien(uint8_t alien_index, point_t erase_alien_pos) {
	uint16_t y = 0;
	uint16_t x = 0;
	uint16_t alien_x = erase_alien_pos.x + (alien_index % NUM_COLUMNS) * DISTANCE_BETWEEN_ALIEN_COLUMNS;
	uint16_t alien_y = erase_alien_pos.y;
	//xil_printf("erasing alien at pos (%d, %d)\n\r", alien_x, alien_y);

	alien_y = calculate_alineY_pos(alien_y, alien_index);

	//draws black over the alien that was killed
	for (y = alien_y; y < alien_y + ALIEN_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = alien_x; x < alien_x + ALIEN_BODY_WIDTH * DOUBLE_BITMAP; x++) {
			draw_pixel(x, y, BACKGROUND_COLOR);
		}
	}
	return alien_x;
}

//draws the death animation for aliens
uint16_t display_explode_alien(uint8_t alien_index, point_t explode_alien_pos) {

	uint16_t y = 0; // always init to 0
	uint16_t x = 0; // always init to 0
	uint16_t alien_x = explode_alien_pos.x + (alien_index % NUM_COLUMNS) * DISTANCE_BETWEEN_ALIEN_COLUMNS;
	uint16_t alien_y = explode_alien_pos.y;
	uint16_t want_to_draw_bit = 0;

	alien_y = calculate_alineY_pos(alien_y, alien_index);

	//draws black over the alien that was killed
	for (y = alien_y; y < alien_y + ALIEN_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = alien_x; x < alien_x + ALIEN_BODY_WIDTH * DOUBLE_BITMAP; x++) {
			want_to_draw_bit = (alien_explosion_12x10[(y - alien_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BODY_WIDTH - SMALL_OFFSET - ((x - alien_x) / DOUBLE_BITMAP))));
			if (want_to_draw_bit) {
				draw_pixel(x, y, DISPLAY_WHITE);
			}
		}
	}
	return alien_x;
}

//draws all of the aliens to the screen
void draw_aliens() {

	curr_alien_pos = get_alien_block_position();
	point_t temp_pos = curr_alien_pos;
	uint8_t i = 0;
	uint8_t* alien_array;
	alien_array = get_alien_array(); // get array of the aliens

	legs_in = !legs_in;
	//if we are drawing legs in
	if (legs_in) {
		for (i = 0; i < NUM_ALIENS; i++) {
			//for the first row of aliens
			if (i < ROW_2) { // draw first row
				if (alien_array[i]) { // check to see if alien is alive and draw accordingly
					draw_alien(temp_pos, (uint32_t *)alien_top_in_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS; //increment x pos for each alien
			}
			//for the second/third row of aliens
			if (i >= ROW_2 && i < ROW_4) {
				//if you get to the end of the row, then reset x pos and increment y pos
				if (i == ROW_2) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}

				//if the alien is alive then draw the middle alien
				if (alien_array[i]) {
					draw_alien(temp_pos, (uint32_t *)alien_middle_in_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS;	//adjust x coord after each alien
				//if we get to the end of row 3, then reset x pos and increment y
				if (i == ROW_3 - SMALL_OFFSET) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
			}
			//for the fourth/fifth row of aliens
			if (i >= ROW_4 && i < NUM_ALIENS) {
				//if you get to the end of the row, then reset x pos and increment y pos
				if (i == ROW_4) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
				//if the alien is alive then draw the bottom alien
				if (alien_array[i]) {
					draw_alien(temp_pos, (uint32_t *)alien_bottom_in_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS;
				//if we get to the end of row 5, then reset x pos and increment y
				if (i == ROW_5 - SMALL_OFFSET) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
			}
		}
	} else {
	//if we are drawing legs out. it's all the same comments as above. please
	//don't make me rewrite all of those comments. cuz ew.
		for (i = 0; i < NUM_ALIENS; i++) {
			if (i < ROW_2) { // draw first row
				if (alien_array[i]) { // check to see if alien is alive
					draw_alien(temp_pos, (uint32_t *)alien_top_out_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS;
			}
			if (i >= ROW_2 && i < ROW_4) {
				if (i == ROW_2) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
				if (alien_array[i]) { // check to see if alien is alive
					draw_alien(temp_pos, (uint32_t *)alien_middle_out_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS;
				if (i == ROW_3 - SMALL_OFFSET) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
			}
			if (i >= ROW_4 && i < NUM_ALIENS) {
				if (i == ROW_4) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
				if (alien_array[i]) { // check to see if alien is alive
					draw_alien(temp_pos, (uint32_t *)alien_bottom_out_12x8);
				}
				temp_pos.x += DISTANCE_BETWEEN_ALIEN_COLUMNS;
				if (i == ROW_5 - SMALL_OFFSET) {
					temp_pos.x = curr_alien_pos.x;
					temp_pos.y += DISTANCE_BETWEEN_ALIEN_ROWS;
				}
			}

		}
	}
}

//degrades a given block of a given bunker
void degrade_bunker_block(uint8_t* bunker, uint8_t block_num){
	bunker[block_num] = bunker[block_num] - DEGRADE_BUNKER_BY_1;
}

//draws an individual bunker block (bb) using the arguments
#define NUM_BUNKERS 4
#define NUM_BLOCKS 12
#define BB_ROW_2 4
#define BB_ROW_3 8
#define BB_DAMAGE_0 0
#define BB_DAMAGE_1 1
#define BB_DAMAGE_2 2
#define BB_DAMAGE_3 3
void draw_bunker_block(uint8_t bunker_number, uint8_t block_number){
	uint8_t* bunker = get_bunker(bunker_number);	//get the needed bb array

	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor;
	uint32_t y_coor = BUNKER_Y_POS;

	//switch statement to grab the correct bunker x coordinate
	switch (bunker_number) {
	case 0:
		x_coor = BUNKER_0_X; // set to bunker starting x coor
		break;
	case 1:
		x_coor = BUNKER_1_X; // set to bunker starting x coor
		break;
	case 2:
		x_coor = BUNKER_2_X; // set to bunker starting x coor
		break;
	case 3:
		x_coor = BUNKER_3_X; // set to bunker starting x coor
		break;
	}

	//get the x coordinate of the correct bb
	x_coor = x_coor + ((block_number % NUM_BUNKERS) * NUM_BLOCKS);
	y_coor = y_coor;

	//get the correct y coordinate of the bb
	if (block_number >= BB_ROW_2 && block_number < BB_ROW_3) {
	//if block is in row 2 then add the height of a bb
		y_coor += BB_HEIGHT * DOUBLE_BITMAP;
	}
	else if (block_number >= BB_ROW_3){
	//if block is in row 3 then add 2 * the height of a bb
		y_coor += (BB_HEIGHT * DOUBLE_BITMAP) * DOUBLE_BITMAP;
	}

	uint32_t* block_damage; //represents the bitmap to be displayed
	//switch to select which bitmap will be drawn
	switch(bunker[block_number]){
	//fully damaged
	case BB_DAMAGE_0:
	//damaged fource (i know)
		block_damage = (uint32_t *)bunkerDamage0_6x6;
		break;
	case BB_DAMAGE_1:
	//damage thrice
		block_damage = (uint32_t *)bunkerDamage1_6x6;
		break;
	case BB_DAMAGE_2:
	//damaged twice
		block_damage = (uint32_t *)bunkerDamage2_6x6;
		break;
	case BB_DAMAGE_3:
	//damaged once
		block_damage = (uint32_t *)bunkerDamage3_6x6;
		break;
	default:
	//undamaged
		block_damage = (uint32_t *)bunkerDamage4_6x6;
	}

	//draws the bb
	for (y = y_coor; y < y_coor + BB_HEIGHT * DOUBLE_BITMAP; y++) {
		for (x = x_coor; x < x_coor + BB_WIDTH * DOUBLE_BITMAP; x++) {
			if ((block_damage[(y - y_coor) / DOUBLE_BITMAP] & (1 << (BB_WIDTH - SMALL_OFFSET - ((x
					- x_coor) / DOUBLE_BITMAP)))) ) {
				if(find_pixel(x, y) != BACKGROUND_COLOR){
					draw_pixel(x, y, BUNKER_COLOR);
				}
			} else {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//degrades the whole bunker for lab 3
void degrade_bunker(uint8_t bunker_number) {
	uint8_t* bunker = get_bunker(bunker_number);
	uint8_t i = 0;
	//for each block in a given bunker, decrement the
	//value for the bunker block array
	for(i = 0; i < BUNKER_BLOCKS ; i++) {
		if (bunker[i] != 0) {
			bunker[i] -= DEGRADE_BUNKER_BY_1;
			draw_bunker_block(bunker_number, i);
		}
	}
}

//draws all four bunkers
void draw_bunkers() {
	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor;
	uint32_t y_coor = BUNKER_Y_POS;

	uint8_t i = 0;

	//for each bunker set the proper x coordinate then draw the bunker
	for (i = 0; i < NUM_BUNKERS; i++) {
		switch (i) {
		case 0:
			x_coor = BUNKER_0_X; // set to bunker starting x coor
			break;
		case 1:
			x_coor = BUNKER_1_X; // set to bunker starting x coor
			break;
		case 2:
			x_coor = BUNKER_2_X; // set to bunker starting x coor
			break;
		case 3:
			x_coor = BUNKER_3_X; // set to bunker starting x coor
			break;
		}

		//draws the bunker
		for (y = y_coor; y < y_coor + BUNKER_HEIGHT * DOUBLE_BITMAP; y++) {
			for (x = x_coor; x < x_coor + BUNKER_WIDTH * DOUBLE_BITMAP; x++) {
				if ((bunker_24x18[(y - y_coor) / DOUBLE_BITMAP] & (1 << (BUNKER_WIDTH - SMALL_OFFSET
						- ((x - x_coor) / DOUBLE_BITMAP))))) {
					draw_pixel(x, y, BUNKER_COLOR);
				} else {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
	}
}

//draws the tanks that depict available lives at top of screen
//this function will be called initially and on tank death
#define LIVES_TANK_GAP 45
#define DRAW 1
#define NO_DRAW 0
void draw_lives_tanks(uint8_t num_lives){
	uint32_t y = 0;
	uint32_t x = 0;
	uint32_t x_coor = LIVES_TANK_0_X;
	uint32_t y_coor = LIVES_TANK_Y_POS;
	uint8_t draw_tank = DRAW;
	uint8_t i;

	//for the input number of lives it draws the tanks
	for(i = 0; i < STARTING_LIVES; i++){
		//check if should draw tank
		if(i >= num_lives){
			draw_tank = NO_DRAW;
		}
		for (y = y_coor; y < y_coor + TANK_HEIGHT; y++) {
			for (x = x_coor; x < x_coor + TANK_WIDTH; x++) {
				if ((tank_30x16[(y - y_coor)] & (1 << (TANK_WIDTH - SMALL_OFFSET
						- ((x - x_coor)))))) {
					//if we should draw the tank then draw it, if not then erase it.
					//not super efficient, but this only draws every time the tank dies,
					//which shouldn't be very often so it should be fine
					if(draw_tank){
						draw_pixel(x, y, TANK_COLOR);
					}else{
						draw_pixel(x, y, BACKGROUND_COLOR);
					}
				} else {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
			}
		}
		x_coor += LIVES_TANK_GAP;
	}
}

//possible types of bullets
enum bullet_type{
	alien_bullet_0,
	alien_bullet_1,
	alien_bullet2,
	tank_bullet
};

//gets the index of the alien at a given point
#define NUM_ALIEN_ROWS 5
#define ERROR_VAL 55
uint8_t get_alien_index(uint16_t x, uint16_t y){
	point_t alien_block = get_alien_block_position();
	int16_t alien_block_x = alien_block.x;
	uint16_t alien_block_y = alien_block.y;
	uint16_t i, j;
	//if x is first column
	for(i = 0; i < NUM_COLUMNS; i++){
		if (x >= alien_block_x + (i * DISTANCE_BETWEEN_ALIEN_COLUMNS) && x <= alien_block_x + (i * DISTANCE_BETWEEN_ALIEN_COLUMNS) + ALIEN_WIDTH * DOUBLE_BITMAP) {
			for(j = 0; j < NUM_ALIEN_ROWS; j++){
				if (y >= alien_block_y + (j * DISTANCE_BETWEEN_ALIEN_ROWS) && y <= alien_block_y + (j * DISTANCE_BETWEEN_ALIEN_ROWS) + ALIEN_HEIGHT * DOUBLE_BITMAP) {
					return (i + (j * NUM_COLUMNS));
				}
			}
		}
	}
	return ERROR_VAL;
}

//gets the index a bunker given an x coordinate
#define NUM_BUNKERS 4
uint8_t get_bunker_index(uint16_t x){
	uint8_t i;
	for(i = 0; i < NUM_BUNKERS; i++){
		// calculates the location of the bunker block
		if(x >= BUNKER_0_X + (i * (BUNKER_GAP + BUNKER_WIDTH * DOUBLE_BITMAP)) && x <= BUNKER_0_X + (i * (BUNKER_GAP + BUNKER_WIDTH * DOUBLE_BITMAP)) + BUNKER_WIDTH * DOUBLE_BITMAP){
			return i;
		}
	}
	return -1;
}

//gets the bunker block index at a given point
#define BB_COLS 4 //number of columns of bunker blocks
#define BB_ROWS 3 //number of rows of bunker blocks
uint8_t get_bunker_block_index(uint16_t x, uint16_t y){
	uint8_t bunker_num = get_bunker_index(x);
	uint16_t bunker_x_pos = BUNKER_0_X + (bunker_num * (BUNKER_GAP + BUNKER_WIDTH * DOUBLE_BITMAP));
	uint8_t i, j;
	for(i = 0; i < BB_COLS; i++){
		if(x >= bunker_x_pos + (i * BB_WIDTH * DOUBLE_BITMAP) && x < bunker_x_pos + ((i + 1) * BB_WIDTH * DOUBLE_BITMAP)){
			for(j = 0; j < BB_ROWS; j++){
				if(y >= BUNKER_Y_POS + (j * BB_HEIGHT * DOUBLE_BITMAP) && y < BUNKER_Y_POS + (j + 1) * BB_HEIGHT * DOUBLE_BITMAP){ // precision calculating
					return (i + (j * BB_COLS));
				}
			}
		}
	}
	return -1;
}

//all collision logic is here
//detects the color at a given point and decides what to do with the collision
uint8_t check_bullet_collision(uint16_t x, uint16_t y, uint8_t bullet_type){
	uint32_t color = find_pixel(x,y);
	//if it's a tank bullet
	if(bullet_type == tank_bullet){
		//it's it hit an alien
		if(color == ALIEN_COLOR){
			erase_tank_bullet();
			int16_t alien_index = get_alien_index(x,y);
			//if valid index then kill the alien that was hit
			if(alien_index >= 0 && alien_index < NUM_ALIENS){
				set_most_recent_alien_death(alien_index);
				kill_alien(alien_index);
				return TRUE;
			}
		//else if it's hit a bunker
		}else if(color == BUNKER_COLOR){
			//degrade_bunker
			erase_tank_bullet();
			uint8_t bunker = get_bunker_index(x);
			degrade_bunker_block(get_bunker(bunker), get_bunker_block_index(x,y));
			draw_bunker_block(bunker, get_bunker_block_index(x,y));
			return TRUE;
		//else if you hit the red guy
		}else if(color == RED_GUY_COLOR){
			//kill red guy
			display_erase_red_guy();
			erase_tank_bullet();
			set_red_guy_destroyed_flag(TRUE);
			set_red_guy_just_died(TRUE);
			return TRUE;
		}
	}else{
	//else it's an alien bullet
		if(color == BUNKER_COLOR){
			if(y >= TANK_Y_POS){
			//if hit tank
				//kill tank
				uint8_t cur_lives = getLives();
				cur_lives--;	// lower current lives
				setLives(cur_lives);	//set the global to current lives
				draw_lives_tanks(cur_lives);	// draw all the lives we have
				erase_alien_bullet(bullet_type);	// erase the bullet
				return TRUE;
			}else{
			//else you hit bunker
				//degrade bunker. Also erases bullet
				erase_alien_bullet(bullet_type);
				uint8_t bunker = get_bunker_index(x);
				degrade_bunker_block(get_bunker(bunker), get_bunker_block_index(x,y));
				draw_bunker_block(bunker, get_bunker_block_index(x,y));
				return TRUE;
			}
		}
	}
	return FALSE;
}

//draws the tank bullet at the given x and y coordinates. first_time just keeps the bullet
//from erasing itself when initially shot from tank.
void draw_tank_bullet(uint16_t bullet_x, uint16_t bullet_y, uint8_t first_time) {
	uint16_t y = 0;
	uint16_t x = 0;

	//draws the bullet and erases below the bullet so long as first time != 0
	for (y = bullet_y + TANK_BULLET_HEIGHT * DOUBLE_BITMAP + BULLET_SPEED; y >= bullet_y; y--) {
		for (x = bullet_x; x < bullet_x + TANK_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
			if (y > bullet_y + TANK_BULLET_HEIGHT * DOUBLE_BITMAP && first_time == 0) { // if y is greater than the button of the bullet and is already in flight
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
			if (y < bullet_y + TANK_BULLET_HEIGHT * DOUBLE_BITMAP && (tank_bullet_1x4[(y - bullet_y) / DOUBLE_BITMAP])) {
				if(check_bullet_collision(x,y,tank_bullet)) {
					return;
				}
				draw_pixel(x, y, BULLET_COLOR);
			}
		}
	}
}

//draws the cross alien bullet
#define LEFT_OF_BULLET 2
#define RIGHT_OF_BULLET 4
void draw_alien_bullet1(uint16_t bullet_x, uint16_t bullet_y, uint8_t first_time, uint8_t bullet_number) {
	uint16_t y = 0;
	uint16_t x = 0;

	static uint8_t bullet_0_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_1_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_2_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_type = 1;		//indicates the guise of the bullets

	//need to keep track the guise of each of the bullets
	//this checks each active bullet and flips the guise
	if (bullet_number == ALIEN_BULLET_0) {
		bullet_0_type = !bullet_0_type;
		bullet_type = bullet_0_type;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		bullet_1_type = !bullet_1_type;
		bullet_type = bullet_1_type;
	}
	else {
		bullet_2_type = !bullet_2_type;
		bullet_type = bullet_2_type;
	}

	//if guise 1 then draw cross bullet 1
	uint32_t color;
	if (bullet_type) {
		for (y = bullet_y - BULLET_SPEED; y < bullet_y + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP; y++) {
			for (x = bullet_x; x < bullet_x + ALIEN_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
				color = find_pixel(x,y);
				if (x < bullet_x + LEFT_OF_BULLET || x > bullet_x + RIGHT_OF_BULLET) { // erase both sides of bullet
					if (first_time == 0) { // only erase if bullet has already been in flight
						if(color == BULLET_COLOR){
							draw_pixel(x, y, BACKGROUND_COLOR);
						}
					}
				}
				if (y < bullet_y && first_time == 0) { 	// if y coor is bottom of bullet, and it just got shot
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
				if (y > bullet_y && (cross_bullet_1_3x5[(y - bullet_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BULLET_WIDTH - SMALL_OFFSET - ((x - bullet_x) / DOUBLE_BITMAP))))) {
					if(check_bullet_collision(x,y,bullet_number)) {
						return;
					}
					draw_pixel(x, y, BULLET_COLOR);
				}
			}
		}
	}
	//if guise 2 then draw cross bullet 2
	else {
		for (y = bullet_y - BULLET_SPEED; y < bullet_y + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP; y++) {
			for (x = bullet_x; x < bullet_x + ALIEN_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
				if (x < bullet_x + LEFT_OF_BULLET || x > bullet_x + RIGHT_OF_BULLET) { // erase both sides of bullet
					if (first_time == 0) { // only erase if bullet has already been in flight
						if(color == BULLET_COLOR){
							draw_pixel(x, y, BACKGROUND_COLOR);
						}
					}
				}
				if (y < bullet_y && first_time == 0) {	// if y coor is above bottom of bullet, and it just got shot
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
				if (y > bullet_y && (cross_bullet_2_3x5[(y - bullet_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BULLET_WIDTH - SMALL_OFFSET - ((x - bullet_x) / DOUBLE_BITMAP))))) {
					if(check_bullet_collision(x,y,bullet_number)) {
						return; }
					draw_pixel(x, y, BULLET_COLOR);
				}
			}
		}
	}
}

//draws the zigzag alien bullet
void draw_alien_bullet2(uint16_t bullet_x, uint16_t bullet_y, uint8_t first_time, uint8_t bullet_number) {
	uint16_t y = 0;
	uint16_t x = 0;
	static uint8_t bullet_0_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_1_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_2_type = 1;	//indicates the guise of the bullets
	static uint8_t bullet_type = 1;		//indicates the guise of the bullets

	//need to keep track the guise of each of the bullets
	//this checks each active bullet and flips the guise
	if (bullet_number == ALIEN_BULLET_0) {
		bullet_0_type = !bullet_0_type;
		bullet_type = bullet_0_type;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		bullet_1_type = !bullet_1_type;
		bullet_type = bullet_1_type;
	}
	else {
		bullet_2_type = !bullet_2_type;
		bullet_type = bullet_2_type;
	}

	//if guise 1 then draw zigzag bullet 1
	uint32_t color;
	if (bullet_type) {
		for (y = bullet_y - BULLET_SPEED; y < bullet_y + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP; y++) {
			for (x = bullet_x; x < bullet_x + ALIEN_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
				color = find_pixel(x,y);
				if (x < bullet_x + LEFT_OF_BULLET || x > bullet_x + RIGHT_OF_BULLET) { // erase both sides of bullet
					if (first_time == 0) { // only erase if bullet has already been in flight
						if(color == BULLET_COLOR){
							draw_pixel(x, y, BACKGROUND_COLOR);
						}
					}
				}
				if (y < bullet_y && first_time == 0) {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
				if (y > bullet_y && (zigzag_bullet_1_3x5[(y - bullet_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BULLET_WIDTH - SMALL_OFFSET - ((x - bullet_x) / DOUBLE_BITMAP))))) {
					if(check_bullet_collision(x,y,bullet_number)) { return; }
					draw_pixel(x, y, BULLET_COLOR);
				}
			}
		}
	}
	else {
		for (y = bullet_y - BULLET_SPEED; y < bullet_y + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP; y++) {
			for (x = bullet_x; x < bullet_x + ALIEN_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
				if (x < bullet_x + LEFT_OF_BULLET || x > bullet_x + RIGHT_OF_BULLET) { // erase both sides of bullet
					if (first_time == 0) { // only erase if bullet has already been in flight
						if(color == BULLET_COLOR){
							draw_pixel(x, y, BACKGROUND_COLOR);
						}
					}
				}
				if (y < bullet_y && first_time == 0) {
					draw_pixel(x, y, BACKGROUND_COLOR);
				}
				if (y > bullet_y && (zigzag_bullet_2_3x5[(y - bullet_y) / DOUBLE_BITMAP] & (1 << (ALIEN_BULLET_WIDTH - SMALL_OFFSET - ((x - bullet_x) / DOUBLE_BITMAP))))) {
					if(check_bullet_collision(x,y,bullet_number)) { return; }
					draw_pixel(x, y, BULLET_COLOR);
				}
			}
		}
	}
}

// sets the type of the bullet that was shot. This is important because we need to know
// which bitmap to use when we update and redraw the bullets
void set_bullet_type(uint8_t bullet_number, uint8_t bullet_type) {
	if (bullet_number == ALIEN_BULLET_0) {
		bullet0_type = bullet_type;
	}
	else if (bullet_number == ALIEN_BULLET_1) {
		bullet1_type = bullet_type;
	}
	else {
		bullet2_type = bullet_type;
	}
}

// causes a random alien to shoot
// It checks a random column for any aliens that may be alive. If it
// finds one, it will return a point directly below that alien
// If all aliens are dead in that column, return an impossible value.
// Might make it find an alien recursively?
#define NUM_COLUMNS 11
#define CENTER_SHOT 2
point_t cause_an_alien_to_shoot() {
	uint8_t* alien_array = get_alien_array();
	int8_t i;
	uint8_t found = FALSE;
	point_t alien_to_shoot_coor = {IMPOSSIBLE_COOR, IMPOSSIBLE_COOR}; // init to impossible values
	uint32_t r = rand() % NUM_ALIENS;      // randomly select an alien column
	uint8_t fall_back_column = 0;

	while (!found) {
		// determine if the lowest alien in that column
		// start at the last row, see if alien. if no alien
		// move up a row and check
		for (i = r + LAST_ORIGINAL_ALIEN_ROW_INDEX; i >= 0 ; i -= NUM_COLUMNS) {
			if (alien_array[i] == 1) {// if alien is alive, find a point to shoot from
				alien_to_shoot_coor = get_alien_block_position();

				alien_to_shoot_coor.x = alien_to_shoot_coor.x + ( i % NUM_COLUMNS) * DISTANCE_BETWEEN_ALIEN_COLUMNS + ALIEN_WIDTH - CENTER_SHOT;

				//if row 1 get the y coordinate of that row
				if (i >= 0 && i < ROW_2) {
					alien_to_shoot_coor.y = alien_to_shoot_coor.y + ALIEN_HEIGHT * DOUBLE_BITMAP;
				}
				//if row 2 get the y coordinate of that row
				else if(i >= ROW_2 && i < ROW_3) {
					alien_to_shoot_coor.y = alien_to_shoot_coor.y + DISTANCE_BETWEEN_ALIEN_ROWS + ALIEN_HEIGHT * DOUBLE_BITMAP;
				}
				//if row 3 get the y coordinate of that row
				else if(i >= ROW_3 && i < ROW_4) {
					alien_to_shoot_coor.y = alien_to_shoot_coor.y + DISTANCE_BETWEEN_ALIEN_ROWS * ROW_3_MULT + ALIEN_HEIGHT * DOUBLE_BITMAP;
				}
				//if row 4 get the y coordinate of that row
				else if(i >= ROW_4 && i < ROW_5) {
					alien_to_shoot_coor.y = alien_to_shoot_coor.y + DISTANCE_BETWEEN_ALIEN_ROWS * ROW_4_MULT + ALIEN_HEIGHT * DOUBLE_BITMAP;
				}
				//if row 5 get the y coordinate of that row
				else if(i >= ROW_5) {
					alien_to_shoot_coor.y = alien_to_shoot_coor.y + DISTANCE_BETWEEN_ALIEN_ROWS * ROW_5_MULT + ALIEN_HEIGHT * DOUBLE_BITMAP;
				}
				found = TRUE;
				break;
			}
		}
		r = fall_back_column;
		fall_back_column++;
	}
	return alien_to_shoot_coor;


}

//shoots a random bullet from a random alien given the number of bullets on the screen (0, 1, or 2)
void shoot_alien_bullet(uint8_t bullet_number) {
	if (get_alien_bullet_inflight(bullet_number) == TRUE) { // bullet is already in flight?
		return;
	}

	point_t alien_to_shoot_coor = cause_an_alien_to_shoot();
	if (alien_to_shoot_coor.x == IMPOSSIBLE_COOR) { // compare to impossible value
		return; // return. We won't draw a bullet this time because we checked a column that didn't have any aliens
	}
	uint16_t bullet_x = alien_to_shoot_coor.x; // change x coordinate
	uint16_t bullet_y = alien_to_shoot_coor.y; // change y coordinate
	uint8_t first_time = INITIAL_SHOT_IS_TRUE;	//this will always be true when first fired

	set_alien_bullet_inflight(TRUE, bullet_number); //update value of the global bullet

	setAlienBulletPositionX(bullet_x, bullet_number);	//set the x position of bullet
	setAlienBulletPositionY(bullet_y, bullet_number);	//set the y position of bullet

	uint32_t r = rand() % RAND_LIMIT_4_BULLET_TYPE;      // random number of either 0 or 1

	//randomize the bullet selected
	if (r == 0) {
		draw_alien_bullet1(bullet_x, bullet_y, first_time, bullet_number);
		set_bullet_type(bullet_number, BULLET_CROSS);
	}
	else {
		draw_alien_bullet2(bullet_x, bullet_y, first_time, bullet_number);
		set_bullet_type(bullet_number, BULLET_ZIGZAG);
	}

}

//shoots a tank bullet
void shoot_tank_bullet() {
	if (get_tank_bullet_inflight() == TRUE) { // bullet is already in flight?
		return;
	}
	set_tank_bullet_just_fired(TRUE);
	//
	uint16_t bullet_x = getTankPositionGlobal() + (TANK_WIDTH / DOUBLE_BITMAP) - SMALL_OFFSET; //get bullet x
	uint16_t bullet_y = TANK_Y_POS - TANK_BULLET_HEIGHT * DOUBLE_BITMAP - 1;			//get bullet y
	uint8_t first_time = INITIAL_SHOT_IS_TRUE;	//always true first time

	set_tank_bullet_inflight(TRUE);	//sets the tank_bullet_inflight global so we can't shoot another one until it's gone

	setTankBulletPositionX(bullet_x);	//set x position
	setTankBulletPositionY(bullet_y);	//set y position
	draw_tank_bullet(bullet_x, bullet_y, first_time);	//draws the bullet
}

//erases the alien bullet
// need some sort of condition checking to erase the bullets differently when it hits something, or when it reaches end of screen.
void erase_alien_bullet(uint8_t bullet_number) {
	//xil_printf("inside erase alien bullet\n\r");
	uint16_t bullet_x = getAlienBulletPositionX(bullet_number); //get x position
	uint16_t bullet_y = getAlienBulletPositionY(bullet_number) - ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP; // since bullet is ahead, but not drawn yet, we have to erase a bit behind
	uint16_t y = 0;
	uint16_t x = 0;

	set_alien_bullet_inflight(FALSE, bullet_number);	//makes the bullet available for refire

	//draws background over the bullet
	for (y = bullet_y; y <= bullet_y + ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP + BULLET_SPEED; y++) {
		for (x = bullet_x; x < bullet_x + ALIEN_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
			if(find_pixel(x,y) == ALIEN_BULLET_COLOR){
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//erase tank bullet
void erase_tank_bullet() {
	uint16_t bullet_x = getTankBulletPositionX();	//get x position
	uint16_t bullet_y = getTankBulletPositionY();	//get y position
	uint16_t y = 0;
	uint16_t x = 0;

	set_tank_bullet_inflight(FALSE);	//makes bullet available for refire

	//draws background over the tank bullet
	for (y = bullet_y; y <= bullet_y + TANK_BULLET_HEIGHT * DOUBLE_BITMAP + BULLET_SPEED; y++) {
		for (x = bullet_x; x < bullet_x + TANK_BULLET_WIDTH * DOUBLE_BITMAP; x++) {
			if(find_pixel(x,y) == BULLET_COLOR && y >= TOP_SCREEN_PLAYABLE_AREA){
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//draws all bullets called by control
void draw_bullets() {
	uint8_t bullet_inflight;
	//if tank bullet is above playable area, then erase it
	if (getTankBulletPositionY() < TOP_SCREEN_PLAYABLE_AREA) {
		erase_tank_bullet();
	}
	//else redraw the tank bullet
	else {
		if(get_tank_bullet_inflight()){
			draw_tank_bullet(getTankBulletPositionX(), getTankBulletPositionY(), !INITIAL_SHOT_IS_TRUE);
		}
	}
	// draws or erase alien bullet #0
	bullet_inflight = get_alien_bullet_inflight(ALIEN_BULLET_0);
	if (getAlienBulletPositionY(ALIEN_BULLET_0) >= BOTTOM_LINE_Y - (ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP) - 1 && bullet_inflight) {
		erase_alien_bullet(ALIEN_BULLET_0); //erase it
	}
	else {
		if (bullet0_type == BULLET_CROSS) { 	// if bullet0 type is bullet cross, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_0)){
				draw_alien_bullet1(getAlienBulletPositionX(ALIEN_BULLET_0), getAlienBulletPositionY(ALIEN_BULLET_0), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_0);
			}
		}
		else {									// if bullet0 type is bullet zigzag, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_0)){
				draw_alien_bullet2(getAlienBulletPositionX(ALIEN_BULLET_0), getAlienBulletPositionY(ALIEN_BULLET_0), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_0);
			}
		}
	}
	// draws or erase alien bullet #1
	bullet_inflight = get_alien_bullet_inflight(ALIEN_BULLET_1);
	if (getAlienBulletPositionY(ALIEN_BULLET_1) >= BOTTOM_LINE_Y - (ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP) - 1 && bullet_inflight) {
		erase_alien_bullet(ALIEN_BULLET_1);	//erase it
	}
	else {
		if (bullet1_type == BULLET_CROSS) { 	// if bullet1 type is bullet cross, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_1)) {
				draw_alien_bullet1(getAlienBulletPositionX(ALIEN_BULLET_1), getAlienBulletPositionY(ALIEN_BULLET_1), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_1);
			}
		}
		else {									// if bullet1 type is bullet zigzag, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_1)) {
				draw_alien_bullet2(getAlienBulletPositionX(ALIEN_BULLET_1), getAlienBulletPositionY(ALIEN_BULLET_1), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_1);
			}
		}
	}
	// draws or erase alien bullet #2
	bullet_inflight = get_alien_bullet_inflight(ALIEN_BULLET_2);
	if (getAlienBulletPositionY(ALIEN_BULLET_2) >= BOTTOM_LINE_Y - (ALIEN_BULLET_HEIGHT * DOUBLE_BITMAP) - 1 && bullet_inflight) {
		erase_alien_bullet(ALIEN_BULLET_2); //erase it
	}
	else {
		if (bullet2_type == BULLET_CROSS) { 	// if bullet2 type is bullet cross, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_2)) {
				draw_alien_bullet1(getAlienBulletPositionX(ALIEN_BULLET_2), getAlienBulletPositionY(ALIEN_BULLET_2), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_2);
			}
		}
		else {									// if bullet2 type is bullet zigzag, draw that kind
			if(get_alien_bullet_inflight(ALIEN_BULLET_2)) {
				draw_alien_bullet2(getAlienBulletPositionX(ALIEN_BULLET_2), getAlienBulletPositionY(ALIEN_BULLET_2), !INITIAL_SHOT_IS_TRUE, ALIEN_BULLET_2);
			}
		}
	}
}

//draws the tank on the screen
void draw_tank(){
	uint32_t y = 0;
	uint32_t x = 0;
	uint16_t next_pos = getTankPositionGlobal(); //gets pos where the tank should be drawn

	// erases the trail behind tank after it moves////////////////////
	if (curr_tank_pos < next_pos) { // tank moved to the right so erase to the left
		for (y = TANK_Y_POS; y < TANK_Y_POS + TANK_HEIGHT; y++) {
			for (x = curr_tank_pos; x < curr_tank_pos + TANK_SPEED; x++) {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
	else {	// tank moved to the left so erase to the right
		uint16_t x_coor_end = curr_tank_pos + TANK_WIDTH - TANK_SPEED;
		for (y = TANK_Y_POS; y < TANK_Y_POS + TANK_HEIGHT; y++) {
			for (x = x_coor_end; x < curr_tank_pos + TANK_WIDTH; x++) {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
	//////////////////////////////////////////////////////////////////

	// draws the tank
	curr_tank_pos = next_pos;
	for (y = TANK_Y_POS; y < TANK_Y_POS + TANK_HEIGHT; y++) {
		for (x = curr_tank_pos; x < curr_tank_pos + TANK_WIDTH; x++) {
			if ((tank_30x16[(y - TANK_Y_POS)] & (1 << (TANK_WIDTH - SMALL_OFFSET
					- ((x - curr_tank_pos)))))) {
				//optimize later?
				draw_pixel(x, y, TANK_COLOR);
			} else {
				draw_pixel(x, y, BACKGROUND_COLOR);
			}
		}
	}
}

//general render function only called when first drawing the whole screen
void display_render() {
	display_black_screen();	// blanks out the screen
	draw_scoreboard_text(); // function describes the purpose
	display_draw_score(RESET_SCORE); // function describes the purpose. set's score back to 0
	draw_lives_text(); // function describes the purpose
	draw_bottom_line(); // function describes the purpose
	draw_aliens(); // function describes the purpose
	draw_bunkers(); // function describes the purpose
	draw_tank(); // function describes the purpose
	draw_lives_tanks(getLives()); // function describes the purpose
	initializing = 0; // used to know if this is the first time the game is running
}

void display_draw_tank_death(uint8_t guise) {
	uint16_t y = 0;
	uint16_t x = 0;
	uint16_t x_coor = getTankPositionGlobal();
	uint16_t y_coor = TANK_Y_POS;
	uint32_t* bitmap;
	uint16_t want_to_draw_bit = 0;

	// since death animation has two different guises, we must switch back and forth between them.
	if (guise) {
		bitmap = (uint32_t*) tank_explosion_1_15x8;
	}
	else {
		bitmap = (uint32_t*) tank_explosion_2_15x8;
	}
	// draws the death animation inside the for loop
	for (y = y_coor; y < y_coor + TANK_HEIGHT; y++) {
		for (x = x_coor; x < x_coor + TANK_WIDTH; x++) {
			want_to_draw_bit = (bitmap[(y - y_coor) / DOUBLE_BITMAP] & (1 << ((TANK_WIDTH/DOUBLE_BITMAP) - SMALL_OFFSET - ((x - x_coor) / DOUBLE_BITMAP))));
			//xil_printf("want_to_draw_bit: %d\r\n",want_to_draw_bit);
			if (want_to_draw_bit) {
				draw_pixel(x, y, TANK_COLOR);
			} else {
				draw_pixel(x, y, BACKGROUND_COLOR); // draws the background color if it isn't tank_color
			}
		}
	}
}
