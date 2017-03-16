#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define cbi(port, bit) (port) &= ~(1 << (bit))
#define sbi(port, bit) (port) |=  (1 << (bit))

#define LATCH 4
#define DATA 5
#define SCK 7
#define BTN0 0b00000010
#define BTN1 0b00000100
#define BTN2 0b00001000
#define DIR0 3
#define DIR1 6
#define SERVO_ERROR 250
#define SERVO_CENTER 3000 + SERVO_ERROR
#define SERVO_PWM_PERIOD 10*2000UL
#define SERVO_STEP 6
#define LINE_DEFAULT 300 //lol lazy coding !!!
#define motor_ratio 0.5

uint16_t LINE = LINE_DEFAULT;
uint16_t adc_average[8];
uint16_t adc_line[8];
uint16_t adc_no_line[8];

uint16_t EEMEM eeprom_adc_line[8];
uint16_t EEMEM eeprom_adc_no_line[8];

uint8_t EEMEM eeprom_m;
uint8_t EEMEM eeprom_motor_speed;
uint16_t EEMEM eeprom_KP;
uint16_t EEMEM eeprom_KI;
uint16_t EEMEM eeprom_KD;

uint16_t K_P;
uint16_t K_I;
uint16_t K_D;

uint8_t m;
uint8_t	motor_speed;

//led 7 data
struct ld {
	uint8_t i, p_1, p_10, p_100, p_1000, sensor_debug_output;
} led_data;

int16_t velocity = 10;
int16_t feedback_velocity;

volatile uint32_t timer0_mil = 0;

uint32_t milis() {
	return timer0_mil;
}

uint8_t get_button(uint8_t keyid) {
	if ( (PINB & keyid) == 0) {
		while ( (PINB & keyid) == 0);
		return 1;
	}
	return 0;
}

uint8_t get_switch() {
	return ~PINC & 0xf;
}

void servo(int delta) {
	if (delta > 150) delta = 150;
	else if(delta < -150) delta = -150;
	OCR1A = SERVO_CENTER + delta*SERVO_STEP;
}

void fwd(uint16_t left, uint16_t right) {
	left  = left  *  motor_ratio;
	right = right *  motor_ratio;
	if (left >= 0 ) {
		cbi(PORTD,DIR0);
		OCR1B=left*200;
	}
	else {
		sbi(PORTD,DIR0);
		OCR1B=(100+left)*200;
	}
	
	if(right >= 0) {
		cbi(PORTD,DIR1);
		OCR2=right*255/100;
	}
	else {
		sbi(PORTD,DIR1);
		OCR2=(100+right)*255/100;
	}
}

void SPI(uint8_t data) { //transfer data using SPI
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));	//wait for it to complete
}

void set_led_data(uint32_t num) {
	led_data.p_1 = (8<<4) | (num % 10);
	led_data.p_10 = num % 100 / 10;
	led_data.p_100 = num % 1000 / 100;
	led_data.p_1000 = num / 1000;
	led_data.p_1000 |= (led_data.p_1000 != 0) ? 1<<4 : 0;
	led_data.p_100 |= (led_data.p_1000 != 0 || led_data.p_100 != 0) ? 2<<4 : 0;
	led_data.p_10 |= (led_data.p_1000 != 0 || led_data.p_100 != 0 || led_data.p_10 != 0) ? 4<<4 : 0;
}

void print() {
	uint8_t value = 0;

	if(led_data.i++ == 4) led_data.i=0;
	switch(led_data.i) {
		case 0: value = led_data.p_1000; break;
		case 1: value = led_data.p_100;	break;
		case 2: value = led_data.p_10; break;
		case 3: value = led_data.p_1; break;
		default: break;
	}

	SPI(~led_data.sensor_debug_output);
	SPI(value);
	sbi(PORTB,LATCH);
	cbi(PORTB,LATCH);
}


void write_eeprom(void* src, void* pointer_eeprom, size_t n) {
	while (!eeprom_is_ready());
	eeprom_write_block(src, pointer_eeprom, n);
}

void read_eeprom(void* dst, void* pointer_eeprom, size_t n) {
	while (!eeprom_is_ready());
	eeprom_read_block(dst, pointer_eeprom, n);
}
uint16_t read_adc(uint8_t channel) {
	ADMUX= (1<<REFS0) | channel; //selecting channel
	ADCSRA |= (1<<ADSC); //start conversion
	loop_until_bit_is_set(ADCSRA, ADIF);
	return ADCW;
}

