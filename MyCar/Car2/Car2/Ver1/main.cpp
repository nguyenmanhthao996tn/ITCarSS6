/*
 * Ver1.cpp
 *
 * Created: 3/10/2017 3:14:01 AM
 * Author: ThaoNguyen
 */ 

#include "function.h"

bool check_crossline( void );
bool check_rightline( void );
bool check_leftline( void );

uint8_t pattern = 10;
uint8_t sensor = 0x00;
uint16_t timer_cnt, encoder_pulse;

int16_t tmp = 0, tmp2;

int main(void)
{
	INIT();
	sel_mode();
	
	while (true)
	{
		sensor_cmp();
		led7(encoder_pulse);
		if (get_button(BTN0)) encoder_pulse = 0;
		if (get_button(BTN1)) break;
	}
	
	pattern = 10; /* Chay thang */
	set_encoder(-1);
	
    while (true)
    {
        switch (pattern)
		{
			/* Chay thang */
			case 10:
				led7(10);
				
				if (check_crossline())     /* Cua vuong */
				{
					pattern = 21;
					break;
				}
				else if (check_leftline()) /* Chuyen lan trai */
				{
					pattern = 51;
					timer_cnt = 0;
					encoder_pulse = 0;
					break;
				}
				else if (check_rightline()) /* Chuyen lan phai */
				{
					pattern = 61;
					timer_cnt = 0;
					encoder_pulse = 0;
					break;
				}
				
				switch (sensor_cmp() & 0b01111110)
				{
					case 0b01111110:
					break;
					
					case 0b00011000: /* Chay thang */
						handle(0);
						speed(100 ,100);
					break;
					
					case 0b00011100:
					case 0b00001000:
						speed(100,100);
						handle(9);
					break;
					case 0b00001100:
						speed(100,100);
						handle(17);
					break;
					case 0b00001110:
					case 0b00000100:
						speed(100,85);
						handle(31);
					break;
					case 0b00000110:
						speed(100,80);
						handle(50);
					break;
					case 0b00000010:
						speed(100,70);
						handle(75);
						pattern=11;	/* Lech phai goc lon */
					break;		
					
					case 0b00111000:
					case 0b00010000:
						speed(100,100);
						handle(-9);
					break;
					case 0b00110000:
						speed(100,100);
						handle(-17);
					break;
					case 0b01110000:
					case 0b00100000:
						speed(85,100);
						handle(-31);
					break;
					case 0b01100000:
						speed(80,100);
						handle(-50);
					break;
					case 0b01000000:
						speed(70,100);
						handle(-75);
						pattern=12; /* Lech trai goc lon */
					break;
					
					default:
					break;
				}
			break; /* case 10 */
			
			/* Lech phai goc lon */
			case 11:
				led7(11);
				set_encoder(12);
				switch (sensor_cmp() & 0b11001100)
				{
					case 0b11000000:
						speed(80, /*0*/-10);
						handle(105);
					break;
					
					case 0b10000000:
						speed(80, /*15*/5);
						handle(90);
					break;
					
					case 0b00000000:
						speed(80, /*20*/10);
						handle(78);
					break;
					
					case 0b00000100:
						speed(80, /*25*/15);
						handle(60);
					break;
					
					case 0b00001100:
						speed(80, /*30*/20);
						handle(52);
						pattern = 10;
						set_encoder(-1);
					break;
					
					case 0b01100000:
					pattern=12;
					break;
					
					default:
						pattern=11;
					break;
				}
			break; /* case 11 */
			
			/* Lech trai goc lon */
			case 12:
				led7(12);
				set_encoder(12);
				switch(sensor_cmp() & 0b00110011)
				{
					case 0b00000011:
						speed(-10,80);
						handle(-105);
					break;
					
					case 0b00000001:
						speed(5,80);
						handle(-90);
					break;
					
					case 0b00000000:
						speed(10,80);
						handle(-78);
					break;
					
					case 0b00100000:
						speed(15,80);
						handle(-60);
					break;
					
					case 0b00110000:
						speed(20,80);
						handle(-52);
						pattern = 10;
					break;
					
					case 0b00000110:
					pattern=11;
					break;
					
					default:
						pattern=12;
					break;
				}
			break; /* case 12 */
			
			/* Cua vuong */
			case 21:
				led7(21);
				
				handle(0);
				speed(-40,-40);
				timer_cnt = 0;
				encoder_pulse = 0;
				pattern = 22;
			break; /* case 21 */
			
			case 22:
				led7(22);
				
				//if( timer_cnt > (285 - (delay * 200)) || encoder_pulse > 35 )
				if (speed_wait(2))
				{
					speed(50, 50);
					pattern = 23;
					timer_cnt = 0;
					encoder_pulse = 0;
				}
				
				/* Nguoc lai thi chinh thang cho xe */
				switch (sensor_cmp() & 0b01111110)
				{
					case 0b01111110:
					break;
					
					case 0b00011000: /* Chay thang */
					handle(0);
					break;
					
					case 0b00011100:
					case 0b00001000:
					handle(9);
					break;
					
					case 0b00001100:
					handle(17);
					break;
					
					case 0b00001110:
					case 0b00000100:
					handle(31);
					break;
					
					case 0b00000110:
					handle(50);
					break;
					
					case 0b00000010:
					handle(75);
					break;
					
					case 0b00111000:
					case 0b00010000:
					handle(-9);
					break;
					
					case 0b00110000:
					handle(-17);
					break;
					
					case 0b01110000:
					case 0b00100000:
					handle(-31);
					break;
					
					case 0b01100000:
					handle(-50);
					break;
					
					case 0b01000000:
					handle(-75);
					break;
					
					default:
					break;
				}
			break; /* case 22 */
			
			case 23:
				led7(23);
				
				sensor = sensor_cmp();
				/* Cua trai */
				if ((encoder_pulse > 12) && (((sensor & 0b11111000) == 0b11111000) || ((sensor & 0b11110000) == 0b11110000) || ((sensor & 0b11100000) == 0b11100000) || ((sensor & 0b11111100) == 0b11111100)))	/* Neu gap tin hieu nay la goc cua 90 trai thi be */
				{
					pattern = 26;
					timer_cnt=0;
					encoder_pulse = 0;
					break;
				}
				/* Cua phai */
				if ((encoder_pulse > 12) && (((sensor & 0b00011111) == 0b00011111 ) || ((sensor & 0b00000111) == 0b00000111) || ((sensor & 0b00001111) == 0b00001111) || ((sensor & 0b00111111) == 0b00111111))) /* Neu gap tin hieu nay la goc cua 90 phai thi be */
				{
					pattern = 27;
					timer_cnt=0;
					encoder_pulse = 0;
					break;
				}
				/* No line */
				if ((sensor & 0b01111110) == 0x00)
				{
					pattern = 73;
					handle(0);
					speed(100, 100);
					timer_cnt = 0;
					encoder_pulse = 0;
				}
				
				/* Nguoc lai thi chinh thang cho xe */
				switch (sensor_cmp() & 0b01111110)
				{
					case 0b01111110:
					break;
					
					case 0b00011000: /* Chay thang */
					handle(0);
					break;
					
					case 0b00011100:
					case 0b00001000:
					handle(9);
					break;
					
					case 0b00001100:
					handle(17);
					break;
					
					case 0b00001110:
					case 0b00000100:
					handle(31);
					break;
					
					case 0b00000110:
					handle(50);
					break;
					
					case 0b00000010:
					handle(75);
					break;
					
					case 0b00111000:
					case 0b00010000:
					handle(-9);
					break;
					
					case 0b00110000:
					handle(-17);
					break;
					
					case 0b01110000:
					case 0b00100000:
					handle(-31);
					break;
					
					case 0b01100000:
					handle(-50);
					break;
					
					case 0b01000000:
					handle(-75);
					break;
					
					default:
					break;
				}
			break; /* case 23 */
			
			case 26: /* trai */
				led7(26);
				
				handle(-SERVO_ANGLE_MAX); /* -120 */
				speed( -100 , 80 );
				pattern = 31;
				timer_cnt = 0;
				encoder_pulse = 0;
			break; /* case 26 */
			
			case 27: /* phai */
				led7(27);
				
				handle(SERVO_ANGLE_MAX); /* 120 */
				speed( 80 , -100 );
				pattern = 41;
				timer_cnt = 0;
				encoder_pulse = 0;
			break; /* case 27 */
			
			case 31:
				led7(31);
				
				if( timer_cnt > 200 ) /* 200ms */
				{
					pattern = 32;
					timer_cnt = 0;
					encoder_pulse = 0;
				}
			break; /* case 31 */
			
			case 32:
				led7(32);
				sensor = sensor_cmp();
				if((sensor & 0b11100111) == 0b00100000)
				{
					pattern = 10;
				}
			break; /* case 32 */
			
			case 41:
				led7(41);
				
				if( timer_cnt > 200 ) /* 200ms */
				{
					pattern = 42;
					timer_cnt = 0;
				}
			break; /* case 41 */
			
			case 42:
				led7(42);
				sensor = sensor_cmp();
				if((sensor & 0b11100111) == 0b00000100) 
				{
					pattern = 10;
				}
			break; /* case 42 */
			
			/* Chuyen lan trai */
			case 51:
				led7(51);
				
				sensor = sensor_cmp();
				if (((sensor & 0b00000111) == 0b00000111) || ((sensor & 0b00001111) == 0b00001111) || ((sensor & 0b00011111) == 0b00011111))
				{
					pattern = 21;
					break;
				}
				
				speed(100, 100);
				
				if (encoder_pulse >= 25 || timer_cnt >= 120)
				{
					pattern = 52;
					timer_cnt = 0;
					encoder_pulse=0;
				}
			break; /* case 51 */
			
			case 52:
				led7(52);
				
				handle(-50);
				speed( 80 ,100 );
				pattern = 53;
				timer_cnt = 0;
				encoder_pulse = 0;
			break; /* case 52 */
			
			case 53:
				led7(53);
				
				sensor = sensor_cmp();
				if(((encoder_pulse > 100) || (timer_cnt > 200 * delay)) && ((sensor & 0b00110000 ) == 0b00110000))
				{
					pattern = 10;
				}
			break; /* case 53 */
			
			/* Chuyen lan phai */
			case 61:
				led7(61);
				sensor = sensor_cmp();
				if (((sensor & 0b11100000) == 0b11100000) || ((sensor & 0b11110000) == 0b11110000) || ((sensor & 0b11111000) == 0b11111000))
				{
					pattern = 21 ;
					break;
				}
				
				speed(100, 100);
				
				if (encoder_pulse >= 25 || timer_cnt >= 120)
				{
					pattern = 62;
					timer_cnt = 0;
					encoder_pulse=0;
				}
			break; /* case 61 */
			
			case 62:
				led7(62);
				
				handle(50);
				speed( 100 ,80 );
				pattern = 63;
				timer_cnt = 0;
				encoder_pulse = 0;
			break; /* case 62 */
			
			case 63:
				led7(63);
				
				sensor = sensor_cmp();
				if(((encoder_pulse > 100) || (timer_cnt > 200 * delay)) && ((sensor & 0b00110000 ) == 0b00110000))
				{
					pattern = 10;
				}
			break; /* case 63 */
			
			/* No line */
			case 73:
			led7(73);
			set_encoder(12);
			speed(60,60);
			sensor = sensor_cmp();
			if ((sensor & 0b11000000) == 0b11000000) handle(40);
			if ((sensor & 0b10000000) == 0b10000000) handle(30);
			if ((sensor & 0b00000011) == 0b00000011) handle(-40);
			if ((sensor & 0b00000001) == 0b00000001) handle(-30);
			if ((sensor & 0b00111100) != 0)
			{
				pattern = 10;
				set_encoder(-1);
			}
			break; /* case 73 */
			
			default:
				pattern = 10;
			break; /* default */
		}
    }
}

ISR(TIMER0_COMP_vect) /* 1ms */
{
	print();
	cal_ratio();
	timer_cnt++;
}

ISR(INT0_vect) /* encoder */
{
	encoder_pulse++;
	pulse_ratio++;
}

bool check_crossline( void )
{
	sensor = sensor_cmp();
	return (((sensor & 0b11111111) == 0b11111111) || ((sensor & 0b01111110) == 0b01111110));
}
bool check_rightline( void )
{
	sensor = sensor_cmp();
	return (((sensor & 0b00001111) == 0b00001111) || ((sensor & 0b00011111) == 0b00011111));
}
bool check_leftline( void )
{
	sensor = sensor_cmp();
	return (((sensor & 0b11110000) == 0b11110000) || ((sensor & 0b11111000) == 0b11111000));
}