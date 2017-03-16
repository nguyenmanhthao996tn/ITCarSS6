/*
 * ViDu1.cpp
 *
 * Created: 03/07/2016 4:50:40 AM
 *  Author: ThaoNguyen
 */

 #define sbi(port, bit) (port) |= (1 << bit)
 #define cbi(port, bit) (port) &= ~(1 << bit)

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	// Khoi tao chan ra
	sbi(DDRD, 1);
	cbi(PORTD, 1);
	
	// Khoi tao timer0
	/*
	 * Chon Prescale = 64
	 *    64
	 * --------- x (255 - TCNT) = 0.001s
	 *  8000000
	 * 
	 * => TCNT = 130
	 */
	TCCR0 = (1 << CS01) | (1 << CS00); // Prescale = 64
	TCNT0 = 130;
	TIMSK |= (1 << TOIE0);
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 130;
	PORTD ^= 0b00000010;
}