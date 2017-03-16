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

uint16_t start_time = 0;

PID_DATA servo;
int8_t currentSensor = 0;
int16_t valueSensor = 0;

int main(void)
{
	pid_Init(100, 7, 30, &servo);
	
	INIT();
	sel_mode();
	
	speed(80, 80);
	
	while (true)
	{
		if (valueSensor >= 0) 
		{
			led7(valueSensor);
		}
		else
		{
			led7(-valueSensor);
		}
		currentSensor = sensor_cmp2();
	}
	
	while (true)
	{
		led7(pulse_v);
		if (get_button(BTN0)) pulse_v = 0;
		if (get_button(BTN1)) break;
	}
	
	pattern = 1;
	while (true)
	{
		switch (pattern)
		{
			case 1:
			led7(1);
			set_encoder(7);
			if( check_crossline() ) // Be 90
			{
				pattern = 21;
				break;
			}
			if( check_rightline() ) // Chuyen lan phai
			{
				pulse_v = 0;
				cnt1 = 0;
				pattern = 51;
				break;
			}
			if( check_leftline() )  // Chuyen lan trai
			{
				pulse_v = 0;
				cnt1 = 0;
				pattern = 61;
				break;
			}
			
			switch(sensor_cmp(0b01111110))
			{
				/* Lech phai */
				case 0b00011000:
				handle( 0 );
				speed( 100 ,100 );
				break;
				
				case 0b00011100:
				case 0b00001000:
				speed(100,90);
				handle(16);
				break;
				
				case 0b00001100:
				speed(100,80);
				handle(24);
				break;
				
				case 0b00001110:
				case 0b00000100:
				speed(100,70);
				handle(28);
				break;
				
				case 0b00000110:
				speed(100,60);
				handle(57);
				break;
				
				case 0b00000010:
				speed(100,40);
				handle(82);
				pattern=11;
				break;
				
				/* Lech trai */
				case 0b00111000:
				case 0b00010000:
				speed(90,100);
				handle(-16);
				break;
				
				case 0b00110000:
				speed(80,100);
				handle(-24);
				break;
				
				case 0b01110000:
				case 0b00100000:
				speed(70,100);
				handle(-28);
				break;
				
				case 0b01100000:
				speed(60,100);
				handle(-57);
				break;
				
				case 0b01000000:
				speed(40,100);
				handle(-82);
				pattern=12;
				break;
				
				default:
				break;
			}
			break;
			
			/* -------------------------------------------------- */
			/* Lech phai goc lon */
			case 11:
			set_encoder(10);
			led7(11);
			switch(sensor_cmp(0b11001100))
			{
				case 0b11000000:
					speed(80, -10);
					handle(102);
				break;
				case 0b10000000:
					speed(80, 5);
					handle(87);
				break;
				case 0b00000000:
					speed(80, 10);
					handle(75);
				break;
				case 0b00000100:
					speed(80, 15);
					handle(57);
				break;
				case 0b00001100:
					speed(80, 20);
					handle(49);
					set_encoder(-1);
				pattern=1;
				break;
				default:
					pattern=11;
				break;
			}
			break;
			
			/* -------------------------------------------------- */
			/* Lech trai goc lon */
			case 12:
			led7(12);
			set_encoder(10);
			switch(sensor_cmp(0b00110011))
			{
				case 0b00000011:
				speed(-10,80);
				handle(-102);
				break;
				case 0b00000001:
				speed(5,80);
				handle(-87);
				break;
				case 0b00000000:
				speed(10,80);
				handle(-75);
				break;
				case 0b00100000:
				speed(15,80);
				handle(-58);
				break;
				case 0b00110000:
				speed(20,80);
				handle(-49);
				set_encoder(-1);
				pattern=1;
				break;
				
				default:
					pattern=12;
				break;
			}
			break;
			
			/* -------------------------------------------------- */
			/* Chuyen line phai */
			case 51:
			led7(51);
			if ((sensor_cmp(0b11100000) == 0b11100000) || (sensor_cmp(0b11110000) == 0b11110000) || (sensor_cmp(0b11111000) == 0b11111000))
			{
				pattern = 21 ;
				break;
			}
			speed(100, 100);
			if (pulse_v >= 25 || cnt1 >= 120)
			{
				pattern = 52;
				cnt1 = 0;
				pulse_v=0;
			}
			break;
			
			case 52:
			led7(52);
			handle( 35);
			speed( 100 ,80 );
			pattern = 53;
			cnt1 = 0;
			break;
			
			case 53:
			led7(53);
			if(((pulse_v > 100) || (cnt1 > 200 * delay)) && (sensor_cmp( 0b00110000 ) == 0b00110000))
			{
				pattern = 1;
				cnt1 = 0;
				led7(0);
			}
			break;
			
			/* Chuyen line trai */
			case 61:
			led7(61);
			if ( (sensor_cmp(0b00000111) == 0b00000111) || (sensor_cmp(0b00001111) == 0b00001111) || (sensor_cmp(0b00011111) == 0b00011111))
			{
				pattern = 21 ;
				break;
			}
			speed(100, 100);
			if (pulse_v >= 25 || cnt1 >= 120)
			{
				pattern = 62;
				cnt1 = 0;
				pulse_v=0;
			}
			break;
			
			case 62:
			led7(62);
			handle( -35);
			speed( 80 ,100 );
			pattern = 63;
			cnt1 = 0;
			break;
			
			case 63:
			led7(63);
			if(((pulse_v > 100) || (cnt1 > 200 * delay)) && (sensor_cmp( 0b00110000 ) == 0b00110000))
			{
				pattern = 1;
				cnt1 = 0;
				led7(0);
			}
			break;
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	cnt1++;
	//cal_ratio();
	print();
	valueSensor = pid_Controller(0, currentSensor, &servo);
	OCR1A = SERVO_CENTER - valueSensor;
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