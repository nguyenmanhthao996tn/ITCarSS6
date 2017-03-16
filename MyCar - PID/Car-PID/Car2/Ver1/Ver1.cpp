/*
 * main1.cpp
 *
 * Created: 3/3/2017 3:36:26 PM
 *  Author: ThaoNguyen
 */
#include "pid.h"
#include "function.h"

int check_crossline( void );
int check_rightline( void );
int check_leftline( void );

uint8_t  pattern;
uint16_t cnt1, pulse_v;
extern int16_t pulse_ratio;

/* PID */
uint8_t PID_Counter = 0;
int16_t PID_Value = 0, sensorValue = SERVO_CENTER;
PID_DATA PID_Data;

int main( void )
{
	pid_Init(1, 1, 1, &PID_Data);
	
	INIT();
	sel_mode();
	
	Servo_Calibrate();
	
	while (1)
	{
		sensorValue = sensor_cmp2();
		led7(PID_Value);
	}
	
	return 0;
}

ISR(TIMER0_COMP_vect)
{
	cnt1++;
	//cal_ratio();
	print();
	
	if (++PID_Counter >= 49)
	{
		PID_Value = SERVO_CENTER - pid_Controller(SERVO_CENTER, sensorValue, &PID_Data);
		OCR1A = PID_Value;
		PID_Counter = 0;
	}
}
ISR(INT0_vect)
{
	pulse_v++;
	pulse_ratio++;
	pul++;
}
int check_crossline( void )
{
	int ret=0;
	if(( sensor_cmp(0b11111111)==0b11111111) || (sensor_cmp(0b01111110)==0b01111110))ret = 1;
	return ret;
}
int check_rightline( void )
{
	int ret=0;
	if( (sensor_cmp(0b00001111) == 0b00001111) || (sensor_cmp(0b00011111)==0b00011111))  ret = 1;
	return ret;
}
int check_leftline( void )
{
	int ret=0;
	if( (sensor_cmp(0b11110000) == 0b11110000) || (sensor_cmp(0b11111000)== 0b11111000)) ret = 1;
	return ret;
}