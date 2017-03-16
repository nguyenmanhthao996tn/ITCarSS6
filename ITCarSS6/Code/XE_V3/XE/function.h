#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#ifndef sbi
#define sbi(port,bit) port|=(1 << bit)
#endif

#ifndef cbi
#define cbi(port,bit) port&=~(1 << bit)
#endif

#define LATCH	4
#define DATA	5
#define SCK		7
#define BTN0	0b11111101
#define BTN1	0b11111011
#define BTN2	0b11110111
#define DIR00	0
#define DIR01	1
#define DIR10	3
#define DIR11   6
#define SERVO_CENTER		3000 -(50)	//Sai số của cần sensor trên xe
#define STEP				7			//Bước quay của servo
#define vach_xam			19/20			//Bằng 1 nếu đường line không có vạch xám

//Variable ADC
uint16_t ADC_average[8];		//ADC trung bình
uint16_t linetrang[8];			//ADC line trắng
uint16_t lineden[8];			//ADC line đen

//Variable LED7
struct led7 {
	uint8_t i;
	uint8_t unit;
	uint8_t ten;
	uint8_t hundred;
	uint8_t thousand;
	uint8_t sensor_out;
} led7_data;

//Variable RATIO
#define ratio_default 0.1
uint16_t cnt_ratio, pulse_ratio;
uint16_t velocity;
float ratio;					//Tỉ số tốc độ
float ratio_base;				//Tỉ số tốc độ nền	

//===================BUTTON + SWITCH=====================
uint8_t get_button(uint8_t keyid)
{
	if ( (PINB & 0x0e) != 0x0e)
	{
		_delay_ms(80);
		if ((PINB|keyid) == keyid) return 1;
	}
	return 0;
}
uint8_t get_switch() // trả về từ 0->15
{
	uint8_t x=0;
	x = ~PINC;
	x = x & 0x0f;
	return x;
}
float get_switch_2() //trả về 0.1 -> 0.4
{
	float val=0;
	for(uint8_t i=0; i<4; i++)
	{
		if ( (((~PINC)>>i)&0x1) == 0x1 ) val+=0.1;
	}
	return val;
}

//================RATIO + SERVO + MOTOR ================
void set_encoder(uint8_t veloc)
{
	velocity=veloc;
}
void cal_ratio()
{
	/*cnt_ratio++;
	if (cnt_ratio == 20) //20ms
	{
		if      (pulse_ratio < velocity / 2)    ratio = ratio_base + 0.3;
		else if (pulse_ratio < velocity)        ratio = ratio_base + 0.1;
		else if (pulse_ratio > velocity)        ratio = ratio_base - 0.35;
		else if (pulse_ratio > velocity / 2)    ratio = ratio_base - 0.25;
		else ratio = ratio_base;
		pulse_ratio = 0;
		cnt_ratio = 0;
	}*/
	ratio = ratio_base;
}
void handle(int goc)
{
	if (goc>150) goc=150;
	else if(goc<-150) goc=-150;
	OCR1A=SERVO_CENTER+goc*STEP;
}
void speed(int left, int right)
{
	left  = left  *  ratio;
	right = right *  ratio;

	if(left>=0)
	{
		sbi(PORTD, DIR00);
		cbi(PORTD, DIR01);
		OCR1B=left*200;
	}
	else
	{
		cbi(PORTD, DIR00);
		sbi(PORTD, DIR01);
		OCR1B=-left*200;
	}
	
	if(right>=0)
	{
		sbi(PORTD, DIR10);
		cbi(PORTD, DIR11);
		OCR2=right*255/100;
	}
	else
	{
		cbi(PORTD, DIR10);
		sbi(PORTD, DIR11);
		OCR2=-right*255/100;
	}
}

inline void fast_brake_left()
{
	sbi(PORTD, DIR00);
	sbi(PORTD, DIR01);
	OCR1B=20000;
}

inline void fast_brake_right()
{
	sbi(PORTD, DIR10);
	sbi(PORTD, DIR11);
	OCR2=255;
}

void fast_brake()
{
	fast_brake_left();
	fast_brake_right();
}

//==========================LED7=========================
void SPI(uint8_t data)			//Truyền dữ  liệu sang led7, sử dụng SPI
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));	//Đợi phần cứng truyền xong data
}
void led7(unsigned int num)		// Tính toán dữ liệu cho 4 led 7 đoạn
{
	led7_data.unit		 = (1<<7) |(unsigned int)(num%10);
	led7_data.ten		 = (unsigned int)(num%100 / 10);
	led7_data.hundred	 = (unsigned int)(num%1000 / 100);
	led7_data.thousand	 = (unsigned int)(num/ 1000);
	led7_data.thousand	|= (led7_data.thousand!=0)? 1<<4 : 0;
	led7_data.hundred	|= (led7_data.thousand!=0 || led7_data.hundred!=0)? 1<<5 : 0;
	led7_data.ten		|= (led7_data.thousand!=0 || led7_data.hundred !=0 || led7_data.ten!=0)? 1<<6 : 0;
}
void print()					//Luôn thực thi mỗi vài ms để quét LED
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

