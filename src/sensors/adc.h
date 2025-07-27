#pragma once
#ifndef _ADC_H_
#define _ADC_H_
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
ISR(ADC_vect);
void adc_init(void);

void adc_start(void);
uint8_t adc_end(void);
uint16_t get_result(void);
uint16_t readADC(void);
#endif
