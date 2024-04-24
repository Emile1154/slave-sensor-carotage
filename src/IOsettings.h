#pragma once
#ifndef _IOSETTINGS_H_
#define _IOSETTINGS_H_

//#define F_CPU 16000000          //cpu frequency

//pin address setup
#define EEPROM_SIGNAL_PIN  PC0
#define HOLLA_SENSOR_PIN   PD4  //INT1
#define TENSION_PWM_OUTPUT PD6  //TIM0A
#define HOLLA_PWM_OUTPUT   PD5  //TIM0B
#define START_PWM_NO_OFFSET 39 
#endif