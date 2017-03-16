#include "stack.h"

#define MASK3_3 0b11100111
#define MASK3_0 0b11100000
#define MASK0_3 0b00000111
#define MASK4_0 0b11110000
#define MASK0_4 0b00001111
#define MASK1_0 0b10000000
#define MASK0_1 0b00000001
#define MASK2_0 0b11000000
#define MASK0_2 0b00000011
/*
	8 sensors
	*   *   *   *   *   *   *   *
	3   2   1          -1   -2  -3
	
*/

int16_t last_cte = 0;
uint8_t off_lane = 0;

inline int16_t lsb_first(int16_t sensor_val) {
	int16_t ret;
		
	for(uint8_t i = 0; i < 3; i++) {
		if ( (sensor_val & (1 << i)) != 0) {
			ret = -(3*m - m*i);
			last_cte = ret;
			return ret;
		}
	}
	return 0;
}

inline int16_t msb_first(int16_t sensor_val) {
	int16_t ret;
	
	for(uint8_t i = 0; i < 3; i++) {
		if ( (sensor_val & (0b100 >> i)) != 0 ) {
			ret = (3*m - m*i);
			last_cte = ret; //using last cte in case the car is in the middle of two white lines
			return ret;
		}
	}
	return 0;
}

inline int16_t calc_cte(uint8_t sensor_val) { //calculate cross track error
	uint8_t t = 0;

	/*if (sensor_val == 0b00011000 || sensor_val == 0b00010000 || sensor_val == 0b00001000 || sensor_val == 0xff) { //if the car is in the middle of the track return 0
		last_cte = 0;
		return 0;
	}*/
	
	if (sensor_val == 0b00011000) { //increase sensor resolution
		last_cte = 0;
		return 0;
	} else if (sensor_val == 0b00010000) {
		last_cte = 25;
		return 25;
	} else if (sensor_val == 0b00001000) {
		last_cte = -25;
		return -25;
	}
	
	if ( (sensor_val & MASK3_0) != 0) t += 1;
	if ( (sensor_val & MASK0_3) != 0) t += 2;
	
	switch (t) {
		case 1: //right
			if (last_cte >= 0) return msb_first(sensor_val >> 5);
			else return lsb_first(sensor_val >> 5) - 3*m; /* change from 40 -> 60*/
		break;
	
		case 2: //left
			if (last_cte <= 0) return lsb_first(sensor_val);
			else return msb_first(sensor_val) + 3*m; /* change from 40 -> 60*/
		break;
		
		case 3:
			if (last_cte > 0) return last_cte + m;
			if (last_cte  < 0) return last_cte - m;
			return last_cte;
		break;
			
		default:	
			return 0;
		break;
	}
}

inline uint8_t check_crossline() {
	if (read_sensor() == 0xff) return 1;
	return 0;
}

inline uint8_t check_leftline() {
	uint8_t sensor_val;
	
	sensor_val = read_sensor();
	if ( (sensor_val == 0b11111100) || (sensor_val == 0b11111000) || (sensor_val == 0b11110000)) return 1;
	return 0;
}

inline uint8_t check_rightline() {
	uint8_t sensor_val;
	
	sensor_val = read_sensor();
	if ( (sensor_val == 0b00111111) || (sensor_val == 0b00011111) || (sensor_val == 0b00001111)) return 1;
	return 0;
}

uint8_t check_noline() {
	if (read_sensor() == 0) return 1;
	return 0;
}

void dummy_0() {
	print();
}

void dummy_1() {
	print();
	read_sensor();
}

void (*isr_ptr)(void) = &dummy_0;

void isr_0() {
	print();
}

void back_trace() {
	uint16_t idx = stack_size - 1;
	isr_ptr = &dummy_1;
	while (1) {
		if (get_button(BTN1)) {
			if (stack_size > 0) idx -= 1;
			else idx = stack_size - 1;
		}
		if (get_button(BTN2)) {
			if (stack_size < stack_size-1) idx += 1;
			else idx = 0;
		}
		set_led_data(get_stack(idx));
	}
}

uint8_t my_timer = 0;

void isr_1() {
	/*
	my_timer += 1;
	if (my_timer >= 100) {
		uint8_t t = read_sensor();
		if (t == 0 || t == 0xff) off_lane += 1;
		if (off_lane >= 20 ) {
			off_lane = 0;
			fwd(0, 0);
			back_trace();
		}
		my_timer = 0;
	}
	*/
	print();
}

#include "old_school_main.h"