#pragma once
#ifndef _MICROSUTIL_H_
#define _MICROSUTIL_H_
#include <stdint.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/io.h>

ISR(TIMER1_COMPA_vect);
uint64_t _micros(void);
void initTimer(void);
#endif