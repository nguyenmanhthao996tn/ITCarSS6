/*
		pos = read_sensor();
		if ((pos & mask_mid) == 0) {
			if ( (pos & mask3_0) != 0 && (pos & mask0_3) != 0 ) pos = prev_pos;
			if ((pos & mask3_0) != 0) {
				while ((read_sensor() & mask_mid) != 0b00011000) {
					if (servo_pos > -150) servo_pos -= 1;
					servo(servo_pos);
				}
			}
			else if ((pos & mask0_3) != 0) while ((read_sensor() & mask_mid) != 0b00011000) {
				if (servo_pos < 150) servo_pos += 1;
				servo(servo_pos);
			}
		}
		prev_pos = pos;
		*/


/*
		if (o == 0) fwd(motor_speed, motor_speed);
		else if (o >= max_error) fwd(motor_speed/4, motor_speed/4);
		else {
			speed = (float)motor_speed * ((max_error-o)/max_error);
			fwd((int)speed, (int)speed);	
		}
		
		set_led_data(abs((int)speed));
		*/


/*

*/

	/*if (square_turn) {
			fwd(0, 0);
			break;
		}*/
		
		//if (rv == 0 || rv == 0xff) off_lane++;
		
		//if (rv == 0xff) {off_lane++; square_turn = 1;}
		//else if (rv == 0) off_lane++;
		//else if (rv == 0b00001111) switch_lane = 1;
		//else if (rv == 0b11110000) switch_lane = 2;
		
		
		
		
#define mask_11 0b10000001
#define mask_22 0b11000011
#define mask_33 0b11100111
#define mask_mid 0b00011000
#define mask3_0 0b11100000
#define mask0_4 0b00001111
#define mask0_3 0b00000111
#define motor_speed 100

int16_t last_pv = 0;
uint8_t off_lane = 0, square_turn = 0, switch_lane = 0;
enum cp {mid, left_lane, right_lane} car_pos = mid;

int16_t calc_process_val() {
	int16_t pv = 0;
	uint8_t t = 0;
	uint8_t rv;
	
	rv = read_sensor();
	
	if ((rv & mask0_3) != 0) t += 1;
	if ((rv & mask3_0) != 0) t += 2;
	
	switch (t) {
		case 1:
		for(uint8_t i = 0; i < 3; i++) {
			if (rv & (1 << i)) {
				pv = -(300 - i*100);
				last_pv = pv;
				//set_led_data(pv);
				return pv;
			}
		}
		break;
		
		case 2:
		rv >>= 5;
		for(uint8_t i = 0; i < 3; i++) {
			if (rv & (0b100 >> i)) {
				pv = (300 - i*100);
				last_pv = pv;
				//set_led_data(-pv);
				return pv;
			}
		}
		break;
		
		case 3:
		//set_led_data(abs(last_pv));
		return 0;
		break;
		
		default: //center
		//set_led_data(0);
		return 0;
		break;
	}
}



uint8_t rv;
int16_t pv, po;
int16_t set_point = 0;

set_led_data(1338);
pid_Init(K_P, K_I, K_D, &steer);
fwd(motor_speed, motor_speed);

while (1) {
	if (off_lane == 50) {
		fwd(0, 0);
		break;
	}
	
	pv = calc_process_val();
	po = pid_Controller(set_point, pv, &steer);
	servo(po);
	set_led_data(abs(po));
}


//uint8_t s = 0, m = 0;
uint8_t idx = 0;

/*
	while (1) {
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) idx--;
		if (get_button(BTN2)) idx++;
		if (idx > 7 || idx < 0) idx = 0;
		set_led_data(read_adc(idx));
		led_data.sensor_debug_output = 1 << idx;
	}*/
	/**
	set_led_data(8888);
	while (1) {
		read_sensor();
		
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) m += 1;
		if (get_button(BTN2)) s += 1;		

		m %= 2;
		s %= 3;
		
		switch (m) {
			case 0:
				fwd(0, 0);
				break;
			case 1:
				fwd(50, 50);
				break;
			case 2:
				bwd(-50, -50);
				break;
		}
		
		switch (s) {
			case 0:
				servo(0);
				break;
			case 1:
				servo(150);
				break;
			case 2:
				servo(-150);
				break;
		}
	}
	*/
	
	
	/*This file has been prepared for Doxygen automatic documentation generation.*/   
