#define  DEBUG
#include "helper.h"
#include "pid.h"
#include "functions.h"

pidData_t steer;
void old_school_main();
void pid_main();

uint8_t switch_lane = 0;
uint8_t _90_turn = 0;
uint8_t no_line = 0;

#define  switch_lane_constant 95

inline void do_switch_lane() {
	int16_t servo_pos;
	uint8_t t = (uint8_t)(motor_speed - (motor_speed/3));
	
	fwd(t, t);
	if (switch_lane == 1) {
		servo_pos = switch_lane_constant;
		last_cte = -1;
	}
	else {
		servo_pos = -switch_lane_constant - 10;
		last_cte = 1;
	}
	servo(servo_pos);
	while (read_sensor() == 0);
	switch_lane = 0;
}

void do_noline() {
	int16_t pid_output;
	
	pid_output = pid_Controller(0, last_cte, &steer);
	servo(pid_output/2);
	while (read_sensor() == 0);
	no_line = 0;
}

void set_line() {
	uint16_t t;
	
	LINE = eeprom_read_word(&eeprom_LINE);
	if (LINE == 0xffff) {
		LINE = LINE_DEFAULT;
	}
	while (1) {
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) LINE -= 10;
		if (get_button(BTN2)) LINE += 10;
		set_led_data(LINE);
	}
	eeprom_write_word(&eeprom_LINE, LINE);
}

int main() {
	init();
	set_led_data(8888);
	servo(0);
	
	set_line();
	isr_ptr = dummy_1;
	while (1) {
		if (get_button(BTN0)) {
			isr_ptr = isr_0;
			pid_main();
		}
		if (get_button(BTN1)) {
			isr_ptr = isr_1;
			old_school_main();
		}
	}
}

#define  _90_turn_const 0

void do_90_right_turn() {
	fwd(_90_turn_const, _90_turn_const);
	servo(150);
	while (read_sensor() != 0b00011000);
	_90_turn = 0;
}

void do_90_left_turn() {
	fwd(_90_turn_const, _90_turn_const);
	servo(-150);
	while (read_sensor() != 0b00011000);
	_90_turn = 0;
}

typedef struct Pid_motor_speed {
	uint16_t l, r;	
} pms;

pms pid_motor_speed;

#define pid_speed_ratio 0.3
#define max_cte 300.0
#define speed_increase_const 1.5

inline void dynamic_speed(uint16_t l, uint16_t r) {
	pid_motor_speed.l = l;
	pid_motor_speed.r = r;
}

inline void calc_motor_speed(int16_t cte) {
	uint16_t t;
	
	if (cte == max_cte) {
		dynamic_speed(0, motor_speed);
	} else if (cte == -max_cte) {
		dynamic_speed(motor_speed, 0);
	} else if (cte == 0) {
		dynamic_speed(motor_speed, motor_speed);
	} else if (cte > 0) {
		t = (int16_t)(motor_speed * ((max_cte - cte)/max_cte));
		t = (int16_t)(motor_speed - (pid_speed_ratio*t));
		dynamic_speed((uint16_t)(t - (cte / 5)), (uint16_t)(t + (cte / 5)));
	} else if(cte < 0) {
		t = (int16_t)(motor_speed * ((-max_cte-cte)/-max_cte));
		t = (int16_t)(motor_speed - (pid_speed_ratio*t));
		dynamic_speed((uint16_t)(t - (cte / 5)), (uint16_t)(t + (cte / 5)));
	}
}

#define normal_trace 0

void pid_main() {
	/* PID variables */
	int16_t cte = 0;
	int16_t pid_output;
	
	/* others */
	uint8_t state = 0;
	
	test_mode();
	pid_Init(K_P, K_I, K_D, &steer);
	
	dynamic_speed(motor_speed, motor_speed);
	fwd(pid_motor_speed.l, pid_motor_speed.r);
	while (1) {
		set_led_data(state);
		led_data.sensor_debug_output = (switch_lane) | (_90_turn << 7) | (no_line << 6);
		switch (state) {
			case 0: //normal trace
				if (off_lane == 100) {
					fwd(0, 0);
					main();
				}
				if (check_crossline()) {
					off_lane += 1;
					state = 3;
				} else if (check_leftline()) {
					state = 1;
				} else if (check_rightline()) {
					state = 2;
				} else if (check_noline()) {
					state = 10;
				} else {
					cte = calc_cte(read_sensor());
					/*
					if (cte < 0) set_led_data(9000 - cte);
					else set_led_data(cte);
					*/
					pid_output = pid_Controller(0, cte, &steer);
					calc_motor_speed(cte);
					servo(pid_output/2);	
					fwd(pid_motor_speed.l, pid_motor_speed.r);
				}
			break;
			
			case 10: //no line
				if (switch_lane) {
					do_switch_lane();
				} else if (_90_turn) { //it's no line not 90 turn
					_90_turn = 0;
				} else if (no_line) {
					no_line = 1;
					do_noline();
				}
				state = normal_trace;
			break;	
		
			case 1: //left switch
				if ((read_sensor() & mask0_3) != 0) { //wrong detection between halfline and full line
					state = 3;
				} else {
					if (!_90_turn) { //it's a 90 turn not switch lane
						switch_lane = 2; //set switch lane flag
						state = normal_trace;
					} else {
						do_90_left_turn();
					}
				}
			break;
			
			case 2: //right switch
				if ((read_sensor() & mask3_0) != 0) { //wrong detection between halfline and full line
					state = 3;
				} else {
					if (!_90_turn) {
						switch_lane = 1; //set switch lane flag
						state = normal_trace;
					} else {
						do_90_right_turn();
					}
				}
			break;
			
			case 3: //crossline detected
				_90_turn = 1;
				no_line = 1;
				fwd(motor_speed/2, motor_speed/2); //lower speed for 90 degree turn
				switch_lane = 0;
				state = normal_trace;
			break;
		}
	}	
}

ISR(TIMER0_COMP_vect) {
	isr_ptr();
}

/*
ISR(INT0_vect) {
	timer0_mil += 1;
} */
