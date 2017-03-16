#include "helper.h"
#include "pid.h"
#include "functions.h"
#include "special_cases.h"

pidData_t steer;
void old_school_main();
void pid_main();

int main() {
	init();
	set_led_data(1337);
	servo(0);
	set_line();
	isr_ptr = dummy_1;
	
	//main menu
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

typedef struct Pid_motor_speed {
	uint16_t l, r;	
} pms;

pms pid_motor_speed;
#define MAX_CTE 300.0
#define speed_increase_const 1.5

inline void dynamic_speed(uint16_t l, uint16_t r) {
	pid_motor_speed.l = l;
	pid_motor_speed.r = r;
}

#define DECREASE_SPEED_CONST 5
void decrease_speed() {
	pid_motor_speed.l -= DECREASE_SPEED_CONST;
	pid_motor_speed.r -= DECREASE_SPEED_CONST;
}

#define PID_SPEED_RATIO 0.3
inline void calc_motor_speed(int16_t cte) {
	uint16_t t;
	
	if (_90_turn) {
		dynamic_speed(mspeed/3, mspeed/3);
	}
	else if (cte == MAX_CTE) {
		dynamic_speed(0, mspeed);
	} else if (cte == -MAX_CTE) {
		dynamic_speed(mspeed, 0);
	} else if (cte == 0) {
		dynamic_speed(mspeed, mspeed);
	} else if (cte > 0) {
		t = (int16_t)(mspeed * ((MAX_CTE - cte)/MAX_CTE));
		t = (int16_t)(mspeed - (PID_SPEED_RATIO*t));
		dynamic_speed((uint16_t)(t - (cte / 5)), (uint16_t)(t + (cte / 5)));
	} else if(cte < 0) {
		t = (int16_t)(mspeed * ((-MAX_CTE-cte)/-MAX_CTE));
		t = (int16_t)(mspeed - (PID_SPEED_RATIO*t));
		dynamic_speed((uint16_t)(t - (cte / 5)), (uint16_t)(t + (cte / 5)));
	}
}

#define NORMAL_TRACE 0

void loop4ever() {
	while (1);
}

#define RAMP_CONST 300

void pid_main() {
	/* PID variables */
	int16_t cte = 0;
	int16_t pid_output;
	
	/* others */
	uint8_t state = 0;
	uint16_t first_encoder_read = 0, next_encoder_read;
	uint16_t delta;
	
	pid_calibrate();
	pid_Init(K_P, K_I, K_D, &steer);
	
	set_led_data(1337);
	dynamic_speed(mspeed, mspeed);
	fwd(pid_motor_speed.l, pid_motor_speed.r);
	while (1) {
		set_led_data(state);
		led_data.sensor_debug_output = (switch_lane) | (_90_turn << 7) | (no_line << 6);
		switch (state) {
			case 0: //normal trace
				if (off_lane == 100) {
					fwd(0, 0);
					loop4ever();
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
					//next_encoder_read = encoder;
					//delta = next_encoder_read - first_encoder_read;
					//first_encoder_read = next_encoder_read;
					//set_led_data(delta);
					//if (delta > RAMP_CONST) decrease_speed;	
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
					do_noline();
				}
				state = NORMAL_TRACE;
			break;	
		
			case 1: //left switch
				_delay_ms(100);
				if ( (read_sensor() & MASK0_1) != 0) { //wrong detection between halfline and full line
					state = 3;
				} else {
					if (!_90_turn) { //it's a 90 turn not switch lane
						switch_lane = 2; //set switch lane flag
						state = NORMAL_TRACE;
					} else {
						no_line = 0;
						do_90_left_turn();
						state = NORMAL_TRACE;
					}
				}
			break;
			
			case 2: //right switch
				_delay_ms(100);
				if ( (read_sensor() & MASK1_0) != 0 ) { //wrong detection between halfline and full line
					state = 3;
				} else {
					if (!_90_turn) {
						switch_lane = 1; //set switch lane flag
						state = NORMAL_TRACE;
					} else {
						no_line = 0;
						do_90_right_turn();
						state = NORMAL_TRACE;
					}
				}
			break;
			
			case 3: //crossline detected
				_delay_ms(200);
				_90_turn = 1;
				no_line = 1;
				switch_lane = 0;
				state = NORMAL_TRACE;
			break;
		}
	}	
}

ISR(TIMER0_COMP_vect) {
	isr_ptr();
}

ISR(INT0_vect) {
	encoder += 1;
} 