/*! \file *********************************************************************  
 *  
 * \brief General PID implementation for AVR.  
 *  
 * Discrete PID controller implementation. Set up by giving P/I/D terms  
 * to Init_PID(), and uses a struct PID_DATA to store internal values.  
 *  
 * - File:               pid.c  
 * - Compiler:           ATMEL STUDIO  
 * - Supported devices:  All AVR devices can be used.  
 * - AppNote:            AVR221 - Discrete PID controller  
 *  
 * \author               Atmel Corporation: http://www.atmel.com \n  
 *                       Support email: avr@atmel.com  
 *  
 * $Name: RELEASE_1_0 $  
 * $Revision: 1.1 $  
 * $RCSfile: pid.c,v $  
 * $Date: 2006/02/16 11:46:13 $  
 *****************************************************************************/ 

#include "stdint.h" 

#define SCALING_FACTOR		128
#define TIME_INTERVAL_PID   100	//ms

typedef struct PID_DATA{ 
  //! Last process value, used to find derivative of process value. 
  int16_t lastProcessValue; 
  //! Summation of errors, used for integrate calculations 
  int32_t sumError; 
  //! The Proportional tuning constant, multiplied with SCALING_FACTOR 
  int16_t P_Factor; 
  //! The Integral tuning constant, multiplied with SCALING_FACTOR 
  int16_t I_Factor; 
  //! The Derivative tuning constant, multiplied with SCALING_FACTOR 
  int16_t D_Factor; 
  //! Maximum allowed error, avoid overflow 
  int16_t maxError; 
  //! Maximum allowed sumerror, avoid overflow 
  int32_t maxSumError; 
} pidData_t; 
 
// Maximum value of variables 
#define MAX_INT         INT16_MAX 
#define MAX_LONG        INT32_MAX 
#define MAX_I_TERM      (MAX_LONG / 2) 
 
// Boolean values 
#define FALSE           0 
#define TRUE            1 

/*! \brief Initialisation of PID controller parameters.  
 *  
 *  Initialise the variables used by the PID algorithm.  
 *  
 *  \param p_factor  Proportional term.  
 *  \param i_factor  Integral term.  
 *  \param d_factor  Derivate term.  
 *  \param pid  Struct with PID status.  
 */ 
void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid)
{
	 // Start values for PID controller
	 pid->sumError = 0;
	 pid->lastProcessValue = 0;
	 // Tuning constants for PID loop
	 pid->P_Factor = p_factor;
	 pid->I_Factor = i_factor;
	 pid->D_Factor = d_factor;
	 // Limits to avoid overflow
	 pid->maxError = MAX_INT / (pid->P_Factor + 1);
	 pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}

/*! \brief PID control algorithm.  
 *  
 *  Calculates output from setpoint, process value and PID status.  
 *  
 *  \param setPoint  Desired value.  
 *  \param processValue  Measured value.  
 *  \param pid_st  PID status struct.  
 */ 
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st)
{
	int16_t error, p_term, d_term;
	int32_t i_term, ret, temp;
	
	error = setPoint - processValue;
	
	// Calculate Pterm and limit error overflow
	if (error > pid_st->maxError){
		p_term = MAX_INT;
	}
	else if (error < -pid_st->maxError){
		p_term = -MAX_INT;
	}
	else{
		p_term = pid_st->P_Factor * error;
	}
	
	// Calculate Iterm and limit integral runaway
	temp = pid_st->sumError + error;
	if(temp > pid_st->maxSumError){
		i_term = MAX_I_TERM;
		pid_st->sumError = pid_st->maxSumError;
	}
	else if(temp < -pid_st->maxSumError){
		i_term = -MAX_I_TERM;
		pid_st->sumError = -pid_st->maxSumError;
	}
	else{
		pid_st->sumError = temp;
		i_term = pid_st->I_Factor * pid_st->sumError;
	}
	
	// Calculate Dterm
	d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);
	
	pid_st->lastProcessValue = processValue;
	
	ret = (p_term + i_term + d_term) / SCALING_FACTOR;
	if(ret > MAX_INT){
		ret = MAX_INT;
	}
	else if(ret < -MAX_INT){
		ret = -MAX_INT;
	}
	
	return((int16_t)ret);
}

