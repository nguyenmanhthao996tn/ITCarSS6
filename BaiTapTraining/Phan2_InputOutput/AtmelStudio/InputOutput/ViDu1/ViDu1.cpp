/*
 * ViDu1.cpp
 *
 * Created: 02/07/2016 9:27:23 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define sbi(port, bit) (port) |= (1 << bit)
#define cbi(port, bit) (port) &= ~(1 << bit)

int main(void)
{
	DDRC |= 0b00000100; // sbi(DDRC, 2);
	PORTC = 0;
	
    while(1)
    {
        //TODO:: Please write your application code 
		PORTC |= 0b00000100; // sbi(PORTC, 2);
		_delay_ms(500);
		PORTC &= 0b11111011; // cbi(PORTC, 2);
		_delay_ms(500);
    }
}