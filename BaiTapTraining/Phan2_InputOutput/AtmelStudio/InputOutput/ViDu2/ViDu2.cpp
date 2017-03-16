/*
 * ViDu2.cpp
 *
 * Created: 02/07/2016 9:44:01 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>

#define sbi(port, bit) (port) |= (1 << bit)
#define cbi(port, bit) (port) &= ~(1 << bit)

#define BUTTON 2
#define LED 4

int main(void)
{
	// Khoi tao nut
	cbi(DDRC, BUTTON);
	cbi(PORTC, BUTTON);
	
	// Khoi tao led
	sbi(DDRD, LED);
	cbi(PORTD, LED);
	
    while(1)
    {
        //TODO:: Please write your application code 
		if (PINC & (1 << BUTTON)) // Khi nhan nut PINC = 0bxxxxx1xx, (1 << BUTTON) = 0b00000100 => PINC & (1 << BUTTON) != 0 khi nut duoc nhan
		{
			sbi(PORTD, LED);
		}
		else
		{
			cbi(PORTD, LED);
		}
    }
}