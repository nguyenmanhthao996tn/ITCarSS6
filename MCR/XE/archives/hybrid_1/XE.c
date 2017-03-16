#include "helper.h"
#include "pid.h"
#include "functions.h"

pidData_t steer;
void old_school_main();
void pid_main();

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

void pid_main() {
	int16_t cte;
	int16_t pid_output;
	uint8_t sensor_val;
	uint8_t pid_motor_speed;
	
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
		if (sensor_val == 0 || sensor_val == 0xff) off_lane++;
		cte = calc_cte(sensor_val);
		pid_output = pid_Controller(0, cte, &steer);
		if (cte > 0) {
			pid_motor_speed = (int16_t)(motor_speed * ((3*m - cte)/3.0*m));
			pid_motor_speed = (int16_t)(motor_speed - (0.15*pid_motor_speed));
		}
		else if(cte < 0) {
			pid_motor_speed = (int16_t)(motor_speed * ((-3*m-cte)/-3.0*m));
			pid_motor_speed = (int16_t)(motor_speed - (0.15*pid_motor_speed));
		}
		else pid_motor_speed = motor_speed;
		fwd(pid_motor_speed, pid_motor_speed);
		servo(pid_output/2);
		set_led_data(abs(pid_output));
		//set_led_data(abs(cte));
	}	
}

ISR(TIMER0_COMP_vect) {
	isr_ptr();
}

ISR(INT0_vect) {
	pulse_v++;
	pulse_pid++;
}