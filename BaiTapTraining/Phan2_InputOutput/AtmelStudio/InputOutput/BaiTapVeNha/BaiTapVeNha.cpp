/*
 * BaiTapVeNha.cpp
 *
 * Created: 02/07/2016 10:49:11 AM
 *  Author: ThaoNguyen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD |= 0b00001111;
	PORTD = 0;
	
	uint8_t counter = 0;
	
    while(1)
    {
        //TODO:: Please write your application code 
		counter++;
		if (counter >= 16) counter = 0;
		PORTD = counter;
		_delay_ms(1000);
    }
}