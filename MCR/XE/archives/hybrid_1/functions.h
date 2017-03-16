#define mask3_0 0b11100000
#define mask0_3 0b00000111

/*
	8 sensors
	*   *   *   *   *   *   *   *
	-3  -2  -1          1   2   3
	
*/

int16_t last_cte = 0;

int16_t lsb_first(int16_t sensor_val) {
	int16_t ret;
		
	for(uint8_t i = 0; i < 3; i++) {
		if ( (sensor_val & (1 << i)) != 0) {
			ret = -(3*m - m*i);
			last_cte = ret;
			return ret;
		}
	}
}

int16_t msb_first(int16_t sensor_val) {
	int16_t ret;
	
	for(uint8_t i = 0; i < 3; i++) {
		if ( (sensor_val & (0b100 >> i)) != 0 ) {
			ret = (3*m - m*i);
			last_cte = ret; //using last cte in case the car is in the middle of two white lines
			return ret;
		}
	}
}

int16_t calc_cte(uint8_t sensor_val) { //calculate cross track error
	uint8_t t = 0;

	if (sensor_val == 0b00011000 || sensor_val == 0b00010000 || sensor_val == 0b00001000) { //if the car is in the middle of the track return 0
		last_cte = 0;
		set_led_data(0);
		return 0;
	}
	if ( (sensor_val & mask3_0) != 0) t += 1;
	if ( (sensor_val & mask0_3) != 0) t += 2;
	
	if (t == 1) { //right
		if (last_cte >= 0) return msb_first(sensor_val >> 5);
		else return lsb_first(sensor_val >> 5) - 50;
	}
	
	if (t == 2) { //left
		if (last_cte <= 0) return lsb_first(sensor_val);
		else return msb_first(sensor_val) + 50;
	}
	
	set_led_data(abs(last_cte));
	if (t == 3) return last_cte;
}

uint8_t check_crossline() {
	if(( read_sensor() == 0b11111111) || ((read_sensor() & 0b01111110) == 0b01111110)) return 1;
	return 0;
}

uint8_t check_rightline() {
	if( ((read_sensor() & 0b00001111) == 0b00001111) || ((read_sensor() & 0b00011111) == 0b00011111)) return 1;
	return 0;
}

uint8_t check_leftline() {
	if( ((read_sensor() & 0b11110000) == 0b11110000) || ((read_sensor() & 0b11111000) == 0b11111000)) return 1;
	return 0;
}

uint8_t off_lane = 0;

void loop4ever() {
	while (1) {
		;
	}
}

void dummy() {
	print();
	read_sensor();
}

void isr_0() {
	print();
}

uint8_t my_timer = 0;

void isr_1() {
	my_timer += 1;
	if (my_timer >= 100) {
		uint8_t t = read_sensor();
		if (t == 0 || t == 0xff) off_lane += 1;
		if (off_lane >= 20 ) {
			off_lane = 0;
			fwd(0, 0);
			loop4ever();
		}
		my_timer = 0;
	}
	print();
}

void (*isr_ptr)(void) = &dummy;

#include "old_school_main.h"