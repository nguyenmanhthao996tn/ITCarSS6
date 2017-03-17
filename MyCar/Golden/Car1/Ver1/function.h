/*
* function.h
*
* Created: 3/11/2017 3:37:21 PM
*  Author: Miger
*/


#ifndef FUNCTION_H_
#define FUNCTION_H_
/* -------------------- CPU Clock -------------------- */
#define F_CPU 16000000UL

/* -------------------- Libraries -------------------- */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>

/* -------------------- Macros -------------------- */
#define cbi(port, bit) (port) &= ~(1 << (bit))
#define sbi(port, bit) (port) |=  (1 << (bit))
#define tbi(port, bit) (port) ^=  (1 << (bit))

/* -------------------- Pins define -------------------- */
#define LATCH	4
#define DATA	5
#define SCK		7
#define DIR00   0
#define DIR01   1
#define DIR10   3
#define DIR11   6

/* -------------------- Masks define -------------------- */
#define BTN0	0b11111101
#define BTN1	0b11111011
#define BTN2	0b11110111

/* -------------------- Constants define -------------------- */
//#define  SERVO_CENTER      3100
uint16_t SERVO_CENTER   =  3000;
#define  STEP			   4
#define  SERVO_ANGLE_MAX   125

/* -------------------- ADC variable -------------------- */
uint16_t ADC_average[8];
uint16_t lineTrang[8];
uint16_t lineDen[8];

/* -------------------- Led 7 struct -------------------- */
struct led7 {
	uint8_t i;
	uint8_t unit;
	uint8_t ten;
	uint8_t hundred;
	uint8_t thousand;
	uint8_t sensor_out;
} led7_data;

/* -------------------- Ratio variable -------------------- */
#define ratio_default 0.30
int16_t velocity;
uint8_t cnt_ratio;
int16_t pulse_ratio;
float ratio_base, ratio;
float delay = 0;

uint8_t cSpeed = 0xff, incCounter = 0;
int16_t cSpeedDiff = 0;

/* -------------------- BUTTON + SWITCH -------------------- */
uint8_t get_button(uint8_t keyid)
{
	if ((PINB & 0x0e) != 0x0e)
	{
		_delay_ms(80);
		if ((PINB|keyid) == keyid) return 1;
	}
	return 0;
}

uint8_t get_switch()
{
	uint8_t x=0;
	x = ~PINC;
	x = x & 0x07;
	return x;
}

uint8_t get_switch2()
{
	uint8_t x=0;
	x = ~PINC;
	x = x & 0x08;
	return x;
}

/* -------------------- RATIO + SERVO + MOTOR -------------------- */
void set_encoder(int8_t veloc)
{
	velocity = veloc;
}

void cal_ratio( void )
{
	cnt_ratio++;
	if (cnt_ratio >= 20) /* 20ms */
	{
		if (velocity < 0)
		{
			ratio = ratio_base;
		}
		else
		{
			if      (pulse_ratio < velocity / 2)    ratio = ratio_base + 0.3;
			else if (pulse_ratio < velocity)        ratio = ratio_base + 0.1;
			else if (pulse_ratio > velocity)        ratio = ratio_base - 0.35;
			else if (pulse_ratio > velocity / 2)    ratio = ratio_base - 0.25;
			else ratio = ratio_base;
			
			if (ratio <= 0) ratio = 0.1;
		}
		
		cSpeedDiff = pulse_ratio - cSpeed;
		cSpeed = pulse_ratio;
		
		pulse_ratio = 0;
		cnt_ratio = 0;
	}
}

bool speed_wait(uint8_t speed)
{
	return (cSpeed <= speed);
}

uint8_t get_speed( void )
{
	return cSpeed;
}

bool check_speed_difference(int8_t level)
{
	return (cSpeedDiff >= level);
}

int16_t get_speed_different( void )
{
	return cSpeedDiff;
}

bool check_bridge( void )
{
	if ((incCounter > 5) && (cSpeed > 0))
	{
		return true;	
	}
	return false;
}

