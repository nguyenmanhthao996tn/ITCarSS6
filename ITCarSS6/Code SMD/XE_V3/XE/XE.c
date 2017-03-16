#include "function.h"

int check_crossline( void );
int check_rightline( void );
int check_leftline( void );

uint16_t cnt2 = 0;

uint8_t  pattern;
uint16_t cnt1, pulse_v;
extern uint16_t pulse_ratio;

int main(void)
{
	
	float val = 0.1;
	
    INIT();
    sel_mode();
    ///////////////////////////////////////////////////////////////////////
    while(1)
    {	
        if (get_button(BTN1))
		{
			ratio = ratio_base = val;
			break;
		}
		if (get_button(BTN0))
		{
			val += 0.05;
			if (val >= 1.0)	 val = 0.1;
		} 
		led7((unsigned int)(val * 100));
        sensor_cmp(0xff);
    }
    //////////////////////////////////////////////////////////////////////////
    
    while(1)
    {
        pattern = 1;
        
        while(1)
        {
            switch(pattern)
                {
                    case 1:
                    if( check_crossline() ) //Be 90
                    {
                        pattern = 21;
                        break;
                    }
                    if( check_rightline() ) // Chuyen lan phai
                    {
                        pulse_v = 0;
                        cnt1 = 0;
                        pattern = 51;
                        break;
                    }
                    if( check_leftline() )  // Chuyen lan trai
                    {
                        pulse_v = 0;
                        cnt1 = 0;
                        pattern = 61;
                        break;
                    }
                    led7(10);
                    switch(sensor_cmp(0b01111110))
                    {
                        case 0b00011000:	// Chay thang
                        handle( 0 );
                        speed( 100 ,100 );
                        break;
                        
                        //lech phai
                        case 0b00011100:
                        case 0b00001000:
                        speed(100,90);
                        handle(14);
                        break;
                        
                        case 0b00001100:
                        speed(100,80);
                        handle(22);
                        break;
                        
                        case 0b00001110:
                        case 0b00000100:
                        speed(100,70);
                        handle(35);
                        break;
                        
                        case 0b00000110:
                        speed(100,60);
                        handle(55);
                        break;
                        
                        case 0b00000010:
                        speed(100,40);
                        handle(80);
                        pattern=11;	//lech phai goc lon
                        break;
                        //////////////////////////////////////////////////////////////////////////
                        
                        //lech trai
                        case 0b00111000:
                        case 0b00010000:
                        speed(90,100);
                        handle(-14);
                        break;
                        
                        case 0b00110000:
                        speed(80,100);
                        handle(-22);
                        break;
                        
                        case 0b01110000:
                        case 0b00100000:
                        speed(70,100);
                        handle(-36);
                        break;
                        
                        case 0b01100000:
                        speed(60,100);
                        handle(-55);
                        break;
                        
                        case 0b01000000:
                        speed(40,100);
                        handle(-80);
                        pattern=12; //lech trai goc lon
                        cnt1 = 0;
                        pulse_v = 0;
                        break;
                        
                        default:
                        break;
                    }
                    break;
                    
                    case 11:
                    led7(11);
                    switch (sensor_cmp(0b11111111))
                    {
                        case 0b00000011:
                        handle(60);
                        speed(90,50);
                        break;
                        
                        case 0b00000110:
                        handle(55);
                        speed(90,50);
                        break;
                        
                        case 0b00001100:
                        handle(40);
                        speed(90,60);
                        break;
                        
                        case 0b00000001:
                        handle(90);
                        speed(90, 30);
                        break;
                        
                        case 0b10000001:
                        handle(90);
                        speed(90, 20);
                        break;
                        
                        case 0b10000000:
                        handle(100);
                        speed(90, 10);
                        break;
                        
                        case 0b11000000:
                        handle(105);
                        speed(90, 0);
                        break;
                        
                        case 0b01100000:
                        case 0b11100000:
                        handle(120);
                        speed(90, 0);
                        break;
                        
                        case 0b00110000:
                        case 0b11110000:
                        handle(130);
                        speed(90, -10);
                        break;
                        
                        case 0b00010000:
                        case 0b00001000:
                        case 0b00000100:
                        case 0b00011000:
                        pattern = 1;
                        pulse_v = 0;
                        cnt1 = 0;
                        led7(10);
                        break;
                        
                        default:
                        break;
                    }
                    break;
                    
                    case 12:
                    led7(12);
                    switch (sensor_cmp(0b11111111))
                    {
                        case 0b11000000:
                        handle(-60);
                        speed(50,90);
                        break;
                        
                        case 0b01100000:
                        handle(-55);
                        speed(50,90);
                        break;
                        
                        case 0b00110000:
                        handle(-40);
                        speed(60,90);
                        break;
                        
                        case 0b10000000:
                        handle(-90);
                        speed(30, 90);
                        break;
                        
                        case 0b10000001:
                        handle(-90);
                        speed(20, 90);
                        break;
                        
                        case 0b00000001:
                        handle(-85);
                        speed(10, 90);
                        break;
                        
                        case 0b00000011:
                        handle(-105);
                        speed(0, 90);
                        break;
                        
                        case 0b00000110:
                        case 0b00000111:
                        handle(-120);
                        speed(0, 90);
                        break;
                        
                        case 0b00001100:
                        case 0b00001111:
                        handle(-130);
                        speed(-10, 90);
                        break;
                        
                        case 0b00001000:
                        case 0b00010000:
                        case 0b00100000:
                        case 0b00011000:
                        pattern = 1;
                        pulse_v = 0;
                        cnt1 = 0;
                        led7(10);
                        break;
                        
                        default:
                        break;
                    }
                    break;

                    case 21:
					led7(21);
					handle( 0 );
					if ((cnt2 % 5) == 0)
					{
						speed(-7, -7);
						_delay_ms(2);
					}
					else
					{
						fast_brake();
					}
					if (cnt1 >= 100)
					{
						if (pulse_v <= 20)
						{
							pattern = 23;
							cnt1 = 0;
							pulse_v=0;
						}
						cnt1 = 0;
						pulse_v = 0;
					}
					break;
					
					case 23:
					led7(23);
					//cua trai
					if( ((pulse_v>50) || (cnt1 > 80)) && ((sensor_cmp(0b11111111)==0b11111000)  || (sensor_cmp(0b11111111)==0b11110000) || (sensor_cmp(0b11111111)==0b11100000) || (sensor_cmp(0b11111111)==0b11111100)))	// Neu gap tin hieu nay la goc cua 90 trai thi be
					{
						pattern = 26;
						cnt1=0;
						break;
					}
					//cua phai
					if(  ((pulse_v>50) || (cnt1 > 80)) &&   ((sensor_cmp(0b11111111)==0b00011111 ) ||(sensor_cmp(0b11111111)==0b00000111) || (sensor_cmp(0b11111111)==0b00001111) || (sensor_cmp(0b11111111)==0b00111111))) // Neu gap tin hieu nay la goc cua 90 phai thi be
					{
						pattern = 27;
						cnt1=0;
						break;
					}
					speed(70, 70);
					// Nguoc lai thi chinh thang cho xe
					switch(sensor_cmp(0b01111110))
					{
						case 0b01111110:
						handle(0);
						break;
						
						case 0b00011000:	// Chay thang
						handle( 0 );
						break;
						
						//lech phai
						case 0b00011100:
						case 0b00001000:
						handle(14);
						break;
						
						case 0b00001100:
						handle(22);
						break;
						
						case 0b00001110:
						case 0b00000100:
						handle(35);
						break;
						
						case 0b00000110:
						handle(55);
						break;
						
						case 0b00000010:
						handle(80);
						break;
						//////////////////////////////////////////////////////////////////////////
						//lech trai
						
						case 0b00111000:
						case 0b00010000:
						handle(-14);
						break;
						
						case 0b00110000:
						handle(-22);
						break;
						
						case 0b01110000:
						case 0b00100000:
						handle(-36);
						break;
						
						case 0b01100000:
						handle(-55);
						break;
						
						case 0b01000000:
						handle(-80);
						break;
						
						default:
						break;
					}
					if (sensor_cmp(0xff) == 0x00)
					{
						pattern = 73;
						handle(0);
						speed(100, 100);
						cnt1 = 0;
						pulse_v = 0;
					}
					break;
					
					case 26: //trai
					led7(26);
					handle( -130 );
					speed( -60 , 80 );
					pattern = 31;
					cnt1 = 0;
					break;
					
					case 27://phai
					led7(27);
					handle( 130);
					speed( 80 , -60 );
					pattern = 41;
					cnt1 = 0;
					break;
					
					
					case 31:	// �Cho 250ms de xe kip be cua 90
					led7(31);
					if( cnt1 > 200 )
					{
						pattern = 32;
						cnt1 = 0;
					}
					break;
					
					case 32:	// Cho tin hieu de ve truong hop chay thang va be cong
					led7(32);
					if( sensor_cmp(0b11100111) == 0b00100000 )
					{
						pattern = 1;
						pulse_v = 0;
						cnt1 = 0;
					}
					break;

					case 41:
					led7(41);
					if( cnt1 > 200 ) {
						pattern = 42;
						cnt1 = 0;
					}
					break;
					
					case 42:
					led7(42);
					if( sensor_cmp(0b11100111) == 0b00000100 ) {
						pattern = 1;
						pulse_v = 0;
						cnt1 = 0;
					}
					break;

					case 51: //PHAI
					if ((sensor_cmp(0b11100000) == 0b11100000) || (sensor_cmp(0b11110000) == 0b11110000) || (sensor_cmp(0b11111000) == 0b11111000))
					{
						pattern = 21 ;
						break;
					}
					led7(51);
					speed(80, 80);
					if (pulse_v >= 25 || cnt1 >= 175)
					{
						pattern = 53;
						cnt1 = 0;
						pulse_v=0;
					}
					break;
					
					case 53:
					led7(53);
					handle(55);
					speed( 85 ,80 );
					pattern = 54;
					cnt1 = 0;
					break;
					break;

					case 54:
					led7(54);
					led7(54);
					if(((pulse_v > 100) || (cnt1 > 200)) && (sensor_cmp( 0b00110000 ) == 0b00110000))
					{
						speed(80, 85);
						handle(-20);
						pattern = 1;
						cnt1 = 0;
						led7(10);
					}
					break;

					case 61:	// Xu ly khi gap vach tin hieu chuyen lan trai dau tien
					if ( (sensor_cmp(0b00000111) == 0b00000111) || (sensor_cmp(0b00001111) == 0b00001111) || (sensor_cmp(0b00011111) == 0b00011111))
					{
						pattern = 21 ;
						break;
					}
					led7(61);
					speed(80, 80);
					if (pulse_v >= 25 || cnt1 >= 175)
					{
						pattern = 63;
						cnt1 = 0;
						pulse_v=0;
					}
					break;

					case 63:
					led7(63);
					handle(-55);
					speed( 80 ,85 );
					pattern = 64;
					cnt1 = 0;
					break;

					case 64:
					led7(64);
					if(((pulse_v > 100) || (cnt1 > 200)) && (sensor_cmp( 0b00110000 ) == 0b00110000))
					{
						speed(85, 80);
						handle(20);
						pattern = 1;
						cnt1 = 0;
						led7(10);
					}
					break;

					case 73:
					led7(73);
					speed(100,100);
					if (sensor_cmp(0b10000000)) handle(30);
					if (sensor_cmp(0b00000001)) handle(-30);
					if (sensor_cmp(0b01111110) > 0)
					{
						pattern=1;
					}
					break;

					default:
					pattern = 1;
					break;
                }
        }
        
    }
}

ISR(TIMER0_COMP_vect)
{
    cnt1++;
	cnt2++;
    cal_ratio();
    print();			//Quét LED7 đoạn
}

int check_crossline( void )
{
    int ret=0;
    if(( sensor_cmp(0b11111111)==0b11111111) || (sensor_cmp(0b01111110)==0b01111110))ret = 1;
    return ret;
}
int check_rightline( void )
{
    int ret=0;
    if( (sensor_cmp(0b00001111) == 0b00001111) || (sensor_cmp(0b00011111)==0b00011111))  ret = 1;
    return ret;
}
int check_leftline( void )
{
    int ret=0;
    if( (sensor_cmp(0b11110000) == 0b11110000) || (sensor_cmp(0b11111000)== 0b11111000)) ret = 1;
    return ret;
}