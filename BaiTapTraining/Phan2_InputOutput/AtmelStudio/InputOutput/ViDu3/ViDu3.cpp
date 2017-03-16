/*
 * ViDu3.cpp
 *
 * Created: 02/07/2016 10:01:56 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define sbi(port, bit) (port) |= (1 << bit)
#define cbi(port, bit) (port) &= (1 << bit)

#define BUTTON1 0
#define BUTTON2 1

int main(void)
{
	// INITIAL SECTION
	cbi(DDRC, BUTTON1);
	cbi(PORTC, BUTTON1);
	cbi(DDRC, BUTTON2);
	cbi(PORTC, BUTTON1);
	
	DDRD = 0xff;
	PORTD = 0;
	
	int8_t led = 0;
	
    while(1)
    {
        //TODO:: Please write your application code 
		if (PINC & (1 << BUTTON1))
		{
			_delay_ms(50);
			if ((PINC & (1 << BUTTON1)) == 0)
			{
				led++;
				if (led >= 8) led = 0;
			}
		}
		if (PINC & (1 << BUTTON2))
		{
			_delay_ms(50);
			if ((PINC & (1 << BUTTON2)) == 0)
			{
				led--;
				if (led <= -1) led = 7;
			}
		}
		PORTD = (1 << led);
    }
}