//==========================ADC==========================
void read_adc_eeprom()
{
	for(uint8_t j=0; j<8; j++)
	{
		while(!eeprom_is_ready());
		linetrang[j] = eeprom_read_word((uint16_t*)(j*2));
		while(!eeprom_is_ready());
		lineden[j] = eeprom_read_word((uint16_t*)((j+8)*2));
	}
	for(uint8_t i=0; i<8; i++)
	{
		ADC_average[i]=(linetrang[i]+lineden[i])/2;
		ADC_average[i]=ADC_average[i]*vach_xam;
	}
}
void write_adc_eeprom()
{
	for(uint8_t j=0; j<8; j++)
	{
		while(!eeprom_is_ready());
		eeprom_write_word((uint16_t*)(j*2), (uint16_t)linetrang[j]);
		while(!eeprom_is_ready());
		eeprom_write_word((uint16_t*)((j+8)*2), (uint16_t)lineden[j]);
	}
}
uint16_t adc_read(uint8_t ch)
{
	ADMUX = (1<< REFS0)|ch;									// selecting channel
	ADCSRA|=(1<<ADSC);										// start conversion
	while(!(ADCSRA & (1<<ADIF)));							// waiting for ADIF, conversion complete
	return ADCW;											// Giá trị trả về từ [0 -> 1024] tương ứng [0V -> 5V]	
}
uint8_t sensor_cmp(uint8_t mask)							//Sensor compare: đọc về và so sánh với trung bình 
{															//Thêm tính năng che mặt nạ: mask mặc định là: 0xff (0b11111111)
	uint8_t ADC_value=0;												
	for(uint8_t i=0; i<8; i++)
	{
		if(adc_read(i)<ADC_average[i]) sbi(ADC_value,i);	//Nhỏ hơn trung bình -> gần về 0V -> led thu hồng ngoại dẫn -> có nhiều hồng ngoại -> vạch trắng
		//else    cbi(ADC_value,i);
	}
	led7_data.sensor_out=ADC_value;							//Cập nhật giá trị xuất ra 8 led đơn
	return (ADC_value & mask);
}
void learn_color()
{
	uint16_t ADC_temp=0;
	for (uint8_t i=0; i<8; i++)
	{
		linetrang[i]=1024;
		lineden[i]=0;
	}
	
	led7(2017);
	while(1)
	{
		if(get_button(BTN0)) return;
		else if(get_button(BTN2)) break;
		for (uint8_t i=0; i<8; i++)
		{
			ADC_temp=adc_read(i);
			if (ADC_temp < linetrang[i]) linetrang[i]=ADC_temp;
			if(ADC_temp>lineden[i]) lineden[i]=ADC_temp;
		}
	}
	for (uint8_t i=0; i<8; i++)
	{
		ADC_average[i]=(linetrang[i]+lineden[i])/2;
		
		ADC_average[i]=ADC_average[i] * vach_xam;
	}
	write_adc_eeprom();										//Ghi vào eeprom để cho các lần sau			
}

//=======================INITIAL=========================
void INIT()
{
	//ADC
	ADMUX=(1<<REFS0);										// 0b0100000000 Chọn điện áp tham chiếu từ chân AVCC, thêm tụ ở AREF
	ADCSRA=(1<<ADEN) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// 0b10000111 Enable ADC and set Prescaler = 128
	read_adc_eeprom();										// Tự động đọc Eeprom ra khi bật nguồn chip
	
	//PORT
	DDRB  = 0b11110001;
	PORTB = 0b11111111;
	DDRC  = 0b00000000;
	PORTC = 0b11111111;
	DDRD  = 0b11111011;
	PORTD = (1 << DIR00) | (1 << DIR10);										// DIR00 = 1, DIR01 = 0, DIR10 = 1, DIR11 = 0
	
	//SPI
	SPCR	= (1<<SPE)|(1<<MSTR);							//Enable spi, Master
	SPSR	= (1<<SPI2X);									//SCK Mode 2X: Fosc/2
	
	//TIMER
	TCCR0=(1<<WGM01) | (1<<CS02);							// Mode 2 CTC,  Prescaler = 256
	OCR0=62;												// 1ms
	TIMSK=(1<<OCIE0);
		
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);			// SET OCR1A & OCR1B at BOTTOM, CLEAR at Compare Match (Non-invert), Mode 14 Fast PWM
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);				// Prescaler = 8
	ICR1 = 20000;											// Time Period = 10ms
	
	TCCR2=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS22)|(1<<CS21)|(1<<CS20);  //SET OC2 at BOTTOM, CLEAR OC2 on compare match,(non-invert), Mode 3 Fast PWM,  Prescaler = 1024
	OCR2=0;
	sei();
	
	//ENCODER
	MCUCR |= (1<<ISC11)|(1<<ISC01);
	GICR |= (1<<INT0);
	set_encoder(19);
}

void test_hardware()
{
	uint8_t _index=0;
	while(1)
	{
		if(get_button(BTN0))		{ speed(100,100); handle(-100);}
		else if (get_button(BTN1))	{ if(++_index == 8) _index=0;}
		else if (get_button(BTN2))	{ speed(-100,-100); handle(100); }
		else						{ speed(0,0);  handle(0);    }
		
		led7(adc_read(_index));
		led7_data.sensor_out = 0 | (1<<_index);
	}
}
//========================START==========================
void sel_mode()
{
	handle(0);
	speed(0,0);
	while(1)
	{
		ratio = ratio_base = ratio_default + get_switch_2();
		led7(ratio_base*100);
		sensor_cmp(0xff);
		if(get_button(BTN0))		return;
		else if (get_button(BTN1))	test_hardware();
		else if (get_button(BTN2))	learn_color();
	}
}