/*! \brief Resets the integrator.  
 *  
 *  Calling this function will reset the integrator in the PID regulator.  
 */  
void pid_Reset_Integrator(pidData_t *pid_st)
{
	pid_st->sumError = 0;   
}

	
	cbi(PORTD,DIR0);
	OCR1B=left*200;
	cbi(PORTD,DIR1);
	OCR2=right*255/100;
	
	
	/*
	uint8_t pattern;
	off_lane = 0;
	my_timer = 0;
	while(1) {
		if (get_button(BTN1)) break;
		read_sensor();
	}
	
	while(1) {
		pattern = 1;
		while(1) {
			
			switch(pattern) {
				case 1:
					if(check_crossline()) { //90 degree turn
						pattern = 21;
					}
					break;
				
					if(check_rightline()) { //right lane
						pattern = 51;
					}
					break;
					
					if(check_leftline()) { //left lane
						pattern = 61;	
					}
					break;
					set_led_data(10);
					#ifdef DEBUG
					push_stack(10); 
					#endif
					//set_encoder(19);
					switch(read_sensor() & 0b01111110) {
						case 0b01111110:
							fwd(0,0);
							servo(0);
						break;
					
						case 0b00011000: //forward
							servo(0);
							fwd(100, 100);
						break;
					
						case 0b00011100:
						case 0b00001000:
							fwd(90,80);
							servo(8);
						break;
					
						case 0b00001100:
							fwd(90,70);
							servo(10);
						break;
					
						case 0b00001110:
						case 0b00000100:
							fwd(90,60);
							servo(12);
						break;
					
						case 0b00000110:
							fwd(90,50);
							servo(15);
						break;
					
						case 0b00000010:
							fwd(90,30);
							servo(20);
							pattern = 11;
						break;
					
						case 0b00111000:
						case 0b00010000:
							fwd(80,90);
							servo(-8);
						break;
					
						case 0b00110000:
							fwd(70,90);
							servo(-10);
						break;
					
						case 0b01110000:
						case 0b00100000:
							fwd(60,90);
							servo(-12);
						break;
					
						case 0b01100000:
							fwd(50,90);
							servo(-15);
						break;
					
						case 0b01000000:
							fwd(30,90);
							servo(-20);
							pattern = 12;
						break;
					
						default:
						break;
					}
				break;
				
				case 11:
					set_led_data(11);
					#ifdef DEBUG
					push_stack(11);
					#endif
					//set_encoder(23);
					switch(read_sensor() & 0b11001100) {
						case 0b11000000:
							fwd(80, -10);
							servo(70);
						break;
					
						case 0b10000000:
							fwd(80, 5);
							servo(47);
						break;
					
						case 0b00000000:
							fwd(80, 10);
							servo(45);
						break;
					
						case 0b00000100:
							fwd(80, 15);
							servo(34);
						break;
					
						case 0b00001100:
							fwd(80,20);
							servo(26);
							pattern = 1;
						break;
					
						default:
							pattern = 11;
						break;
					}
				break;
				
				case 12:
					set_led_data(12);
					//set_encoder(23);
					#ifdef DEBUG
					push_stack(12);
					#endif
					switch(read_sensor() & 0b00110011) {
						case 0b00000011:
							fwd(-10, 80);
							servo(-100);
						break;
					
						case 0b00000001:
							fwd(5, 80);
							servo(-47);
						break;
					
						case 0b00000000:
							fwd(10, 80);
							servo(-45);
						break;
					
						case 0b00100000:
							fwd(15, 80);
							servo(-34);
						break;
					
						case 0b00110000:
							fwd(20, 80);
							servo(-26);
							pattern = 1;
						break;
					
						default:
							pattern=12;
						break;
					}
				break;
				
				case 21:
					set_led_data(21);
					//set_encoder(20);
					#ifdef DEBUG
					push_stack(21);
					#endif
					servo(0);
					fwd(60 ,60);
					pattern = 22;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 22:
					set_led_data(22);
					//set_encoder(20);
					#ifdef DEBUG
					push_stack(2);
					#endif
					servo(0);
					fwd(60 ,60);
					if( cnt1 > 70 || pulse_v > 10 ) {
						pattern = 23;
						cnt1 = 0;
						pulse_v = 0;
					}
				break;
				
				case 23:
					set_led_data(23);
					#ifdef DEBUG
					push_stack(23);
					#endif
					if( (pulse_v > 100) && ((read_sensor() == 0b11111000)  || (read_sensor()==0b11110000) || (read_sensor()==0b11100000) || (read_sensor() == 0b11111100))) {
						pattern = 24;
						cnt1 = 0;
						break;
					}
				
					if(  (pulse_v > 100) &&   ((read_sensor() == 0b00011111 ) || (read_sensor() == 0b00000111) || (read_sensor() == 0b00001111) || (read_sensor() == 0b00111111))) {
						pattern = 25;
						cnt1 = 0;
						break;
					}
				
					switch(read_sensor() & 0b01111110) {
						case 0b00011000:
							servo(0);
							fwd(20, 20);
						break;
					
						case 0b00001000:
							fwd(23, 3);
							servo(8);
						break;
					
						case 0b00001100:
							fwd(23,-3);
							servo(10);
						break;
					
						case 0b00000100:
							fwd(23,-8);
							servo(13);
						break;
					
						case 0b00000110:
						case 0b00000010:
							fwd(23,-13);
							servo(18);
						break;
					
						case 0b00010000:
							fwd(5,20);
							servo(-8);
						break;
					
						case 0b00110000:
							fwd(0, 20);
							servo(-10);
						break;
					
						case 0b00100000:
							fwd(-5,20);
							servo(-13);
						break;
					
						case 0b01100000:
						case 0b01000000:
							fwd(10,20);
							servo(-18);
						break;
					
						default:
						break;
					}
				break;
				
				case 24:
					//set_encoder(18);
					fwd(50, 50);
					if ( (pulse_v > 100) && (((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111))) {
						pattern = 23 ;
						break;
					}
					if (cnt1 > 50 || pulse_v > 10) {
						pattern = 26;
						break;
					}
					break;
					
					case 25:
					//set_encoder(18);
					fwd(50,50);
						if ( (pulse_v > 100 ) && (((read_sensor() & 0b11100000) == 0b11100000) || ((read_sensor() & 0b11110000) == 0b11110000))) {
							pattern = 23 ;
							break;
						}
						if (cnt1 > 50 || pulse_v > 10) {
							pattern = 27;
							break;
						}
					break;
				
				case 26:
					set_led_data(24);
					servo(-150);
					//set_encoder(8);
					#ifdef DEBUG
					push_stack(24);
					#endif
					fwd(-30, 90);
					pattern = 31;
					cnt1 = 0;
				break;
				
				case 27:
					set_led_data(25);
					#ifdef DEBUG
					push_stack(25);
					#endif
					servo(150);
					//set_encoder(8);
					fwd(90, -30);
					pattern = 41;
					cnt1 = 0;
				break;
				
				case 31:
					set_led_data(31);
					#ifdef DEBUG
					push_stack(31);
					#endif
					if(cnt1 > 250) {
						pattern = 32;
						cnt1 = 0;
					}
				break;
				
				case 32:
					set_led_data(32);
					#ifdef DEBUG
					push_stack(32);
					#endif
					if( (read_sensor() & 0b11100111) == 0b00100000 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 41:
					set_led_data(41);
					#ifdef DEBUG
					push_stack(41);
					#endif
					if( cnt1 > 200 ) {
						pattern = 42;
						cnt1 = 0;
					}
				break;
				
				case 42:
					set_led_data(42);
					#ifdef DEBUG
					push_stack(42);
					#endif
					if( (read_sensor() & 0b11100111) == 0b00000100 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 51:
					//set_encoder(20);
					if ((read_sensor(0b11100000) == 0b11100000) || (read_sensor(0b11110000) == 0b11110000)) {
						pattern = 21 ;
						break;
					}
					set_led_data(51);
					#ifdef DEBUG
					push_stack(51);
					#endif
					servo(0);
					fwd(70 , 70);
					pattern = 52;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 52:
					if( cnt1 > 100 || pulse_v>10) {
						pattern = 53;
						cnt1 = 0;
					}
					if (((read_sensor() & 0b11100000) == 0b11100000) || ((read_sensor() & 0b11110000) == 0b11110000)) {
						pattern = 21 ;
						break;
					}
				break;

				case 53:
				if( read_sensor() == 0b00000000 ) {
					servo(70);
					fwd(50, 60);
					pattern = 54;
					cnt1 = 0;
					break;
				}
				switch( read_sensor() & 0b11100111 ) {
					case 0:
						servo( 0 );
						fwd( 30 ,30 );
						break;
						case 4:
						case 6:
						case 7:
						case 3:
						servo(15);
						fwd(30, 20);
					break;
					case 0x20:
					case 0x60:
					case 0xe0:
					case 0xc0:
						servo(-15);
						fwd(20, 30);
					break;
					
					default:
					break;
				}
				break;

				case 54:
					if( (read_sensor() & 0b00001100) == 0b00001100 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 61:
					//set_encoder(20);
					if ( ((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111) ) {
						pattern = 21 ;
						break;
					}
					set_led_data(61);
					#ifdef DEBUG
					push_stack(61);
					#endif
					servo(0);
					fwd(90, 90);
					pattern = 62;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 62:
					if( cnt1 > 100 || pulse_v > 10) {
						pattern = 63;
						cnt1 = 0;
					}
					if (((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111)) {
						pattern = 21 ;
						break;
					}
				break;

				case 63:
					if( read_sensor() == 0 ) {
						servo(-70);
						fwd(60, 40);
						pattern = 64;
						cnt1 = 0;
					}
				break;
				
				switch( read_sensor() & 0b11100111 ) {
					case 0:
						servo(0);
						fwd(30, 30);
					break;
					case 4:
					case 6:
					case 7:
					case 3:
						servo( 15 );
						fwd( 30 ,20 );
					break;
					case 0x20:
					case 0x60:
					case 0xe0:
					case 0xc0:
						servo(-15);
						fwd(20, 30);
					break;
					
					default:
					break;
				}
				break;

				case 64:
					if( (read_sensor() & 0b00110000) == 0b00110000 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;
				
				case 71:
					//set_encoder(30);
					set_led_data(71);
					#ifdef DEBUG
					push_stack(71);
					#endif
					fwd(100, 100);
					if( cnt1 > 100 || pulse_v > 10) {
						pattern = 72;
						cnt1 = 0;
					}
				break;
				
				case 72:
					if (read_sensor() == 0b00000000) {
						servo(0);
						fwd(100,100);
						pattern=73;
						break;
					}
				
					switch(read_sensor() & 0b01111110) {
						case 0b00011000:
							servo(0);
							fwd(100,100 );
						break;
					
						case 0b00001000:
							fwd(90,17);
						servo(8);
						break;
					
						case 0b00001100:
							fwd(90,70);
							servo(13);
						break;
					
						case 0b00000100:
							fwd(90,65);
							servo(18);
						break;
					
						case 0b00000110:
						case 0b00000010:
							fwd(90,60);
							servo(26);
						break;
					
						case 0b00010000:
							fwd(75,90);
							servo(-8);
						break;
						case 0b00110000:
							fwd(70,90);
							servo(-13);
						break;
					
						case 0b00100000:
							fwd(65,90);
							servo(-18);
						break;
					
						case 0b01100000:
						case 0b01000000:
							fwd(60,90);
							servo(-26);
						break;
					
						default:
						break;
				}
				break;
				
				case 73:
					servo(0);
					fwd(100, 100);
					if (read_sensor() > 0) {
						pattern = 1;
					}
				break;

				default:
					pattern = 1;
				break;
			}
		}
	}
	*/
	
