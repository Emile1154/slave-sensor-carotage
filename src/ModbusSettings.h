#pragma once
#ifndef _MODBUSSETTINGS_H_
#define _MODBUSSETTINGS_H_

#include <stdint.h>
//<---------commands modbus rtu------------>
//<------------write commands--------------->
//one register have size 16bit (65535)
//      NAME_COMMAND         KEY    ADDRESS   VALUE
#define SET_ID                0   // 0003     1 - 32    
#define SET_SPEED             1   // 7502     1 - 5 (1200,2400,4800,9600,19200)  29954
#define SET_EVEN              2   // 7503     1, 3, 5 (NONE, EVEN, ODD)           29955
#define SET_INTERVAL          3   // 7508    (1 - 16) x2 = value ms             29960
#define SET_COUNT_STOP_BIT    4   // 750A     0 - 1  (1bit, 2bit)               29962

#define SET_ZERO_TENSION      5   // 0005     1
#define CALIB_TENSION         6   // 0006     0 - 65535

#define SET_ENCODER_WIRE      7   // 0007     1 - 4 (I2C, SPI, ST, ABZ)
#define SET_INVERT_ENCODER    8   // 0008

#define CALIB_HALL            9   // 0009     0 - 255 

//<----------read commands------------------->
#define GET_TENSION           10  // 000A     0 - 65535
#define GET_ENCODER_HI        11  // 000B   | 0 - 65535    // set too
#define GET_ENCODER_LO        12  // 000C   | 0 - 65535    // set too
#define GET_VELOCITY          13  // 000D
#define GET_HALL              14  // 000E     0 - 1 

#endif
//EXAMPLE FOR SINGLE WRITE REGISTER
/* 8
 * id
 * function               0x06
 * ADDRESS                HIGH
 * ADDRESS                LOW
 * VALUE                  HIGH
 * VALUE                  LOW
 * CRC                    HIGH
 * CRC                    LOW
*/

//EXAMPLE FOR GROUP WRITE REGISTER
/* 13
 * id
 * function               0x10
 * START_REGISTER         HIGH
 * START_REGISTER         LOW
 * COUNT_REGISTER         HIGH
 * COUNT_REGISTER         LOW
 * COUNT DATA BYTES       EQUALS 2*COUNT_REGISTER
 * FIRST_GROUP_DATA       HIGH
 * FIRST_GROUP_DATA       LOW
 * SECOND_GROUP_DATA      HIGH
 * SECOND_GROUP_DATA      LOW
 * CRC                    HIGH
 * CRC                    LOW
*/