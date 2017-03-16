#define  DEBUG
#include "helper.h"
#include "pid.h"
#include "functions.h"

pidData_t steer;
void old_school_main();
void pid_main();

uint8_t switch_lane = 0;
uint8_t _90_turn = 0;

#define  switch_lane_constant 95
void do_switch_lane() {
	int16_t servo_pos;
	uint8_t t = (uint8_t)(motor_speed - (motor_speed/3));
	
	fwd(t, t);
	if (switch_lane == 1) servo_pos = switch_lane_constant;
	else servo_pos = -switch_lane_constant - 10;
	servo(servo_pos);
	last_cte = 0;
	while (read_sensor() == 0);
	switch_lane = 0;
}

void set_line() {
	while (1) {
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) LINE -= 10;
		if (get_button(BTN2)) LINE += 10;
		set_led_data(LINE);
	}
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

#define  _90_turn_const 20

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

#define pid_speed_ratio 0.2

pms pid_motor_speed;

#define max_cte 300.0

void calc_motor_speed(int16_t cte) {
	uint16_t t;

	if (cte == 0) {
		pid_motor_speed.l = (uint16_t)(motor_speed * 1.5);
		pid_motor_speed.r = (uint16_t)(motor_speed * 1.5);
	}
	else if (cte > 0) {
		t = (int16_t)(motor_speed * ((max_cte - cte)/max_cte));
		t = (int16_t)(motor_speed - (pid_speed_ratio*t));
		pid_motor_speed.l = (uint16_t)(t - (cte / 5));
		pid_motor_speed.r = (uint16_t)(t + (cte / 5));
	}
	else if(cte < 0) {
		t = (int16_t)(motor_speed * ((-max_cte-cte)/-max_cte));
		t = (int16_t)(motor_speed - (pid_speed_ratio*t));
		pid_motor_speed.l = (uint16_t)(t - (cte / 5));
		pid_motor_speed.r = (uint16_t)(t + (cte / 5));
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
	
	fwd(motor_speed, motor_speed);
	while (1) {
		//set_led_data(state);
		//led_data.sensor_debug_output = (switch_lane | (_90_turn << 7));
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
					pid_output = pid_Controller(0, cte, &steer);
					calc_motor_speed(cte);
					if (cte < 0) cte -= 9000;
					set_led_data(abs(cte));
					fwd(pid_motor_speed.l, pid_motor_speed.r);
					servo(pid_output/2);
				}
			break;
			
			case 10: //no line
				if (switch_lane) {
					do_switch_lane();
				} else if (_90_turn) { //it's no line not 90 turn
					_90_turn = 0;
				}
				state = normal_trace;
			break;	
		
			case 1: //left switch
				if (check_crossline()) { //wrong detection between halfline and full line
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
				if (check_crossline()) { //wrong detection between halfline and full line
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
			
			case 3:
				_90_turn = 1;
				switch_lane = 0;
				state = normal_trace;
			break;
		}
	}	
}

ISR(TIMER0_COMP_vect) {
	isr_ptr();
}


ISR(INT0_vect) {
	timer0_mil += 1;
}
