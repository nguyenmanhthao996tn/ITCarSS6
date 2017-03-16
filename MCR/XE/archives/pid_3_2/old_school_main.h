uint8_t sign = 0;

void adjust_servo(int16_t servo_pos) {
	if (sign) servo(-servo_pos);
	else servo(servo_pos);
}

void old_school_main() {
	uint8_t sensor_val;
	int16_t cte = 0;
	
	
	m = 100;
	while (1) {
		sensor_val = read_sensor();
		cte = calc_cte(sensor_val);
		set_led_data(abs(cte));
		
		if (cte == 0) fwd(motor_speed, motor_speed);
		else if (cte > 0) sign = 1;
		else if (cte < 0) sign = 0;
		cte = abs(cte);
		
		if (cte < 100) adjust_servo(20);
		else if (cte < 200) {
			adjust_servo(40);
			fwd(motor_speed/2, motor_speed/2);
		}
		else if (cte < 300) {
			adjust_servo(50);
			fwd(motor_speed/2, motor_speed/2);
		}
		else if (cte < 400) {
			adjust_servo(80);
			fwd(motor_speed/2, motor_speed/2);
		}
	}
}