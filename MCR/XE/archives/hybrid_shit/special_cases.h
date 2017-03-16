uint8_t switch_lane = 0;
uint8_t _90_turn = 0;
uint8_t no_line = 0;

#define  SWITCH_LANE_CONST 95
#define TIMEOUT_CONST 1000

void f_timeout() {
	set_led_data(1338);
	fwd(0, 0);
	while (1) {
		;
	}
}

inline void do_switch_lane() {
	int16_t servo_pos;
	uint16_t timeout = 0;
	
	set_led_data(5555);
	if (switch_lane == 1) { //right switch
		fwd(motor_speed, motor_speed/2);
		servo_pos = SWITCH_LANE_CONST;
	}
	else { //left switch
		servo_pos = -SWITCH_LANE_CONST;
		fwd(motor_speed/2, motor_speed);
	}
	servo(servo_pos);
	if (switch_lane == 1) {
		while ( (read_sensor() & 0b01111000) == 0) {
			timeout += 1;
			if (timeout == TIMEOUT_CONST) {
				f_timeout();
			}
		}
	}
	else {
		while ( (read_sensor() & 0b00011110) == 0) {
			timeout += 1;
			if (timeout == TIMEOUT_CONST) {
				f_timeout();
			}
		}
	}
	switch_lane = 0;
}

#define NOLINE_CONST 100

void do_noline() {
	uint8_t loop = 1;
	uint16_t timeout = 0;
	
	last_cte = 0;
	fwd(motor_speed/2, motor_speed/2);
	while (loop) {
		timeout += 1;
		if (timeout == TIMEOUT_CONST) f_timeout();
		switch(read_sensor()) {
			case 0:
				servo(0);
			break;
			
			case 0b10000000:
			case 0b11000000:
			servo(-NOLINE_CONST);
			break;
			
			case 0b00000001:
			case 0b00000011:
			servo(NOLINE_CONST);
			break;
			
			case 0b00011000:
			case 0b00010000:
			case 0b00001000:
				loop = 0;
			break;
		}
	}
	no_line = 0;
}

void do_90_right_turn() {
	uint16_t timeout = 0;
	
	set_led_data(7777);
	while (read_sensor() != 0);
	fwd(motor_speed, 0);
	servo(150);
	while (read_sensor() != 0b00011000) {
		timeout += 1;
		if (timeout == TIMEOUT_CONST) f_timeout();
	}
	last_cte = 0;
	_90_turn = 0;
	fwd(motor_speed, motor_speed);
}

void do_90_left_turn() {
	uint16_t timeout = 0;
	
	set_led_data(6666);
	while (read_sensor() != 0);
	fwd(0, motor_speed);
	servo(-150);
	while (read_sensor() != 0b00011000) {
		timeout += 1;
		if (timeout == TIMEOUT_CONST) f_timeout();
	}
	last_cte = 0;
	_90_turn = 0;
	fwd(motor_speed, motor_speed);
}