void handle(int goc)
{
	if      (goc > SERVO_ANGLE_MAX)  goc =  SERVO_ANGLE_MAX;
	else if (goc < -SERVO_ANGLE_MAX) goc = -SERVO_ANGLE_MAX;
	
	OCR1A = SERVO_CENTER + (goc * STEP);
}

void speed(int left, int right)
{
	left  = left  *  ratio;
	right = right *  ratio;
	
	if (left >= 0)
	{
		sbi(PORTD, DIR00);
		cbi(PORTD, DIR01);
		OCR1B = left * 200;
	}
	else
	{
		cbi(PORTD, DIR00);
		sbi(PORTD, DIR01);
		OCR1B = (-left) * 200;
	}
	
	if (right >= 0)
	{
		sbi(PORTD, DIR10);
		cbi(PORTD, DIR11);
		OCR2 = right * 255/100;
	}
	else
	{
		cbi(PORTD, DIR10);
		sbi(PORTD, DIR11);
		OCR2 = (-right) * 255/100;
	}
}

void stop(void)
{
	speed(-50,-50);
	speed_wait(0);
	speed(0,0);
}

/* -------------------- LED7 -------------------- */
void SPI(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}

void led7(unsigned int num)
{
	led7_data.unit		 = (1<<7) |(unsigned int)(num%10);
	led7_data.ten		 = (unsigned int)(num%100 / 10);
	led7_data.hundred	 = (unsigned int)(num%1000 / 100);
	led7_data.thousand	 = (unsigned int)(num/ 1000);
	led7_data.thousand	|= (led7_data.thousand!=0)? 1<<4 : 0;
	led7_data.hundred	|= (led7_data.thousand!=0 || led7_data.hundred!=0)? 1<<5 : 0;
	led7_data.ten		|= (led7_data.thousand!=0 || led7_data.hundred !=0 || led7_data.ten!=0)? 1<<6 : 0;
}

void print( void )
{
	uint8_t value=0;
	if(led7_data.i++ == 4) led7_data.i=0;
	switch(led7_data.i)
	{
		case 0: value=led7_data.thousand;	break;
		case 1: value=led7_data.hundred;	break;
		case 2: value=led7_data.ten;		break;
		case 3: value=led7_data.unit;		break;
		default: break;
	}
	SPI(~led7_data.sensor_out);
	SPI(value);
	sbi(PORTB,LATCH);
	cbi(PORTB,LATCH);
}

/* -------------------- ADC -------------------- */
void read_adc_eeprom( void )
{
	for(uint8_t j=0; j<8; j++)
	{
		while(!eeprom_is_ready());
		lineTrang[j] = eeprom_read_word((uint16_t*)(j*2));
		while(!eeprom_is_ready());
		lineDen[j] = eeprom_read_word((uint16_t*)((j+8)*2));
	}
	for(uint8_t i=0; i<8; i++)
	{
		ADC_average[i]=(lineTrang[i]+lineDen[i])/2;
	}
}

void write_adc_eeprom( void )
{
	for(uint8_t j=0; j<8; j++)
	{
		while(!eeprom_is_ready());
		eeprom_write_word((uint16_t*)(j*2), (uint16_t)lineTrang[j]);
		while(!eeprom_is_ready());
		eeprom_write_word((uint16_t*)((j+8)*2), (uint16_t)lineDen[j]);
	}
}

uint16_t adc_read( uint8_t ch )
{
	ADMUX = (1<< REFS0)|ch;
	ADCSRA|=(1<<ADSC);
	while(!(ADCSRA & (1<<ADIF)));
	return ADCW;
}

uint8_t sensor_cmp( void )
{
	uint8_t ADC_value=0;
	for(uint8_t i=0; i<8; i++)
	{
		if(adc_read(i)<ADC_average[i]) sbi(ADC_value,i);
	}
	led7_data.sensor_out=ADC_value;
	return (ADC_value);
}

