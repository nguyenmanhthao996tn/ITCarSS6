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