if (off_lane == 100) {
	fwd(0, 0);
	break;
}
sensor_val = read_sensor();

if (sensor_val == 0) {
	if (_90_turn == 1) _90_turn = 0;
	if (switch_lane) do_switch_lane();
}
else if (sensor_val == 0xff) {
	off_lane += 1;
	if (!_90_turn) {
		_90_turn = 1;
		fwd(_90_turn_const, _90_turn_const);
		switch_lane = 0;
	}
}
else if (sensor_val == 0b00000111 || sensor_val == 0b00001111 || sensor_val == 0b00011111) {
	if (_90_turn == 0) {
		if (!switch_lane)
		switch_lane = 1; //right switch
		} else {
		do_90_right_turn();
	}
}
else if (sensor_val == 0b11100000 || sensor_val == 0b11110000 || sensor_val == 0b11111000) {
	if (_90_turn == 0) {
		if (!switch_lane)
		switch_lane = 2; //left switch
		} else {
		do_90_left_turn();
	}
}
else {
	//cte = calc_cte(sensor_val);
	cte = calc_cte(sensor_val);
	pid_output = pid_Controller(0, cte, &steer);
	pid_motor_speed = calc_motor_speed(cte);
	fwd(pid_motor_speed, pid_motor_speed);
	servo(pid_output/2);
}

