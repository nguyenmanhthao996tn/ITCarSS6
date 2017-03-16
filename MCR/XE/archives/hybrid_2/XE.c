#define  DEBUG
#include "helper.h"
#include "pid.h"
#include "functions.h"

pidData_t steer;
void old_school_main();
void pid_main();

#define  switch_lane_constant 80
void do_switch_lane(uint8_t switch_lane) {
	int16_t servo_pos;
	
	if (switch_lane == 1) servo_pos = switch_lane_constant;
	else servo_pos = -switch_lane_constant;
	servo(servo_pos);
	switch_lane = 0;
	while (!(read_sensor() > 1));
}

int main() {
	init();
	set_led_data(8888);
	servo(0);
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

#define pid_speed_ratio 0.15

int16_t calc_motor_speed(int16_t cte) {
	uint8_t pid_motor_speed;
	
	if (abs(cte) == 3*m) return (int16_t)(motor_speed - (motor_speed / 3));
	if (cte == 0) return motor_speed;
	if (cte > 0) {
		pid_motor_speed = (int16_t)(motor_speed * ((3*m - cte)/3.0*m));
		pid_motor_speed = (int16_t)(motor_speed - (pid_speed_ratio*pid_motor_speed));
	}
	else if(cte < 0) {
		pid_motor_speed = (int16_t)(motor_speed * ((-3*m-cte)/-3.0*m));
		pid_motor_speed = (int16_t)(motor_speed - (pid_speed_ratio*pid_motor_speed));
	}
	return pid_motor_speed;
}

void pid_main() {
	int16_t cte;
	int16_t pid_output;
	uint8_t sensor_val;
	uint8_t pid_motor_speed;
	uint8_t switch_lane = 0;
	off_lane = 0;
	test_mode();
	pid_Init(K_P, K_I, K_D, &steer);
	
	fwd(motor_speed, motor_speed);
	while (1) {
		if (off_lane == 100) {
			fwd(0, 0);
			break;
		}
		sensor_val = read_sensor();
		if (sensor_val == 0 || sensor_val == 0xff) {
			off_lane += 1;
			if (switch_lane) do_switch_lane(switch_lane);
		}
		if ((sensor_val & 0b00011111) == 0b00011111) switch_lane = 1; //right switch
		if ((sensor_val & 0b11111000) == 0b11111000) switch_lane = 2; //left switch
		cte = calc_cte(sensor_val);
		pid_output = pid_Controller(0, cte, &steer);
		pid_motor_speed = calc_motor_speed(cte);
		fwd(pid_motor_speed, pid_motor_speed);
		servo(pid_output/2);
		set_led_data(switch_lane);
		//set_led_data(abs(pid_output));
	}	
}

ISR(TIMER0_COMP_vect) {
	isr_ptr();
}

ISR(INT0_vect) {
	pulse_v++;
	pulse_pid++;
}