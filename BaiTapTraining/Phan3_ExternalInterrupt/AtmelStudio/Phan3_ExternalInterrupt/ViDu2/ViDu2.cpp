/*
 * ViDu2.cpp
 *
 * Created: 03/07/2016 4:39:27 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define sbi(port, bit) (port) |= (1 << bit)
#define cbi(port, bit) (port) &= ~(1 << bit)

volatile int8_t led = 0;

int main(void)
{
	// Khoi tao chan ra LED
	DDRA = 0xff;
	PORTA = 0b00000001;
	
	// Khoi tao Interrupt
	DDRD &= 0b11110011;
	PORTD = 0b00001100;
	MCUCR |= (1 << CS00) | (1 << CS01) | (1 << CS10) | (1 << CS11);
	GICR |= (1 << INT0) | (1 << INT1);
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

ISR(INT0_vect)
{
	if (++led > 7) led = 0;
	PORTA = 1 << led;
}

ISR(INT1_vect)
{
	if (--led < 0) led = 7;
	PORTA = 1 << led;
}