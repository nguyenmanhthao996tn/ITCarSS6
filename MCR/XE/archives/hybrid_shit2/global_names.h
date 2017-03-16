#define mask3_0 0b11100000
#define mask0_3 0b00000111

#define K_P 5
#define K_I 1
#define K_D 1

pidData_t steer;
uint8_t off_lane = 0;
uint16_t timer = 0;

uint16_t EEMEM eeprom_adc_line[8];
uint16_t EEMEM eeprom_adc_no_line[8];

uint8_t	motor_speed = 30;
uint8_t m = 100;
#include "helper.h"

#include "functions.h"
#include "pid.h"