uint8_t read_sensor() {
	uint8_t adc_value = 0;
	uint16_t t = 0;
	
	for(uint8_t i=0; i<8; i++) {
		t = (read_adc(i) + read_adc(i)) / 2;
		if(t < LINE) sbi(adc_value, i);
		else cbi(adc_value, i);
	}
	led_data.sensor_debug_output = adc_value;
	return (adc_value);
}

void init() {
	//ADC
	ADMUX = (1<<REFS0);	//reference voltage form avcc
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //enable ADC and set prescaler = 8

	//PORT
	DDRB  = 0b11110001;
	PORTB = 0b11111111;
	DDRC  = 0b00000000;
	PORTC = 0b11111111;
	DDRD  = 0b11111011;
	PORTD = 0b00000000;

	//SPI
	SPCR	= (1<<SPE) | (1<<MSTR); //enable SPI, master
	SPSR	= (1<<SPI2X); //SCK mode 2X: Fosc/2

	//timer
	TCCR0 = (1<<WGM01) | (1<<CS02); //mode 2 CTC,  prescaler = 256
	OCR0 = 62; //1ms
	TIMSK = (1<<OCIE0);
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11); // set OCR1A & OCR1B at bottom, clear at compare match (non-invert), mode 14 fast pwm
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11); //prescaler = 8
	ICR1 = SERVO_PWM_PERIOD; //time period = 10ms
	TCCR2 = (1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS22)|(1<<CS21)|(1<<CS20); //sest OC2 at bottom, clear OC2 on compare match, (non-invert), mode 3 fast pwm, prescaler = 1024
	OCR2 = 0;
	//enable interrupts
	sei();

	//encoder
	MCUCR |= (1<<ISC11)|(1<<ISC01);
	GICR |= (1<<INT0);
}

void test_mode() {
	servo(0);
	uint8_t idx = 0;
	while (1) {
		if (get_button(BTN0)) break;
		if (get_button(BTN1)) idx--;
		if (get_button(BTN2)) idx++;
		if (idx > 7 || idx < 0) idx = 0;
		set_led_data(read_adc(idx));
		led_data.sensor_debug_output = 1 << idx;
	}
	
	motor_speed = eeprom_read_byte(&eeprom_motor_speed);
	if (motor_speed == 255) {
		motor_speed = 80;
	}
	while (1) {
		if (get_button(BTN0)) {
			eeprom_write_byte(&eeprom_motor_speed, motor_speed);
			break;
		}
		if (get_button(BTN1)) motor_speed -= 10;
		if (get_button(BTN2)) motor_speed += 10;
		set_led_data(motor_speed);
	}
	
	m = eeprom_read_byte(&eeprom_m);
	if (m == 255) {
		m = 50;
	}
	while (1) {
		if (get_button(BTN0)) {
			eeprom_write_byte(&eeprom_m, m);
			break;
		}
		if (get_button(BTN1)) m -= 1;
		if (get_button(BTN2)) m += 1;
		set_led_data(m);
	}
	
	K_P = eeprom_read_word(&eeprom_KP);
	if (K_P == 65535UL) {
		K_P = 95;
	}
	while (1) {
		servo(0);
		if (get_button(BTN0)) {
			eeprom_write_word(&eeprom_KP, K_P);
			break;
		}
		if (get_button(BTN1)) K_P -= 10;
		if (get_button(BTN2)) K_P += 10;
		set_led_data(K_P);
	}
	
	K_I = eeprom_read_word(&eeprom_KI);
	if (K_I == 65535UL) {
		K_I = 2;
	}
	while (1) {
		if (get_button(BTN0)) {
			eeprom_write_word(&eeprom_KI, K_I);
			break;
		}
		if (get_button(BTN1)) K_I -= 1;
		if (get_button(BTN2)) K_I += 1;
		set_led_data(K_I);
	}
	
	K_D = eeprom_read_word(&eeprom_KD);
	if (K_D == 65535UL) {
		K_D = 1;
	}
	while (1) {
		if (get_button(BTN0)) {
			eeprom_write_word(&eeprom_KD, K_D);
			break;
		}
		if (get_button(BTN1)) K_D -= 10;
		if (get_button(BTN2)) K_D += 10;
		set_led_data(K_D);
	}
}