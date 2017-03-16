/*
 * ViDu1.cpp
 *
 * Created: 03/07/2016 4:18:06 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define sbi(port, bit) (port) |= (1 << bit)
#define cbi(port, bit) (port) &= ~(1 << bit)

volatile uint8_t led = 0;

int main(void)
{
	// Khoi tao chan ra led
	sbi(DDRC, 1);
	cbi(PORTC, 1);
	
	// Khoi tao INTERRUPT1
	cbi(DDRD, 3);
	sbi(PORTD, 3);
	sbi(MCUCR, ISC11);
	sbi(MCUCR, ISC10);
	sbi(GICR, INT1);
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code
    }
}

ISR(INT1_vect)
{
	led ^= 0b00000010;
	PORTC = led;
}