void learn_color( void )
{
	uint8_t sel = 0;
	uint16_t ADC_temp=0;
	for (uint8_t i = 0; i < 8; i++)
	{
		lineTrang[i] = 1024;
		lineDen[i] = 0;
	}
	
	led7(6014);
	while (1)
	{
		if(get_button(BTN2)) break;
		
		for (uint8_t i = 0; i < 8; i++)
		{
			ADC_temp = adc_read(i);
			if (ADC_temp < lineTrang[i]) lineTrang[i] = ADC_temp;
			if(ADC_temp > lineDen[i]) lineDen[i] = ADC_temp;
		}
	}
	
	for (uint8_t i=0; i<8; i++)
	{
		ADC_average[i]=(lineTrang[i]+lineDen[i])/2;
	}
	
	write_adc_eeprom();
	
	while (1)
	{
		if(get_button(BTN0))
		{
			if (++sel > 7) sel = 0;
		}
		if (get_button(BTN1))
		{
			if (--sel < 0)  sel = 7;
		}
		if (get_button(BTN2))
		{
			break;
		}
		
		led7(ADC_average[sel]);
		led7_data.sensor_out = (1 << sel);
	}
}

/* -------------------- INITIAL -------------------- */
void INIT( void )
{
	/* ADC */
	ADMUX=(1<<REFS0);
	ADCSRA=(1<<ADEN) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	read_adc_eeprom();
	
	/* PORT */
	DDRB  = 0b11110001;
	PORTB = 0b11111111;
	
	DDRC  = 0b00000000;
	PORTC = 0b11111111;
	
	DDRD  = 0b11111011;
	PORTD = 0b00000000;
	
	/* SPI */
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
	
	/* TIMER */
	TCCR0=(1<<WGM01) | (1<<CS02);
	OCR0=62;
	TIMSK=(1<<OCIE0);
	
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
	ICR1 = 20000;
	OCR1B = 0;
	
	TCCR2=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS22)|(1<<CS21)|(1<<CS20);
	OCR2=0;
	sei();
	
	/* ENCODER */
	MCUCR |= (1<<ISC00)|(1<<ISC01);
	GICR |= (1<<INT0);
	set_encoder(19);
}

void test_hardware( void )
{
	uint8_t _index=0;
	while(1)
	{
		if      (get_button(BTN0))  { speed(100,-100); handle(-SERVO_ANGLE_MAX); }
		else if (get_button(BTN1))	{ if(++_index == 8) _index=0;             }
		else if (get_button(BTN2))	{ speed(-100,100); handle(SERVO_ANGLE_MAX);  }
		else						{ speed(0,0);  handle(0);                 }
		
		led7(adc_read(_index));
		led7_data.sensor_out = 0 | (1<<_index);
	}
}

void test_servo( void )
{
	int16_t angle = 0;
	
	while (true)
	{
		led7((angle>=0)?angle:(-angle));
		sensor_cmp();
		handle(angle);
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) angle--;
		if (get_button(BTN2)) angle++;
	}
}

void servo_calibrate( void )
{
	int16_t angle = 0;
	while (true)
	{
		led7((angle>=0)?angle:(-angle));
		sensor_cmp();
		handle(angle);
		if (get_button(BTN0)) angle--;
		if (get_button(BTN1)) break;;
		if (get_button(BTN2)) angle++;
	}
	
	SERVO_CENTER = SERVO_CENTER + (angle * STEP);
}

/* -------------------- START -------------------- */
void sel_mode()
{
	handle(0);
	speed(0,0);
	set_encoder(-1);
	
	while(1)
	{
		ratio_base = ratio_default + (get_switch() / 20.0);
		ratio = ratio_base;
		led7(ratio_base * 100);
		delay = (1.4 - 1.125 * ratio);
		sensor_cmp();
		if(get_button(BTN0))		return;
		else if (get_button(BTN1))	test_hardware(); /*test_servo();*/
		else if (get_button(BTN2))	learn_color();
	}
}

#endif /* FUNCTION_H_ */