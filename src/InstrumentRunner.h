#pragma once
#ifndef _INSTRUMENTRUNNER_H_
#define _INSTRUMENTRUNNER_H_
#include <MicrosUtil.h>
#include <stdint.h>
#include <ModbusSettings.h>
#include <modbus/Modbus.h>
#include <sensors/Sensors.h> 
#include <EEPROMex.h>
#include <math.h>
#include <SoftwareSerial.h>
class InstrumentRunner
{
private:
    static uint8_t id;
    static uint16_t bodRate;             
    static uint8_t type;
    static uint16_t timeout;
    static uint16_t T35;                
    static uint16_t interval;          
    static uint16_t T240;       
    static void updateTimestamp();
public: 
   
    void init();
    void interruptEEPROM();
    void run();
    void readSensors();
    static uint16_t getValue(uint8_t key);
    static void setValue(uint8_t key, uint8_t highByte, uint8_t lowByte);
};
#endif