uint8_t pid_motor_speed;

if (abs(cte) == 3*m) return (int16_t)(motor_speed - (motor_speed / 3));
if (cte == 0) return (uint8_t)(motor_speed * 1.5);
if (cte > 0) {
	pid_motor_speed = (int16_t)(motor_speed * ((3*m - cte)/3.0*m));
	pid_motor_speed = (int16_t)(motor_speed - (pid_speed_ratio*pid_motor_speed));
}
else if(cte < 0) {
	pid_motor_speed = (int16_t)(motor_speed * ((-3*m-cte)/-3.0*m));
	pid_motor_speed = (int16_t)(motor_speed - (pid_speed_ratio*pid_motor_speed));
}
return pid_motor_speed;
return motor_speed;


int16_t calc_cte(uint8_t sensor_val) { //calculate cross track error
	uint8_t t = 0;

	if (sensor_val == 0b00011000 || sensor_val == 0b00010000 || sensor_val == 0b00001000 || sensor_val == 0xff) { //if the car is in the middle of the track return 0
		last_cte = 0;
		return 0;
	}
	if ( (sensor_val & mask3_0) != 0) t += 1;
	if ( (sensor_val & mask0_3) != 0) t += 2;
	
	switch (t) {
		case 1: //right
		if (last_cte >= 0) return msb_first(sensor_val >> 5);
		else return lsb_first(sensor_val >> 5) - 160; /* change from 40 -> 60*/
		break;
		
		case 2: //left
		if (last_cte <= 0) return lsb_first(sensor_val);
		else return msb_first(sensor_val) + 160; /* change from 40 -> 60*/
		break;
		
		case 3:
		return last_cte;
		break;
		
		default:
		return 0;
		break;
	}
}