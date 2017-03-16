uint16_t cnt1 = 0, pulse_v = 0, pulse_pid = 0;

void old_school_main() {
	uint8_t pattern;
	off_lane = 0;
	my_timer = 0;
	while(1) {
		if (get_button(BTN1)) break;
		read_sensor();
	}
	
	while(1) {
		pattern = 1;
		while(1) {
			
			switch(pattern) {
				case 1:
					if(check_crossline()) { //90 degree turn
						pattern = 21;
					}
					break;
				
					if(check_rightline()) { //right lane
						pattern = 51;
					}
					break;
					
					if(check_leftline()) { //left lane
						pattern = 61;	
					}
					break;
					set_led_data(10);
					#ifdef DEBUG
					push_stack(10); 
					#endif
					//set_encoder(19);
					switch(read_sensor() & 0b01111110) {
						case 0b01111110:
							fwd(0,0);
							servo(0);
						break;
					
						case 0b00011000: //forward
							servo(0);
							fwd(100, 100);
						break;
					
						case 0b00011100:
						case 0b00001000:
							fwd(90,80);
							servo(8);
						break;
					
						case 0b00001100:
							fwd(90,70);
							servo(10);
						break;
					
						case 0b00001110:
						case 0b00000100:
							fwd(90,60);
							servo(12);
						break;
					
						case 0b00000110:
							fwd(90,50);
							servo(15);
						break;
					
						case 0b00000010:
							fwd(90,30);
							servo(20);
							pattern = 11;
						break;
					
						case 0b00111000:
						case 0b00010000:
							fwd(80,90);
							servo(-8);
						break;
					
						case 0b00110000:
							fwd(70,90);
							servo(-10);
						break;
					
						case 0b01110000:
						case 0b00100000:
							fwd(60,90);
							servo(-12);
						break;
					
						case 0b01100000:
							fwd(50,90);
							servo(-15);
						break;
					
						case 0b01000000:
							fwd(30,90);
							servo(-20);
							pattern = 12;
						break;
					
						default:
						break;
					}
				break;
				
				case 11:
					set_led_data(11);
					#ifdef DEBUG
					push_stack(11);
					#endif
					//set_encoder(23);
					switch(read_sensor() & 0b11001100) {
						case 0b11000000:
							fwd(80, -10);
							servo(70);
						break;
					
						case 0b10000000:
							fwd(80, 5);
							servo(47);
						break;
					
						case 0b00000000:
							fwd(80, 10);
							servo(45);
						break;
					
						case 0b00000100:
							fwd(80, 15);
							servo(34);
						break;
					
						case 0b00001100:
							fwd(80,20);
							servo(26);
							pattern = 1;
						break;
					
						default:
							pattern = 11;
						break;
					}
				break;
				
				case 12:
					set_led_data(12);
					//set_encoder(23);
					#ifdef DEBUG
					push_stack(12);
					#endif
					switch(read_sensor() & 0b00110011) {
						case 0b00000011:
							fwd(-10, 80);
							servo(-100);
						break;
					
						case 0b00000001:
							fwd(5, 80);
							servo(-47);
						break;
					
						case 0b00000000:
							fwd(10, 80);
							servo(-45);
						break;
					
						case 0b00100000:
							fwd(15, 80);
							servo(-34);
						break;
					
						case 0b00110000:
							fwd(20, 80);
							servo(-26);
							pattern = 1;
						break;
					
						default:
							pattern=12;
						break;
					}
				break;
				
				case 21:
					set_led_data(21);
					//set_encoder(20);
					#ifdef DEBUG
					push_stack(21);
					#endif
					servo(0);
					fwd(60 ,60);
					pattern = 22;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 22:
					set_led_data(22);
					//set_encoder(20);
					#ifdef DEBUG
					push_stack(2);
					#endif
					servo(0);
					fwd(60 ,60);
					if( cnt1 > 70 || pulse_v > 10 ) {
						pattern = 23;
						cnt1 = 0;
						pulse_v = 0;
					}
				break;
				
				case 23:
					set_led_data(23);
					#ifdef DEBUG
					push_stack(23);
					#endif
					if( (pulse_v > 100) && ((read_sensor() == 0b11111000)  || (read_sensor()==0b11110000) || (read_sensor()==0b11100000) || (read_sensor() == 0b11111100))) {
						pattern = 24;
						cnt1 = 0;
						break;
					}
				
					if(  (pulse_v > 100) &&   ((read_sensor() == 0b00011111 ) || (read_sensor() == 0b00000111) || (read_sensor() == 0b00001111) || (read_sensor() == 0b00111111))) {
						pattern = 25;
						cnt1 = 0;
						break;
					}
				
					switch(read_sensor() & 0b01111110) {
						case 0b00011000:
							servo(0);
							fwd(20, 20);
						break;
					
						case 0b00001000:
							fwd(23, 3);
							servo(8);
						break;
					
						case 0b00001100:
							fwd(23,-3);
							servo(10);
						break;
					
						case 0b00000100:
							fwd(23,-8);
							servo(13);
						break;
					
						case 0b00000110:
						case 0b00000010:
							fwd(23,-13);
							servo(18);
						break;
					
						case 0b00010000:
							fwd(5,20);
							servo(-8);
						break;
					
						case 0b00110000:
							fwd(0, 20);
							servo(-10);
						break;
					
						case 0b00100000:
							fwd(-5,20);
							servo(-13);
						break;
					
						case 0b01100000:
						case 0b01000000:
							fwd(10,20);
							servo(-18);
						break;
					
						default:
						break;
					}
				break;
				
				case 24:
					//set_encoder(18);
					fwd(50, 50);
					if ( (pulse_v > 100) && (((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111))) {
						pattern = 23 ;
						break;
					}
					if (cnt1 > 50 || pulse_v > 10) {
						pattern = 26;
						break;
					}
					break;
					
					case 25:
					//set_encoder(18);
					fwd(50,50);
						if ( (pulse_v > 100 ) && (((read_sensor() & 0b11100000) == 0b11100000) || ((read_sensor() & 0b11110000) == 0b11110000))) {
							pattern = 23 ;
							break;
						}
						if (cnt1 > 50 || pulse_v > 10) {
							pattern = 27;
							break;
						}
					break;
				
				case 26:
					set_led_data(24);
					servo(-150);
					//set_encoder(8);
					#ifdef DEBUG
					push_stack(24);
					#endif
					fwd(-30, 90);
					pattern = 31;
					cnt1 = 0;
				break;
				
				case 27:
					set_led_data(25);
					#ifdef DEBUG
					push_stack(25);
					#endif
					servo(150);
					//set_encoder(8);
					fwd(90, -30);
					pattern = 41;
					cnt1 = 0;
				break;
				
				case 31:
					set_led_data(31);
					#ifdef DEBUG
					push_stack(31);
					#endif
					if(cnt1 > 250) {
						pattern = 32;
						cnt1 = 0;
					}
				break;
				
				case 32:
					set_led_data(32);
					#ifdef DEBUG
					push_stack(32);
					#endif
					if( (read_sensor() & 0b11100111) == 0b00100000 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 41:
					set_led_data(41);
					#ifdef DEBUG
					push_stack(41);
					#endif
					if( cnt1 > 200 ) {
						pattern = 42;
						cnt1 = 0;
					}
				break;
				
				case 42:
					set_led_data(42);
					#ifdef DEBUG
					push_stack(42);
					#endif
					if( (read_sensor() & 0b11100111) == 0b00000100 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 51:
					//set_encoder(20);
					if ((read_sensor(0b11100000) == 0b11100000) || (read_sensor(0b11110000) == 0b11110000)) {
						pattern = 21 ;
						break;
					}
					set_led_data(51);
					#ifdef DEBUG
					push_stack(51);
					#endif
					servo(0);
					fwd(70 , 70);
					pattern = 52;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 52:
					if( cnt1 > 100 || pulse_v>10) {
						pattern = 53;
						cnt1 = 0;
					}
					if (((read_sensor() & 0b11100000) == 0b11100000) || ((read_sensor() & 0b11110000) == 0b11110000)) {
						pattern = 21 ;
						break;
					}
				break;

				case 53:
				if( read_sensor() == 0b00000000 ) {
					servo(70);
					fwd(50, 60);
					pattern = 54;
					cnt1 = 0;
					break;
				}
				switch( read_sensor() & 0b11100111 ) {
					case 0:
						servo( 0 );
						fwd( 30 ,30 );
						break;
						case 4:
						case 6:
						case 7:
						case 3:
						servo(15);
						fwd(30, 20);
					break;
					case 0x20:
					case 0x60:
					case 0xe0:
					case 0xc0:
						servo(-15);
						fwd(20, 30);
					break;
					
					default:
					break;
				}
				break;

				case 54:
					if( (read_sensor() & 0b00001100) == 0b00001100 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;

				case 61:
					//set_encoder(20);
					if ( ((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111) ) {
						pattern = 21 ;
						break;
					}
					set_led_data(61);
					#ifdef DEBUG
					push_stack(61);
					#endif
					servo(0);
					fwd(90, 90);
					pattern = 62;
					cnt1 = 0;
					pulse_v = 0;
				break;

				case 62:
					if( cnt1 > 100 || pulse_v > 10) {
						pattern = 63;
						cnt1 = 0;
					}
					if (((read_sensor() & 0b00000111) == 0b00000111) || ((read_sensor() & 0b00001111) == 0b00001111)) {
						pattern = 21 ;
						break;
					}
				break;

				case 63:
					if( read_sensor() == 0 ) {
						servo(-70);
						fwd(60, 40);
						pattern = 64;
						cnt1 = 0;
					}
				break;
				
				switch( read_sensor() & 0b11100111 ) {
					case 0:
						servo(0);
						fwd(30, 30);
					break;
					case 4:
					case 6:
					case 7:
					case 3:
						servo( 15 );
						fwd( 30 ,20 );
					break;
					case 0x20:
					case 0x60:
					case 0xe0:
					case 0xc0:
						servo(-15);
						fwd(20, 30);
					break;
					
					default:
					break;
				}
				break;

				case 64:
					if( (read_sensor() & 0b00110000) == 0b00110000 ) {
						pattern = 1;
						cnt1 = 0;
					}
				break;
				
				case 71:
					//set_encoder(30);
					set_led_data(71);
					#ifdef DEBUG
					push_stack(71);
					#endif
					fwd(100, 100);
					if( cnt1 > 100 || pulse_v > 10) {
						pattern = 72;
						cnt1 = 0;
					}
				break;
				
				case 72:
					if (read_sensor() == 0b00000000) {
						servo(0);
						fwd(100,100);
						pattern=73;
						break;
					}
				
					switch(read_sensor() & 0b01111110) {
						case 0b00011000:
							servo(0);
							fwd(100,100 );
						break;
					
						case 0b00001000:
							fwd(90,17);
						servo(8);
						break;
					
						case 0b00001100:
							fwd(90,70);
							servo(13);
						break;
					
						case 0b00000100:
							fwd(90,65);
							servo(18);
						break;
					
						case 0b00000110:
						case 0b00000010:
							fwd(90,60);
							servo(26);
						break;
					
						case 0b00010000:
							fwd(75,90);
							servo(-8);
						break;
						case 0b00110000:
							fwd(70,90);
							servo(-13);
						break;
					
						case 0b00100000:
							fwd(65,90);
							servo(-18);
						break;
					
						case 0b01100000:
						case 0b01000000:
							fwd(60,90);
							servo(-26);
						break;
					
						default:
						break;
				}
				break;
				
				case 73:
					servo(0);
					fwd(100, 100);
					if (read_sensor() > 0) {
						pattern = 1;
					}
				break;

				default:
					pattern = 1;
				break;
			}
		}
	}
}